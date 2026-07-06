function supervisor = init_supervisor(quadcop,Ts_gcs)
%init_supervisor gets triggered, when user in SIMULINK sets estop=1/=2 ->
%   safety landing is performed. 
arguments (Input)
    quadcop struct % holding quadrocopter related parameters
    Ts_gcs (1,1) double % sample time of the ground station for the integration used in the safety landing trajectory
end

arguments (Output)
    supervisor struct 
end

% geregeltes Soft-Land mit ~0.3..0.5 m/s 
supervisor.v_sink = 0.15; % [m/s]  Soll-Sinkrate

% Falls Mocap-z-Null nicht am Boden liegt, hier den realen Bodenwert setzen.
supervisor.z_ground = 0.0; % [m]  z-Koordinate des Bodens 

% Disarm-Marge ueber Grund: Cutoff (estop=2) bei z_est <= z_ground + margin.
% Aufschlaggeschwindigkeit:  v_imp = sqrt(v_sink^2 + 2*g*margin)
%   margin=0.08, v_sink=0.35 -> v_imp ~ 1.29 m/s
% Kleiner margin => sanfter, aber mehr Bodeneffekte. 
supervisor.disarm_margin = 0.2;       % [m]

supervisor.Ts = Ts_gcs; % [s]

% --- Info-Ausgabe ---
v_imp = sqrt(supervisor.v_sink^2 + 2*quadcop.g*supervisor.disarm_margin);
fprintf(['Supervisor: v_sink=%.2f m/s, margin=%.3f m, z_ground=%.2f m ' ...
         '-> v_impact~%.2f m/s\n'], supervisor.v_sink, supervisor.disarm_margin, ...
         supervisor.z_ground, v_imp);
assert(supervisor.v_sink > 0, 'v_sink muss > 0 sein.');
assert(supervisor.disarm_margin > 0, 'disarm_margin muss > 0 sein.');
end