function run_gate_a(proj_root)
% run_gate_a — Gate A (SIL vs Normal) headless. Funktion, damit die lokalen
% Variablen den 'clear' in params.m (via main-PreLoadFcn) ueberleben.
openProject(fullfile(proj_root,'DROMA.prj'));
load_system('main');                 % PreLoadFcn -> params.m (macht 'clear' im base!)
cd(fullfile(proj_root,'scripts','sitl'));
clear configure_mcu_codegen sil_check_mcu
configure_mcu_codegen('mcu');        % ert_cpp_sitl-Config aktiv setzen (frische Session)
sil_check_mcu;                       % rotor_cmd/led/throttle: Normal vs SIL
end
