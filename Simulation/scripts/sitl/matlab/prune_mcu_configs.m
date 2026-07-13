function prune_mcu_configs(mdl)
%prune_mcu_configs  Config-Set-Wildwuchs auf mcu.slx aufraeumen (Modell-Hygiene).
%   Entfernt NUR die nummerierten Duplikate 'ert_cpp_sitl<N>' / 'ert_cpp_arm<N>'
%   (entstanden durch wiederholtes configure_mcu_codegen bei Namenskollision).
%   Behaelt die kanonischen 'ert_cpp_sitl' + 'ert_cpp_arm' und JEDEN anderen
%   Config (z.B. den Modell-Default) unangetastet. Loescht einen Straggler nur,
%   wenn der zugehoerige kanonische Name existiert -> nie den letzten strippen.
%
%   Voraussetzung: mcu darf NICHT interaktiv offen sein (Skript speichert).
%   Aufruf z.B.:  openProject('DROMA.prj'); prune_mcu_configs('mcu')
if nargin < 1, mdl = 'mcu'; end
load_system(mdl);

names   = getConfigSets(mdl);
haveCan = @(base) any(strcmp(names, base));
isStrag = @(n,base) ~isempty(regexp(n, ['^' base '\d+$'], 'once'));

% Aktiven Config auf einen Keeper setzen (Detach eines aktiven Configs ist verboten).
prefer = '';
if haveCan('ert_cpp_sitl'), prefer = 'ert_cpp_sitl';
elseif haveCan('ert_cpp_arm'), prefer = 'ert_cpp_arm'; end
if ~isempty(prefer) && ~strcmp(getActiveConfigSet(mdl).Name, prefer)
    setActiveConfigSet(mdl, prefer);
end

removed = {};
for i = 1:numel(names)
    n = names{i};
    if (isStrag(n,'ert_cpp_sitl') && haveCan('ert_cpp_sitl')) || ...
       (isStrag(n,'ert_cpp_arm')  && haveCan('ert_cpp_arm'))
        detachConfigSet(mdl, n);
        removed{end+1} = n; %#ok<AGROW>
    end
end

fprintf('Entfernt (%d): %s\n', numel(removed), strjoin(removed, ', '));
fprintf('Behalten: %s\n', strjoin(getConfigSets(mdl), ', '));
fprintf('Aktiv:    %s\n', getActiveConfigSet(mdl).Name);
if isempty(removed)
    fprintf('Nichts zu tun (keine nummerierten Stragglers).\n');
else
    save_system(mdl);
    fprintf('mcu.slx gespeichert.\n');
end
end
