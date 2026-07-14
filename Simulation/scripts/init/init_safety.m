function safety = init_safety(quadcop) %#ok<INUSD>
%INIT_SAFETY  Parameter fuer safety_overspeed (Onboard-Kill-Latch).
arguments (Input)
    quadcop struct % derzeit ungenutzt (war Hover-Schub fuer den verworfenen
                   % Idle-Interlock); Signatur bleibt fuer die Aufrufer stabil.
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

% HINWEIS: rearm_idle_frac/F_rearm_idle (Arming-Idle-Interlock) sind in
% Session 9 entfallen — Begruendung im Schlusskommentar von safety_overspeed.m.
end