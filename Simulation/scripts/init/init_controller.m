function controller = init_controller(quadcop)
%init_controller initializes the position and attitude controller
%   feedback gains
arguments (Input)
    quadcop struct % holding quadrocopter parameters
end

arguments (Output)
    controller struct % holding controller gains
end

% omega_n and zeta are the natural frequency and the damping ratio of the
% damped oscillator the closed loop-system is trying to achieve
omega_n_pos = 0.42*[10; 10; 10];
omega_n_Lage = 1.4*[10; 10; 10];
zeta = 0.707;
controller.kR = diag(quadcop.J * omega_n_Lage.^2);
controller.kOmega =  diag(2 * zeta * quadcop.J * omega_n_Lage);
controller.Kp = diag(quadcop.m * omega_n_pos.^2);
controller.Kd =  diag(2 * zeta * quadcop.m * omega_n_pos);
end