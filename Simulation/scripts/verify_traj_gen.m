%% verify_traj_gen.m  --  Offline-Gegencheck fuer traj_gen.m (MATLAB Function Block in gcu)
%  Prueft OHNE Simulink:
%   (1) Positionsableitungen (FD) vs v_ref, a_ref         -> Polynom-Skalierung
%   (2) Omega_ref (Matrixform) vs 2*vec(qConj(q_ref)⊗q̇)  -> Flachheitskette/Vorzeichen
%   (3) tau_ref vs J*d/dt(Omega_ref) + Omega x J Omega    -> Omega_dot-Extraktion
%   (4) Rast-Invarianten: Omega=0, tau=0, F=m*g
%  + Plots. traj_gen.m muss auf dem Pfad liegen.
clear; clc; close all;

%% --- Parameter (sonst: run params.m / traj_setup.m und prm/traj uebernehmen) ---
prm.m = 0.93;
prm.g = 9.81;
prm.J = diag([6.583e-3, 5.125e-3, 1.104e-2]);

traj.P = [  0   1   1   0   0 ;     % x [m]   (P(:,1) = 6-DOF-IC)!
            0   0   1   1   0 ;     % y [m]
            1   1   1   1   1 ];    % z [m]  
traj.yaw    = deg2rad([ 0   0   0   0 ]);
traj.Tseg   = [ 2.0  2.0  2.0  2.0 ];
traj.Tdwell = [ 1.0  0.5  0.5  0.5  2.0 ];

%% --- Zeitachse (fein fuer zentrale Differenzen) ---
Ttot = sum(traj.Tseg) + sum(traj.Tdwell);
h = 1e-4;
t = (0:h:Ttot).';
Nt = numel(t);

X=zeros(Nt,3); V=zeros(Nt,3); A=zeros(Nt,3); YAW=zeros(Nt,1);
OM=zeros(Nt,3); TAU=zeros(Nt,3); Q=zeros(Nt,4); F=zeros(Nt,1);
for i=1:Nt
    [x,v,a,yaw,Om,tau,q,Fr] = traj_gen(t(i), traj, prm);
    X(i,:)=x.';  V(i,:)=v.';  A(i,:)=a.';  YAW(i)=yaw;
    OM(i,:)=Om.';  TAU(i,:)=tau.';  Q(i,:)=q.';  F(i)=Fr;
end

% Quaternion-Vorzeichen kontinuierlich machen (Doppeldeckung), vor dem Ableiten
for i=2:Nt
    if Q(i,:)*Q(i-1,:).' < 0, Q(i,:) = -Q(i,:); end
end

%% --- (1) Positionsableitungen ---
Vn = ctrdiff(X,h);   An = ctrdiff(V,h);
e_v = maxerr(Vn-V, t, traj);
e_a = maxerr(An-A, t, traj);

