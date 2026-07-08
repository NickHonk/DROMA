%% configure_mcu_codegen.m — Embedded-Coder-Config fuer mcu.slx (Onboard-MCU).
%  Setzt die maszgeblichen Codegen-Optionen als benannten ConfigSet und haengt
%  ihn an mcu.slx. GEN-ONLY: erzeugt C++-Quelle + packNGo-ZIP fuer den host-
%  seitigen SITL-Loop; kein Ziel-Compile aus MATLAB.
%
%  Entscheidungen (gelockt, siehe README §Decision-Table + Handover Teil 6):
%   - ert.tlc (Embedded Coder), Sprache C++  -> reusable Model-Klasse 'MCU'.
%     KLASSENNAME wird EXPLIZIT via Code-Mapping gesetzt (setClassName), sonst
%     nimmt Coder den MODELLNAMEN 'mcu' (klein) als Default -> ABI-Mismatch zum
%     Host-Harness (#include "mcu.h"; MCU obj;). [M3]
%   - Solver: DISCRETE, fixed-step, Basisrate Ts_inner = 1/1000 s (params.m).
%     Alle Safety-Raten sind ganzzahlige Vielfache (Ts_batt=100*Ts_gcs etc.).
%   - SolverMode = SINGLE-TASKING -> genau EIN step() (Sub-Raten intern via
%     Zaehler). Bei Auto/Multitasking emittiert Coder step0()/step1()/... und
%     der Host-Loop braucht mehrere Aufrufe. [M2]
%   - Array-Layout COLUMN-major (Default) — konsistent mit gen_lib_codegen.m
%     und dem C++-Golden-Test-Adapter.
%   - SupportNonFinite=false, kein MAT-File-Logging (embedded-clean).
%   - packNGo -> ein ZIP, das der GoogleTest/CTest-Host-Harness zieht.
%
%  Voraussetzung: mcu.slx laedt params.m via PreLoadFcn (Bus-Objekte im .sldd),
%  d.h. Ts_inner liegt im Base-Workspace, wenn das Modell geladen ist.

function configure_mcu_codegen(mdl)
if nargin < 1, mdl = 'mcu'; end
load_system(mdl);

% --- Sicherstellen, dass die Basisrate bekannt ist (PreLoadFcn -> params.m) ---
if ~evalin('base','exist(''Ts_inner'',''var'')')
    warning(['Ts_inner nicht im Base-Workspace. PreLoadFcn/params.m nicht ' ...
             'gelaufen? FixedStep=''Ts_inner'' wird sonst beim Build scheitern.']);
end

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
set_param(cs,'SolverMode','SingleTasking');              % [M2] genau EIN step()

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

% --- [M3] Klassennamen deterministisch auf 'MCU' pinnen -------------------
% Default waere der Modellname ('mcu'). Code-Mapping holen (oder anlegen) und
% setzen. API bestaetigt fuer R2025b: coder.mapping.api.get / setClassName /
% getClassName; leerer getClassName == Modellname als Default.
try
    cm = coder.mapping.api.get(mdl);
catch
    cm = coder.mapping.utils.create(mdl);
end
setClassName(cm, 'MCU');
assert(strcmp(getClassName(cm),'MCU'), ...
       'Klassenname konnte nicht auf MCU gesetzt werden (getClassName=%s).', ...
       getClassName(cm));
fprintf('C++-Klassenname gepinnt: getClassName = "%s".\n', getClassName(cm));

% --- Entry-Point-Kontrakt (Doku; nach slbuild gegen den Report pruefen) ---
% Erwartete generierte C++-Klasse (Single-Tasking, C++ class):
%   class MCU {
%     public: void initialize();
%             void step();          % 1 kHz Basisrate; ruft Safety-Leafs @ Sub-Raten
%             void terminate();
%             % I/O ueber ExternalInputs (ExtU_MCU_T) / ExternalOutputs (ExtY_MCU_T)
%   };
% Der SITL-Host-Loop instanziiert MCU, verdrahtet die ExtU-Felder aus dem
% Golden-Log (Bus_Cmd/Bus_IMU/...) und taktet step() mit Ts_inner; ExtY wird
% gegen das Golden-Log gedifft.
fprintf(['\nNaechster Schritt:\n' ...
         '  1) slbuild(''%s'')            %% -> C++-Klasse MCU + packNGo-ZIP\n' ...
         '  2) log_mcu_golden.m           %% Golden-I/O an der MCU-Grenze aufzeichnen\n' ...
         '  3) mcu.h/mcu_types.h + Report %% an den Host-Harness (test_mcu_model) geben\n'], ...
         mdl);
end