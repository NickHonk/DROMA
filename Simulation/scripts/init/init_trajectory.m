function traj = init_trajectory()
%init_trajectory initializes the trajectory the quadcopter shall follow
%   with the according dwell times between adjacent trajectories
arguments (Output)
    traj struct % holding parameters for the trahectory
end

traj.P = [  0   1   1   0   0 ; % x [m]
            0   0   1   1   0 ; % y [m]
            0   1   1   1   1 ]; % z [m]  
% traj.P = [  0   0   0   0   0 ; % x [m]
%             0   0   0   0   0 ; % y [m]
%             0   0   0   0   0 ]; % z [m]  

% Yaw konstant je Segment (N-1 Werte) [rad]
traj.yaw    = deg2rad([ 0   0   0   0 ]);

% Bewegungsdauer je Segment (N-1 Werte) [s]
traj.Tseg   = [ 7.0  7.0  7.0  7.0 ];

% Rastdauer je Wegpunkt (N Werte)  -- erster Wert = Anfangs-Hover
traj.Tdwell = [ 10.0  2.0  2.0  2.0  2.0 ];

% Sanity-Checks (offline)
assert(size(traj.P,2) >= 2,                 'traj.P braucht >= 2 Wegpunkte');
assert(numel(traj.yaw)    == size(traj.P,2)-1, 'traj.yaw: N-1 Werte');
assert(numel(traj.Tseg)   == size(traj.P,2)-1, 'traj.Tseg: N-1 Werte');
assert(numel(traj.Tdwell) == size(traj.P,2),   'traj.Tdwell: N Werte');

fprintf('Trajektorie: %d Wegpunkte, Gesamtdauer %.2f s\n', ...
        size(traj.P,2), sum(traj.Tseg)+sum(traj.Tdwell));
end