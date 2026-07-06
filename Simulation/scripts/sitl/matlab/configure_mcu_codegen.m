%% configure_mcu_codegen.m — Embedded-Coder-Config fuer mcu.slx (Onboard-MCU).
%  Setzt die maszgeblichen Codegen-Optionen als benannten ConfigSet und haengt
%  ihn an mcu.slx. GEN-ONLY: erzeugt C++-Quelle + packNGo-ZIP fuer den host-
%  seitigen SITL-Loop; kein Ziel-Compile aus MATLAB.
%
%  Entscheidungen (gelockt, siehe README §Decision-Table):
%   - ert.tlc (Embedded Coder), Sprache C++  -> reusable Model-Klasse 'MCU'.
%   - Solver: DISCRETE, fixed-step, Basisrate Ts_inner = 1/1000 s (params.m).
%     Alle Safety-Raten sind ganzzahlige Vielfache (Ts_batt=100*Ts_gcs etc.).
%   - Array-Layout COLUMN-major (Default) — konsistent mit gen_lib_codegen.m
%     und dem C++-Golden-Test-Adapter.
%   - SupportNonFinite=false, kein MAT-File-Logging (embedded-clean).
%   - packNGo -> ein ZIP, das der GoogleTest/CTest-Host-Harness zieht.
%
%  Voraussetzung: mcu.slx laedt params.m via PreLoadFcn (Bus-Objekte im .sldd).

function configure_mcu_codegen(mdl)
if nargin < 1, mdl = 'mcu'; end
load_system(mdl);

cs = getActiveConfigSet(mdl);
cs = copy(cs); cs.Name = 'ert_cpp_sitl';

% --- Zielsprache / Target ---
set_param(cs,'SystemTargetFile','ert.tlc');
set_param(cs,'TargetLang','C++');
set_param(cs,'CodeInterfacePackaging','C++ class');
set_param(cs,'GenCodeOnly','on');
set_param(cs,'PackageGeneratedCodeAndArtifacts','on');   % packNGo-ZIP

% --- Solver (an params.m ausrichten) ---
set_param(cs,'SolverType','Fixed-step');
set_param(cs,'Solver','FixedStepDiscrete');
set_param(cs,'FixedStep','Ts_inner');                    % 1/1000 s

% --- Embedded-clean ---
set_param(cs,'SupportNonFinite','off');
set_param(cs,'MatFileLogging','off');
set_param(cs,'GenerateReport','on');
set_param(cs,'GenerateComments','on');
set_param(cs,'ArrayLayout','Column-major');              % Default, explizit gesetzt

% --- Reproduzierbarkeit host<->target: keine schnellen, unsauberen Optimierungen ---
% (Auf HW zusaetzlich Compiler ohne -ffast-math, FPU round-to-nearest.)

attachConfigSet(mdl, cs, true);
setActiveConfigSet(mdl, cs.Name);
fprintf('ConfigSet "%s" an %s gehaengt und aktiv.\n', cs.Name, mdl);

% --- Entry-Point-Kontrakt (Doku; nach slbuild pruefen) ---
% Erwartete generierte C++-Klasse (grob):
%   class MCU {
%     public: void initialize();
%             void step();          % ruft die Safety-Leafs @ ihren Raten
%             void terminate();
%             % I/O ueber ExternalInputs/Outputs-structs (Bus_Cmd, Bus_IMU, ...)
%   };
% Der SITL-Host-Loop instanziiert MCU, verdrahtet Bus_Cmd/Bus_IMU/Bus_Mocap
% gegen die simulierte Strecke und taktet step() mit Ts_inner.
fprintf(['Naechster Schritt:  slbuild(''%s'')  -> C++ + packNGo-ZIP;\n' ...
         'dann die generierten safety_*-Leafs (falls als Referenced/Export-Fcn\n' ...
         'gebaut) in den CTest ziehen, sonst separat via gen_lib_codegen.m.\n'], mdl);
end
