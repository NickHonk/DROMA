function safety = init_safety()
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
arguments (Output)
    safety struct % holding parameters for the safety function
end

% 10 rad/s (~573 deg/s) per Achse ggf. per-Achse differenzieren
safety.omega_max = 10.0; % [rad/s]

% N aufeinanderfolgende Samples gegen Gyro-Spikes
safety.debounce_N = uint16(4);
 
% Detektor-Modus: false = per-Achse |Omega_i| (empfohlen, achsselektiv),
%                 true  = Euklidische Norm ||Omega||.
safety.use_norm = true;
end