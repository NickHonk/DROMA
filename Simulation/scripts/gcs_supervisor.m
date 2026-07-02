function [x_ref, v_ref, a_ref, yaw_ref, estop, mode] = gcs_supervisor( ...
        estop_cmd, p_est, x_ref_traj, v_ref_traj, a_ref_traj, yaw_ref_traj, sup)
%#codegen
% GCS_SUPERVISOR Zustandsautomat
%
% Supervisor-MUX VOR pos_ctrl: waehlt die Sollwertquelle (Trajektorie vs.
% geregelter Soft-Land) und setzt das estop-Feld des Bus_Cmd aktiv. 
%
% ZUSTAENDE (mode):
%   0 NORMAL     : Sollwerte = Trajektorie estop=0
%   1 SOFT_LAND  : x/y eingefrieren, z-Ref rampt mit v_sink runter, v_ref=+v_sink,
%                  yaw halten. estop=1 (onboard sieht Soft-Land flag)
%   2 DISARMED   : Grund erreicht -> estop=2 -> onboard-Cutoff (rotors_cmd=0)
%   3 KILL       : Hard-Kill (estop_cmd==2 von ueberall) -> estop=2
%
% ENTWURFSENTSCHEIDUNGEN
%   (1) Disarm = estop=2 (Cutoff)
%   (3) Soft-Land ist COMMITTED: kein Rueckweg -> NORMAL (traj_gen(t) ist
%       absolut-zeitbasiert -> Rueckweg waere unstetig) -> eskaliert
%       Re-Arm am Boden (Reset / onboard ack).
%
% EINGAENGE:
%   estop_cmd     : uint8  Bediener-Wunsch  0 normal / 1 soft-land / 2 hard-kill
%   p_est         : 3x1    Positionsschaetzung [x;y;z] aus Luenberger
%   x_ref_traj    : 3x1    Trajektorien-Sollposition (Durchleitung in NORMAL)
%   v_ref_traj    : 3x1    Trajektorien-Sollgeschwindigkeit
%   a_ref_traj    : 3x1    Trajektorien-Sollbeschleunigung
%   yaw_ref_traj  : double Trajektorien-Soll-Yaw 
%   sup           : struct .v_sink .z_ground .disarm_margin .Ts
%
% AUSGAENGE (-> pos_ctrl bzw. Bus_Cmd):
%   x_ref, v_ref, a_ref : 3x1    selektierte Sollwerte fuer pos_ctrl
%   yaw_ref             : double selektierter Soll-Yaw
%   estop               : uint8  0/1/2 -> Bus_Cmd.estop (Uplink)
%   mode                : uint8  Zustands-ID (Logging/Debug)
%
% KONVENTION NED: z nach unten. z_ground = Bodenpegel (groesstes z).
% Sinken = z waechst Richtung z_ground. v_sink > 0. Hoehe = -z.

% --- Zustands-IDs ---
NORMAL = uint8(0);
SOFT_LAND = uint8(1);
DISARMED = uint8(2);
KILL = uint8(3);

persistent state x0 y0 z0 yaw0 zref inited
if isempty(inited)
    state = NORMAL;
    x0 = 0.0;   
    y0 = 0.0;   
    z0 = 0.0;
    yaw0 = 0.0;
    zref = 0.0;
    inited = true;
end

% --- Hard-Kill gewinnt immer, aus jedem Zustand ---
if estop_cmd == uint8(2)
    state = KILL;
end

% --- Transitionen + zustandslokale Aktualisierung ---
switch state
    case NORMAL
        if estop_cmd == uint8(1) % Soft-Land ausloesen
            state = SOFT_LAND;
            x0 = p_est(1); % Horizontalposition einfrieren
            y0 = p_est(2);
            z0 = p_est(3);
            yaw0 = yaw_ref_traj; % aktuellen Soll-Yaw halten
            zref = z0; % z-Rampe startet auf aktueller Hoehe
        end

    case SOFT_LAND
        zref = zref + sup.v_sink * sup.Ts;
        if zref > sup.z_ground 
            zref = sup.z_ground;
        end
        % Disarm, sobald knapp ueber Grund (Mocap/Luenberger kennt Hoehe)
        if p_est(3) >= sup.z_ground - sup.disarm_margin
            state = DISARMED;
        end

    case DISARMED
        % terminal: estop=2 nullt onboard die Motoren

    otherwise % KILL
        % terminal
end

% --- Ausgangs-Mux nach Zustand ---
switch state
    case NORMAL
        x_ref   = x_ref_traj;
        v_ref   = v_ref_traj;
        a_ref   = a_ref_traj;
        yaw_ref = yaw_ref_traj;
        estop   = uint8(0);

    case SOFT_LAND
        x_ref   = [x0; y0; zref];       
        v_ref   = [0.0; 0.0; sup.v_sink];
        a_ref   = [0.0; 0.0; 0.0];
        yaw_ref = yaw0;
        estop   = uint8(1);

    case DISARMED
        x_ref   = [x0; y0; sup.z_ground];
        v_ref   = [0.0; 0.0; 0.0];
        a_ref   = [0.0; 0.0; 0.0];
        yaw_ref = yaw0;
        estop   = uint8(2);              

    otherwise % KILL
        x_ref   = p_est;                 
        v_ref   = [0.0; 0.0; 0.0];
        a_ref   = [0.0; 0.0; 0.0];
        yaw_ref = yaw0;
        estop   = uint8(2);
end

mode = state;
end