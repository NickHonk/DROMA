function safety = init_battery_manag(quadcop, safety, Ts_batt)
%init_battery_manag initializes parameters for the battery monitoring
%   (blinking LED when battery voltage drops) and the hard landing when the
%   battery voltage drops below a certain threshold
arguments (Input)
    quadcop struct % holding quadrocopter related parameters
    safety struct % already holding safety related parameters 
    Ts_batt (1,1) double % sample time for the Hyterese, so that sudden battery voltage drops don't lead to false forced landing
end

arguments (Output)
    safety struct 
end

% --- ADC / HW (PM06 V2, Teensy) ---
safety.batt_pin = 40; % Pin 40 = A16 
safety.adc_bits = 12; % analogReadResolution(12)

% V_batt = k*count + b (Spannungsteiler 18.182 von PM06 V2, Vref 3.3, 12 bit):
% k = 3.3*18.182/4095 ≈ 0.0146521 V/count,  b = 0.
% k,b aus realer HW-Messung NOCH OFFEN -> hier Idealwerte als Platzhalter.
safety.batt_k = 3.3*18.182/4095; % Steigung
safety.batt_b = 0.0; % offset
 
% --- Tiefpass ---
safety.batt_tau   = 0.7; % gegen Last-Einbruch + Rauschen (0.5..1)
safety.batt_alpha = 1 - exp(-Ts_batt/safety.batt_tau);  % Hysterese-Koeffizient
 
% --- Schwellen (4S LiPo, unter Last). final auf HW bestaetigen ---
safety.V_warn = 14.0; % 3.50 V/Zelle -> LED WARN, Bediener handeln
safety.V_crit = 13.4; % 3.35 V/Zelle -> LED CRIT
safety.V_floor = 12.0; % 3.00 V/Zelle -> onboard Hard-Floor-Descent
safety.V_hyst = 0.2; % Hysterese/Recovery-Band gegen Chattering
 
% --- Harter Sinklflug ---
safety.hardfloor_thrust_frac = 0.99; % F_des = 0.98*m*g (ueberlebbare Sinkrate).
safety.m = quadcop.m;
safety.g = quadcop.g;
 
% Quervalidierung Bereich: V_pin(4S) = 0.726..0.924 V -> count 901..1147
% (~22..28 % des 12-bit-Bereichs). Keine Ueberspannung am Pin (<3.3 V).
end