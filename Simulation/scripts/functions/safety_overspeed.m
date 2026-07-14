function [kill, fault_src, dbg] = safety_overspeed(gyro_corr, estop, ack, safety)
%#codegen
% SAFETY_OVERSPEED  Onboard-KILL-Latch
%
% Overspeed-Entprell-Latch ∪ Hard-Kill (estop==2).
% Aktion downstream: rotors_cmd = 0  (Override NACH Mixer / VOR Motor-PT1+ESC).
% KILL dominiert LAND; KILL latcht; Re-Arm NIE in der Luft.
%
% Warum eine Funktion fuer beide KILL-Quellen: Overspeed und Hard-Kill teilen
% denselben Latch, dieselbe Aktion (rotors_cmd=0) und dieselbe Re-Arm-Semantik
% (ack loescht). Eine gemeinsame Funktion haelt die Re-Arm-Bedingung an EINER
% Stelle und macht "KILL dominiert LAND" trivial. Der geregelte Soft-Land
% (estop==1) wird hier NICHT behandelt — das ist die GCS-Mode-Maschine.
%
% Eingaenge
%   gyro_corr : 3x1  bias-korrigierte Drehrate [rad/s]  (MESSUNG, nicht Schaetzer!)
%   estop : uint8  0 normal / 1 soft-land / 2 hard-kill (aus Bus_Cmd, Uplink)
%   ack : bool   Quittung, bereits ge-OR-t (Teensy-Taster-Flanke | Bus_Cmd.ack)
%   safety : struct  .omega_max [rad/s], .debounce_N (>=1), .use_norm (bool)
%
% Ausgaenge
%   kill : bool   latched -> nachgelagerter Switch zwingt rotors_cmd=0
%   fault_src : uint8  0 none / 1 overspeed / 2 hard-kill   (LED/Debug)
%   dbg : 3x1    [cnt; over_inst; ack_edge]           (verify/logging)
%
% Re-Arm (FAULT->ARMED) NUR bei:  steigende ack-Flanke  &  ~over_inst  &  estop~=2.
% Die ack-FLANKE (nicht der Pegel) verhindert, dass ein gehaltenes ack einen
% frischen Trip sofort wieder loescht oder mid-air in einen Sturz re-armt. Die
% Boden-Bedingung ("nie in der Luft") wird prozedural durch den physischen
% Teensy-Taster garantiert (Bediener hebt die Drohne auf, drueckt) — onboard
% existiert keine Pos/Vel-Schaetzung fuer ein Logik-Interlock (sitzt GCS-seitig).
% Zum verworfenen Arming-Idle-Interlock siehe Hinweis am Dateiende.

persistent latched cnt ack_prev src
if isempty(latched)
    latched = false;
    cnt = uint16(0);
    ack_prev = false;
    src = uint8(0);
end

gw = reshape(gyro_corr, 3, 1);

% --- Overspeed-Detektor, entprellt (N aufeinanderfolgende Samples) ---
if safety.use_norm
    over_inst = sqrt(gw(1)*gw(1) + gw(2)*gw(2) + gw(3)*gw(3)) > safety.omega_max;
else
    over_inst = (abs(gw(1)) > safety.omega_max) || ...
                (abs(gw(2)) > safety.omega_max) || ...
                (abs(gw(3)) > safety.omega_max);
end

Nreq = uint16(safety.debounce_N);
if over_inst
    if cnt < Nreq
        cnt = cnt + uint16(1);
    end
else
    cnt = uint16(0);                 % ein gutes Sample setzt den Zaehler zurueck
end
over_deb = cnt >= Nreq;

% --- Hard-Kill: sofort, keine Entprellung ---
hard_kill = (estop == uint8(2));

% --- KILL setzen (latcht; Quelle nur beim ersten Setzen vermerkt) ---
if over_deb && ~latched
    latched = true;  
    src = uint8(1); % due to fast turning rates from gyro
end
if hard_kill && ~latched
    latched = true;  
    src = uint8(2);
end

% Re-Arm: steigende ack-Flanke + kein Overspeed + kein Hard-Kill.
ack_edge = ack && ~ack_prev;
if latched && ack_edge && ~over_inst && (estop ~= uint8(2))
    latched = false;
    cnt     = uint16(0);
    src     = uint8(0);
end
ack_prev = ack;

kill      = latched;
fault_src = src;
dbg       = [double(cnt); double(over_inst); double(ack_edge)];
end

% -------------------------------------------------------------------------
% ARMING-IDLE-INTERLOCK — ERPROBT UND VERWORFEN (Session 9). Nicht ohne neue
% Argumente wieder einbauen. Die Variante forderte zusaetzlich
% F_des <= safety.F_rearm_idle (=0.1*m*g) fuer den Re-Arm.
%
% Messung (F_des-Sweep gegen mcu.slx, level, gyro_corr=0):
%   - Schwelle griff bit-exakt bei 0.946665 N ('<=' inklusiv).
%   - ABER throttle im Loese-Tick ist NICHT 0: polyval(p_from_omega_sq, 0) =
%     8.404 % (konstanter Term). Bei OneShot125 ~555 counts / 135 us, also UEBER
%     der Anlaufschwelle (~5-10 %) -> die Props laufen beim Re-Armen ohnehin an.
%     "Schub runter zum Armen" macht das Loesen also nicht motorfrei.
%   - Gewinn war nur 9.94 % statt 23.43 % throttle (13.5 Prozentpunkte).
% Preis: der physische Taster (Pin 21) — die einzige LOKALE Freigabe — wurde
% wirkungslos, sobald die GCS >10 % Hover sendet, ohne jede Rueckmeldung am
% Geraet. Lokale Sicherheitsfunktion an einen Remote-Zustand zu koppeln wog
% schwerer als 13.5 Punkte throttle. Daher: Re-Arm haengt allein an
% ack-Flanke & ~over_inst & estop~=2.