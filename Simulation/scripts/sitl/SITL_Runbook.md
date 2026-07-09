# SITL-Runbook — mcu.slx neu testen nach Modelländerung

Kurzanleitung: was tun, wenn du etwas an `mcu.slx` (oder an einem Signal an der
MCU-Grenze) änderst und die Codegen-Treue erneut zertifizieren willst.

Zwei Gates, immer in dieser Reihenfolge:
- **(A) SIL** — schneller In-MATLAB-Check im geschlossenen Kreis.
- **(B) Host-Golden** — strenger, MATLAB-freier, tickgenauer Nachweis (CTest).

---

## Wann welche Schritte nötig sind

| Änderung an mcu.slx …                                   | Neu generieren | Golden neu | (A) | (B) |
|---------------------------------------------------------|:--:|:--:|:--:|:--:|
| Nur Interna (Logik/Parameter), I/O-Ports **unverändert**|  ✓ |  ✓ |  ✓ |  ✓ |
| Neuer/geänderter Root-**Port** (In/Out)                 |  ✓ |  ✓ | ✓ | ✓ + `mcu_io.hpp` anpassen |
| Nur andere Blöcke (plant/gcu/…), mcu **unberührt**      |  – |  ✓ | ✓ | ✓ |

„Golden neu" heißt: `log_mcu_golden.m` erneut laufen (die aufgezeichneten I/O
ändern sich, sobald sich das Verhalten an der MCU-Grenze ändert).

---

## Schritt-für-Schritt

### 0. Voraussetzung (einmalig pro MATLAB-Sitzung)
`main` öffnen, damit `params.m` via PreLoadFcn läuft (→ `Ts_inner` im Workspace).

### 1. Codegen neu erzeugen  (nur wenn mcu.slx geändert)
```matlab
clear configure_mcu_codegen
configure_mcu_codegen('mcu')     % pinnt Klassenname MCU + SingleTasking
slbuild('mcu')                   % -> scripts\sitl\mcu_ert_rtw\  (C++-Klasse MCU + packNGo)
```
Kontrolle im Report: genau EIN `step()`, Klasse `MCU`, ExtU/ExtY wie erwartet.

### 2. Golden neu aufzeichnen
```matlab
clear log_mcu_golden
run scripts\sitl\matlab\log_mcu_golden.m     % -> scripts\sitl\data\golden_mcu_io.csv
```
Abschlusszeile prüfen: „5001 Ticks … N Spalten", Spaltenliste enthält alle
Boundary-Signale (inkl. `led.1`).
> **Wichtig:** der `led`-Ausgang des MCU-Blocks muss in `main` verdrahtet sein
> (Terminator genügt), sonst bricht das Skript bewusst ab.

### 3. Gate (A) — SIL
```matlab
sil_check_mcu        % Defaults: main + 'main/running on the quadrocopter MCU'
```
Erwartung: `rotor_cmd max|d|` ~1e-14, `led` 0 Mismatches. Klein/beschränkt = ok.

### 4. Gate (B) — Host-Golden (MATLAB-frei)
```powershell
cd scripts\sitl
cmake --build build --config Release          # build/ existiert schon
ctest --test-dir build -C Release -R McuGolden --output-on-failure
```
Erwartung: `RotorCmdMatchesGolden` + `DeterministicAcrossFreshInstances` grün,
`|dq| <= 1e-9`, `led` exakt.

---

## Wenn (B) rot wird — Diagnose
```powershell
cmake --build build --config Release --target diag_mcu_replay
.\build\Release\diag_mcu_replay.exe
```
Liest den einkompilierten Golden-Pfad (Zeile 1 der Ausgabe) und meldet:
- ersten Abweich-Tick, Charakter (Einzelspike / dauerhaft / Bursts),
- `got` vs `exp` um den Tick,
- **welche Eingangsspalten** sich am Abweich-Tick ändern.

Deutung:
- **Eingang ändert sich dort** → echtes I/O-/Verhaltens-Delta; Codegen vs Modell
  ernsthaft prüfen (oder Golden veraltet — Schritt 2 wiederholen).
- **Kein Eingang ändert sich, `exp` „friert" einen Takt** → Golden-Writer/Timing.
  (Bereits behoben: `zoh_resample` ist index-basiert `round(t/Ts)`, nicht `interp1`
  auf Fließkomma-Zeit. Falls es je wiederkommt: hier ansetzen.)

---

## Häufige Stolpersteine (real erlebt)
- **Alte Datei läuft:** `run …\log_mcu_golden.m` führt die Datei AM PFAD aus.
  Neue Version wirklich nach `scripts\sitl\matlab\` kopieren; `clear <fn>` davor.
  Bei C++: nach dem Kopieren neu bauen; im Zweifel `Remove-Item -Recurse -Force build`.
- **Diag/Test liest alte CSV:** Golden-Pfad ist beim CMake-Configure einkompiliert
  (`GOLDEN_MCU_CSV`). Prüfen (Zeile 1 der Diag-Ausgabe). Falls falsch:
  `cmake -S . -B build -DGOLDEN_MCU_CSV="…/scripts/sitl/data/golden_mcu_io.csv"`.
- **MODEL_ROOT nicht gefunden:** `-DMODEL_ROOT="…/mcu_ert_rtw"` beim Configure.
- **`interp1`-Fehler „Sample values must be double":** Integer-Kanäle (`estop`
  uint8, `ack` boolean) — der `double`-Cast in `log_mcu_golden.m` fängt das ab
  (schon drin). „at least two sample points" → konstanter Kanal (1 Sample),
  ebenfalls schon abgefangen.
- **Kein `MATLAB_ROOT` nötig** für das Modell-Target (rtwtypes.h selbstständig).
  Nur die alten Leaf-Codegen-Targets (`SAFETY_IMPL=codegen`) brauchen es.

---

## Port-Kontrakt (Stand jetzt)
- **ExtU_mcu_T:** `Bus_IMU_k` (imu_gyro[3], imu_acc[3]), `Bus_Cmd_l`
  (F_des, q_des[4], q_ref[4], Omega_ref[3], tau_ref[3], q_ext[4], estop(uint8),
  ack(bool)), `batt_count`(double).
- **ExtY_mcu_T:** `rotor_cmd[4]` (double, rad/s bzw. U/s), `led`(uint8 = Batterie-FSM-state).
- Reset = frische `MCU`-Instanz (Zustand wird im Konstruktor genullt; `initialize()` leer).
- Bei Port-Änderung: `mcu_io.hpp` (`wire_inputs`/`diff_*`) und ggf. `OUT_NAMES`
  in `log_mcu_golden.m` nachziehen — das ist die einzige Reconcile-Stelle.
