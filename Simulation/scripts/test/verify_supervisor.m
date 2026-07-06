%% verify_supervisor.m  --  modellfreie Verifikation der GCS-Mode-Maschine
%  Ruft gcs_supervisor.m direkt (dieselbe Quelle wie der Simulink-Block).
%  Zwischen Testfaellen 'clear gcs_supervisor' -> persistent-Reset.
%  Pruefstil analog verify_overspeed.m / verify_battery.m.
clear; clc;

% --- sup-Struct (wie params_supervisor_snippet.m) ---
g   = 9.81;
Ts_gcs = 0.01;
sup = struct('v_sink',0.35,'z_ground',0.0,'disarm_margin',0.08,'Ts',Ts_gcs);

pass = true;
function_check = @(name,cond) fprintf('  [%s] %s\n', ternary(cond,'OK ','FAIL'), name);

fprintf('==== TEIL A  Zustandsmaschinen-Invarianten ====\n');

% A1 NORMAL leitet Trajektorie durch, estop=0
clear gcs_supervisor
[xr,vr,ar,yr,es,md] = gcs_supervisor(uint8(0), [1;2;-1], [9;8;-7], ...
    [.1;.2;.3], [0;0;0], 0.5, sup);
pass = pass & report('NORMAL leitet x_ref_traj durch', isequal(xr,[9;8;-7]));
pass = pass & report('NORMAL: estop==0 & mode==0', es==uint8(0) && md==uint8(0));

% A2 Trigger -> SOFT_LAND: x/y eingefroren, yaw gehalten, estop=1
clear gcs_supervisor
gcs_supervisor(uint8(0), [0;0;-1.5], [0;0;-1.5], [0;0;0], [0;0;0], 0.7, sup);
[xr,vr,ar,yr,es,md] = gcs_supervisor(uint8(1), [0.3;-0.2;-1.5], [0;0;-1.5], ...
    [0;0;0], [0;0;0], 0.9, sup);
pass = pass & report('SOFT_LAND friert x/y auf p_est ein', ...
    abs(xr(1)-0.3)<1e-12 && abs(xr(2)+0.2)<1e-12);
pass = pass & report('SOFT_LAND haelt yaw (=yaw_traj@trigger)', abs(yr-0.9)<1e-12);
pass = pass & report('SOFT_LAND: v_ref==[0;0;v_sink]', isequal(vr,[0;0;sup.v_sink]));
pass = pass & report('SOFT_LAND: estop==1 & mode==1', es==uint8(1) && md==uint8(1));

% A3 z_ref rampt monoton Richtung Boden, klemmt bei z_ground
zprev = xr(3); mono = true;
for k=1:3000
    [xr,~,~,~,~,~] = gcs_supervisor(uint8(1), [0.3;-0.2;-1.5], [0;0;0], ...
        [0;0;0], [0;0;0], 0.0, sup);
    mono = mono && (xr(3) >= zprev - 1e-15); zprev = xr(3);
end
pass = pass & report('z_ref monoton nicht-fallend', mono);
pass = pass & report('z_ref bei z_ground geklemmt', abs(xr(3)-sup.z_ground)<1e-12);

% A4 Disarm bei Grund -> estop=2
clear gcs_supervisor
gcs_supervisor(uint8(0), [0;0;-1], [0;0;0],[0;0;0],[0;0;0], 0, sup);
gcs_supervisor(uint8(1), [0;0;-1], [0;0;0],[0;0;0],[0;0;0], 0, sup);
[~,~,~,~,es,md] = gcs_supervisor(uint8(1), [0;0;-0.05], [0;0;0],[0;0;0],[0;0;0], 0, sup);
pass = pass & report('Grund erreicht -> DISARMED, estop==2', es==uint8(2) && md==uint8(2));

