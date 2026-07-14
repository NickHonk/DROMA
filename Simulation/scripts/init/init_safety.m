function safety = init_safety(quadcop)
%INIT_SAFETY  Parameter fuer safety_overspeed (Onboard-Kill-Latch + Idle-Interlock).
arguments (Input)
    quadcop struct % fuer Hover-Schub m*g (Idle-Interlock-Schwelle)
end
arguments (Output)
    safety struct % holding parameters for the safety function
end

% 8.5 rad/s (~487 deg/s) per Achse ggf. per-Achse differenzieren
safety.omega_max = 8.5; % [rad/s]

% N aufeinanderfolgende Samples gegen Gyro-Spikes
safety.debounce_N = uint16(4);

% Detektor-Modus: false = per-Achse |Omega_i| (empfohlen, achsselektiv),
%                 true  = Euklidische Norm ||Omega||.
safety.use_norm = true;

% Arming-Idle-Interlock: Re-Arm (ack-Flanke) loest den Kill-Latch nur, wenn der
% befohlene Schub F_des <= F_rearm_idle ("Schub runter zum Armen"). Verhindert
% den throttle-Sprung-auf-Hover im Loesetick. Default 10 % Hover.
safety.rearm_idle_frac = 0.1;
safety.F_rearm_idle    = safety.rearm_idle_frac * quadcop.m * quadcop.g; % [N]
end