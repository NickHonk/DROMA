function quadcop = init_quadcop()
%init_quadcop initializes the parameters for the quadcopter

arguments (Output)
    quadcop struct
end

quadcop.g   = 9.81; % m/s^2
quadcop.m   = 0.965; % kg            
quadcop.J   = diag([6.583 * 10^-3, 5.125 * 10^-3, 1.104 * 10^-2]); % kg*m^2 
quadcop.J_inv = inv(quadcop.J);
l = 0.124;  % Armlaenge 
c_T = 1.134e-3; % N/(rad/s)^2   
c_tau = 7.398e-6; % Nm/(rad/s)^2 

quadcop.T = [1 0 0; 0 -1 0; 0 0 -1]; % to transform form NED to laboratory coordinate system (all with z-up)

quadcop.x0 = [0;0;0];
quadcop.v0 = [0;0;0];
quadcop.q0 = [1;0;0;0];
quadcop.euler0 = [0;0;0];
quadcop.w0 = [0;0;0];

% Motordrehzaheln -> Schubkraft und Drehmomente:  [F; tau_x; tau_y; tau_z] = Gamma * [w1^2; w2^2; w3^2; w4^2] 
% momentan fliegt Quadrkopter in X-Konfiguration
% konfiguriere Gamma:
alpha = 38.4; % degrees (Winkel aus Lunze/Schwung Paper)
beta = 90 - alpha;
% Quadrcopterkonfiguration ist ein Mix aus H- und X-Konfiguration 
a = l * sin(alpha / 180 * pi);
b = l * cos(alpha / 180 * pi);
% Position der Rotoren im Körperkoordinatensystem
r1 = [ a; -b;  0];
r2 = [ a;  b;  0];
r3 = [-a;  b;  0];
r4 = [-a; -b ; 0];

F = [c_T  c_T  c_T  c_T];
f_i = [0; 0; c_T];
tau_i = [0; 0; c_tau];
tau = [skew(r1)*f_i-tau_i  skew(r2)*f_i+tau_i  skew(r3)*f_i-tau_i  skew(r4)*f_i+tau_i];
quadcop.Gamma = [F; tau];

quadcop.Gamma_inv = inv(quadcop.Gamma);
quadcop.Gamma = 1.2*quadcop.Gamma;


% Motor (PT1) + Saettigung
quadcop.tau_m   = 0.030;             % Motorzeitkonstante  TODO: measure 
quadcop.rotors_min   = 0;            % rad/s
quadcop.rotors_max   = 30000;        % rad/s  


end

% =============================== Helfer ==================================

function schiefsym = skew(r)
%VEE gibt die schiefsymmetrische Matrix schiefsym zu r zurück

arguments (Input)
    r (3,1) double
end

arguments (Output)
    schiefsym (3,3) double
end

schiefsym = [ 0    -r(3)  r(2);
              r(3)  0    -r(1);
             -r(2)  r(1)  0];
end