% A5 COMMITTED: estop_cmd=0 fuehrt NICHT zurueck nach NORMAL
clear gcs_supervisor
gcs_supervisor(uint8(0), [0;0;-1], [0;0;0],[0;0;0],[0;0;0], 0, sup);
gcs_supervisor(uint8(1), [0;0;-1], [0;0;0],[0;0;0],[0;0;0], 0, sup);
[~,~,~,~,es,md] = gcs_supervisor(uint8(0), [0;0;-1], [0;0;0],[0;0;0],[0;0;0], 0, sup);
pass = pass & report('Soft-Land committed (estop_cmd=0 bleibt SOFT_LAND)', ...
    md==uint8(1) && es==uint8(1));

% A6 KILL sticht aus jedem Zustand, terminal
clear gcs_supervisor
gcs_supervisor(uint8(0), [0;0;-1], [0;0;0],[0;0;0],[0;0;0], 0, sup);
gcs_supervisor(uint8(1), [0;0;-1], [0;0;0],[0;0;0],[0;0;0], 0, sup);
[~,~,~,~,es,md] = gcs_supervisor(uint8(2), [0;0;-1], [0;0;0],[0;0;0],[0;0;0], 0, sup);
pass = pass & report('estop_cmd=2 -> KILL, estop==2', md==uint8(3) && es==uint8(2));
[~,~,~,~,es,md] = gcs_supervisor(uint8(0), [0;0;-1], [0;0;0],[0;0;0],[0;0;0], 0, sup);
pass = pass & report('KILL terminal (estop_cmd=0 bleibt KILL)', md==uint8(3) && es==uint8(2));

% =====================================================================
% TEIL B  vertikaler geschlossener Kreis: geregelte Sinkrate ~ v_sink
% =====================================================================
fprintf('==== TEIL B  Geregelter Soft-Land (vertikaler Kreis) ====\n');
m=0.965; zeta=0.707; wn=0.42*10; Kp=m*wn^2; Kd=2*zeta*m*wn;
Ts_inner=1/1000; tau_thrust=0.03; dt=Ts_inner; gcs_dec=round(Ts_gcs/dt);
z=-1.5; w=0; Fz=-m*g; clear gcs_supervisor
xr=[0;0;-1.5]; vr=[0;0;0]; md=uint8(0); wlog=[]; zlog=[]; tlog=[]; mdlog=[];
for k=0:round(8/dt)
    t=k*dt;
    if mod(k,gcs_dec)==0
        ec = uint8(t>=1.0);
        [xr,vr,~,~,~,md]=gcs_supervisor(ec,[0;0;z],[0;0;-1.5],[0;0;0],[0;0;0],0,sup);
    end
    Fz_cmd = m*(0 - g - Kp*(z-xr(3)) - Kd*(w-vr(3)));
    Fz = Fz + (dt/tau_thrust)*(Fz_cmd-Fz);
    zdd = g + Fz/m; w=w+zdd*dt; z=z+w*dt;
    tlog(end+1)=t; zlog(end+1)=z; wlog(end+1)=w; mdlog(end+1)=double(md); %#ok<*SAGROW>
    if md==uint8(2), break; end
end
sel = (tlog>2.0) & (mdlog==1);
w_ss = mean(wlog(sel));
fprintf('  w_ss = %.4f m/s (Soll %.2f), Abw = %.1f mm/s\n', ...
    w_ss, sup.v_sink, abs(w_ss-sup.v_sink)*1000);
pass = pass & report('geregelte Sinkrate ~ v_sink (<30 mm/s)', abs(w_ss-sup.v_sink)<0.03);
pass = pass & report('keine Divergenz (max w < 1.5 v_sink)', max(wlog(mdlog==1))<1.5*sup.v_sink);
v_imp = sqrt(wlog(end)^2 + 2*g*sup.disarm_margin);
fprintf('  Disarm bei z=%.3f m, w=%.3f m/s -> v_impact~%.2f m/s\n', ...
    zlog(end), wlog(end), v_imp);

fprintf('==== GESAMT: %s ====\n', ternary(pass,'ALLE GRUEN','FEHLER'));

%% ---- kleine Helfer ----
function s = ternary(c,a,b); if c, s=a; else, s=b; end; end
function p = report(name,cond)
    fprintf('  [%s] %s\n', ternary(cond,'OK ','FAIL'), name); p = cond;
end