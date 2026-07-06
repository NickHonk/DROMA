function [led, batt_land, V_filt] = safety_battery(batt_count, safety)
%#codegen
% safety_battery Onboard-Batterie-Monitor, 4S LiPo via PM06 V2.
%
% Kette: ADC-count --> V_batt --> V_filt --> led
%        V_filt <= V_floor --(Latch bis Reboot) --> batt_land
%
%   WARN/CRIT -> LED. Bediener im Raum sieht sie, loest MANUELL GCS-Soft-Land
%               (estop=1 => Uplink) aus, solange Marge da ist.
%   FLOOR     -> batt_land = true -> onboard BLINDER Hard-Floor-Descent 
%               (safety_landcmd.m) als Backstop, falls niemand reagiert.
% KILL (Overspeed/Hard-Kill) dominiert: rotors_cmd=0 nachgelagert sticht jeden LAND.
%
% LATCH IST PERMANENT —> KEIN Re-Arm. Zwei Gruende:
%   1) Akkuwechsel = Teensy-Reboot -> persistent wird genullt.
%   2) WICHTIGER: Im Descent faellt der Schub auf 0.98*m*g -> weniger Strom ->
%      V erholt sich UEBER den Floor. Ohne Latch wuerde batt_land deasserten,
%      das GCS-Kommando (Hover @ m*g) kaeme zurueck, Last steigt, V sackt wieder
%      -> GRENZZYKLUS Sinken<->Schweben auf fast leerem Akku. Der Latch verhindert
%      das: einmal committed -> bis zum Boden sinken.
%
% Eingaenge:
%   batt_count : ADC-Rohwert (12 bit, 0..4095). In Sim aus simulierter V-Rampe
%                (count = round((V_batt - b)/k)); auf HW analogRead(A16/Pin40).
%   safety     : struct  .batt_k .batt_b .batt_alpha .V_warn .V_crit .V_floor
%                        .V_hyst
% Ausgaenge:
%   led        : uint8  0 NORMAL / 1 WARN / 2 CRIT   (-> GPIO-Blinkmuster)
%   batt_land  : bool   latched -> safety_landcmd Hard-Floor-Override
%   V_filt     : double gefilterte Batteriespannung [V]  (dbg/logging)
%
% Hysterese: Recovery (Richtung NORMAL) braucht + V_hyst -> kein Flattern bei
% Last-Sag/Rauschen. (tau ~0.5..1 s) glaettet Sag + Rauschen UND verhindert,
% dass ein kurzer Spannungseinbruch unter Last den Floor faelschlich triggert.

persistent Vf state landed
if isempty(Vf)
    Vf     = safety.batt_k * double(batt_count) + safety.batt_b;  % init
    state  = uint8(0);
    landed = false;
end

% --- ADC -> Spannung ---
V_raw = safety.batt_k * double(batt_count) + safety.batt_b;

% --- Tiefpass: V_filt += alpha*(V_raw - V_filt) ---
Vf = Vf + safety.batt_alpha * (V_raw - Vf);
V  = Vf;

% --- 3-stufige LED mit Hysterese (Batterienzeige) ---
% NORMAL(0) -> WARN(1) -> CRIT(2); Rueckweg braucht + V_hyst.
switch state
    case uint8(0) % NORMAL
        if V <= safety.V_warn                     
            state = uint8(1); 
        end
    case uint8(1) % WARN
        if V <= safety.V_crit                 
            state = uint8(2);
        elseif V >= safety.V_warn + safety.V_hyst
            state = uint8(0); 
        end
    otherwise % CRIT (2)
        if V >= safety.V_crit + safety.V_hyst     
            state = uint8(1); 
        end
end

% --- harte Landung ---
if V <= safety.V_floor
    landed = true;
end

led       = state;
batt_land = landed;
V_filt    = V;
end