%% --- (2) Omega_ref vs Quaternion-Formel  [HAUPTCHECK] ---
Qd  = ctrdiff(Q,h);                         % q̇_ref numerisch
OMq = zeros(Nt,3);
for i=1:Nt
    dq = quatMul(quatConj(Q(i,:).'), Qd(i,:).');   % qConj(q) ⊗ q̇
    OMq(i,:) = (2*dq(2:4)).';
end
e_Om = maxerr(OMq-OM, t, traj);

%% --- (3) tau_ref vs J*Omega_dot + Omega x J Omega ---
OMd  = ctrdiff(OM,h);
TAUn = zeros(Nt,3);
for i=1:Nt
    w = OM(i,:).';
    TAUn(i,:) = (prm.J*OMd(i,:).' + cross(w, prm.J*w)).';
end
e_tau = maxerr(TAUn-TAU, t, traj);

%% --- (4) Rast-Invarianten (erste Rast an WP1) ---
ir = t < traj.Tdwell(1);
fprintf('\n=== Cross-Check (max. Fehler im Segment-Inneren, Raender maskiert) ===\n');
fprintf(' (1) |v_num  - v_ref |  = %.3e  m/s\n',   e_v);
fprintf(' (1) |a_num  - a_ref |  = %.3e  m/s^2\n', e_a);
fprintf(' (2) |Omega_q- Omega |  = %.3e  rad/s   <-- Flachheitskette/Vorzeichen\n', e_Om);
fprintf(' (3) |tau_num- tau_ref| = %.3e  Nm\n',    e_tau);
fprintf('\n=== Rast WP1 (sollte ~0 / ~0 / m*g sein) ===\n');
fprintf(' max|Omega_ref| = %.2e rad/s\n', max(vecnorm(OM(ir,:),2,2)));
fprintf(' max|tau_ref|   = %.2e Nm\n',    max(vecnorm(TAU(ir,:),2,2)));
fprintf(' F_ref          = %.4f N   (m*g = %.4f N)\n', mean(F(ir)), prm.m*prm.g);

%% --- Plots ---
figure('Name','Position & Yaw');
subplot(4,1,1); plot(t,X); ylabel('x_{ref} [m]'); legend('x','y','z'); grid on; title('Trajektorie');
subplot(4,1,2); plot(t,V); ylabel('v_{ref} [m/s]'); grid on;
subplot(4,1,3); plot(t,A); ylabel('a_{ref} [m/s^2]'); grid on;
subplot(4,1,4); plot(t,rad2deg(YAW)); ylabel('yaw [deg]'); xlabel('t [s]'); grid on;

figure('Name','Omega-Check');
lbl={'\Omega_x','\Omega_y','\Omega_z'};
for k=1:3
    subplot(3,1,k); plot(t,OM(:,k),'-', t,OMq(:,k),'--'); grid on;
    ylabel([lbl{k} ' [rad/s]']);
    if k==1, legend('traj\_gen (Matrix)','Quaternion-Formel'); title('Omega_{ref}: Matrix vs. Quaternion'); end
end
xlabel('t [s]');

figure('Name','tau_ref & F_ref');
subplot(2,1,1); plot(t,TAU); ylabel('\tau_{ref} [Nm]'); legend('\tau_x','\tau_y','\tau_z'); grid on;
title('\tau_{ref} (Spruenge an Wegpunktuebergaengen erwartet)');
subplot(2,1,2); plot(t,F); yline(prm.m*prm.g,'--'); ylabel('F_{ref} [N]'); xlabel('t [s]'); grid on;

%% ============================ Lokale Helfer ============================
function d = ctrdiff(Y,h)
% Zentrale Differenz spaltenweise; Raender einseitig.
    d = zeros(size(Y));
    d(2:end-1,:) = (Y(3:end,:) - Y(1:end-2,:)) / (2*h);
    d(1,:)   = (Y(2,:)   - Y(1,:))     / h;
    d(end,:) = (Y(end,:) - Y(end-1,:)) / h;
end

function e = maxerr(Err, t, traj, win)
% Max. Zeilennorm des Fehlers, Phasengrenzen +-win ausgeblendet
% (dort springt der Snap -> numerische Ableitung schmiert).
    if nargin < 4, win = 5e-3; end
    bnd = boundaries(traj);
    mask = true(size(t));
    for b = bnd
        mask(abs(t-b) < win) = false;
    end
    E = sqrt(sum(Err.^2,2));
    e = max(E(mask));
end

function bnd = boundaries(traj)
    N = size(traj.P,2);
    bnd = 0; acc = 0;
    for i=1:N
        acc = acc + traj.Tdwell(i); bnd(end+1)=acc; %#ok<AGROW>
        if i<N, acc = acc + traj.Tseg(i); bnd(end+1)=acc; end %#ok<AGROW>
    end
end

function c = quatMul(a,b)
% Hamilton-Produkt, Skalar zuerst. MUSS mit deinem Projekt-quatMul uebereinstimmen.
    a0=a(1); av=a(2:4); b0=b(1); bv=b(2:4);
    c = [ a0*b0 - av.'*bv;
          a0*bv + b0*av + cross(av,bv) ];
end

function qc = quatConj(q)
    qc = [q(1); -q(2); -q(3); -q(4)];
end