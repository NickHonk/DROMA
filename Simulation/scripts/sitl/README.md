# SITL — C++-Golden-Test (Quaternion-Helfer + Safety-Leafs)

Host-seitige Codegen-Treue-Stufe: die eingefrorenen Golden-Vektoren aus
`verify_quat_codegen.py` sowie die `verify_overspeed`/`verify_battery`-Invarianten
nach GoogleTest/CTest gezogen. Läuft **ohne MATLAB** grün gegen mitgelieferte
Referenz-Ports und zertifiziert nach einem reinen Link-Tausch den generierten Code.

```
cmake -S . -B build && cmake --build build -j && ctest --test-dir build --output-on-failure
```
Referenz-Lauf hier: **21/21 grün**, `dcm2quat` worst |dq|=1.8e-16 (Zweige 14/5/5/7),
`quat2dcm` 3.9e-16, `quatMul`/`quatConj` exakt, Round-Trips ~1e-15.

---

## Decision-Table (gelockt)

| # | Entscheidung | Wert | Grund |
|---|---|---|---|
| D1 | ABI der Leaf-Helfer | `extern "C"`, fixed-size `double`, out-Param | bit-identisch zu MATLAB Coder → Link-Tausch statt Testumbau |
| D2 | Matrix-Speicherordnung | **column-major** in der ABI; Golden ist **row-major** | Coder-Default ist column-major; Umrechnung isoliert in `test/csv.hpp` |
| D3 | Leaf-Codegen-Sprache | **C** (`gen_lib_codegen.m`) | trivialer Link; Golden-Diff zertifiziert reine Numerik. Modell `mcu.slx` bleibt **C++** |
| D4 | Golden-Toleranz | `GOLDEN_TOL=1e-9` (Default), Quat bis auf Vorzeichen | = MATLAB-vs-Golden aus `verify_quat_codegen.m`; **bewusst eng** — lose Toleranz kaschiert ULP-Divergenz |
| D5 | Safety-Persistenz | `reset()`+`step()` | 1:1 auf Coders `*_initialize()`+`step()` abbildbar |
| D6 | Safety-Params | `coder.Constant` (default) | kleinere ABI; laufzeitvariabel → als echtes Arg (siehe `gen_lib_codegen.m`) |
| D7 | `mcu.slx`-Interface | ERT, C++-Klasse `MCU`, discrete `Ts_inner=1/1000`, packNGo | host-SITL-Loop instanziiert die Klasse gegen die simulierte Strecke |

Frame **z-up** und `params.m`-IST sind durchgängig respektiert; keine NED-Vorzeichen im Testpfad.

---

## Geflaggte Inkonsistenzen (Toolchain, bitte prüfen)

1. **Golden-Dateiname DCM bricht die Kette.** `verify_quat_codegen.py` schreibt
   `golden_quat.csv`, aber `verify_quat_codegen.m` liest `test_data_quat2DCM.csv`.
   Die drei anderen (`test_data_quatmul/conj/rotate.csv`) matchen. → Der DCM-Golden
   muss zwischen `.py` und `.m` **manuell umbenannt** werden; jeder Re-Run ohne
   Rename testet gegen veraltete Daten. **Fix:** im `.py` `golden_quat.csv`
   → `test_data_quat2DCM.csv` (dieser Test nutzt letzteres als kanonisch).
2. **Handover §7-Manifest stale.** Dort heißen alle vier `golden_*.csv`; der
   Generator schreibt aber `golden_quat.csv` **und** `test_data_quat*.csv`. Namen
   im Manifest an den Generator angleichen.
3. **Zwei Toleranzwelten.** `verify_quat_codegen.m` nutzt `tol_c=1e-6` für MEX.
   Für den **C++**-Golden-Diff ist das zu lose (reiner Double-Codegen ohne
   `-ffast-math` trifft ~1e-15). Hier Default `1e-9`; nur bei belegter ULP-Not
   lockern, sonst versteckt man genau die Codegen-Bugs, die der Test fangen soll.
4. **`quat2dcm_local` ist Rekonstruktion (Handover §5, OFFEN).** Der Referenz-Port
   nutzt dieselbe Aerospace-Formel → `Quat2Dcm_MatchesGolden` ist scharf, aber
   erst gegen das **echte** `quat2dcm_local` aus `mahony_filter`/`geo_attitude_ctrl`
   aussagekräftig. Bis dahin zertifiziert der Test nur, dass Codegen == diese Formel.

---

## Auf Codegen umstellen

1. In MATLAB (Helfer + Safety-Leafs auf dem Pfad):
   ```matlab
   run matlab/gen_lib_codegen.m        % -> codegen/lib/<fn>/*.c(+.h)
   ```
2. Neu konfigurieren + testen:
   ```
   cmake -S . -B build -DQUAT_IMPL=codegen -DSAFETY_IMPL=codegen \
         -DCODEGEN_ROOT=/pfad/zu/codegen
   cmake --build build -j && ctest --test-dir build --output-on-failure
   ```

### Codegen-Shim (nur Safety, wegen `*_initialize`+Struct)
Coder erzeugt `safety_overspeed(...)` mit von ihm benannter Param-Struct
(`struct0_T`). Der Testkörper ruft aber die stabile `overspeed_reset()/step()`-ABI.
Ein ~15-Zeilen-Shim schließt die Lücke:
```cpp
#include "safety_overspeed.h"            // generiert
#include "safety_helpers.h"              // Test-ABI
void overspeed_reset() { safety_overspeed_initialize(); }
void overspeed_step(const double g[3], uint8_t estop, bool ack,
                    const OverspeedParams* p, bool* kill, uint8_t* src, double dbg[3]) {
    // p ignorieren, wenn coder.Constant genutzt wurde; sonst in struct0_T kopieren.
    boolean_T k; uint8_T s; double d[3];
    safety_overspeed(g, estop, ack, &k, &s, d);   // Signatur lt. gen. Header
    *kill=k; *src=s; dbg[0]=d[0]; dbg[1]=d[1]; dbg[2]=d[2];
}
```
Die exakte generierte Signatur steht im erzeugten `.h`; Reihenfolge/Pointer ggf.
anpassen. `battery` analog.

---

## Dateien

```
include/quat_helpers.h        ABI (extern "C", column-major R)
include/safety_helpers.h      ABI reset()+step()
src/quat_helpers_ref.cpp      bit-treuer Port (Standin/CI-Referenz)
src/safety_helpers_ref.cpp    Port safety_overspeed + safety_battery
test/csv.hpp                  CSV-Reader + row->col-major-Adapter
test/test_quat_golden.cpp     5 Helfer vs Golden + Property-Round-Trips
test/test_safety.cpp          Overspeed S1–S9, Battery B1–B6
matlab/gen_lib_codegen.m      Leaf-Codegen (C-Lib) für den Golden-Diff
matlab/configure_mcu_codegen.m ERT-C++-Config für mcu.slx (+ Entry-Kontrakt)
data/test_data_quat*.csv      Golden (aus verify_quat_codegen.py)
```

## Offen (nächste Schritte)
- **[du]** `quat2dcm_local` standalone aus dem mcu-Code extrahieren → Inkonsistenz #4 schließen.
- **[du]** `mcu.slx`: `configure_mcu_codegen.m` laufen, `slbuild`, packNGo ins Host-Repo;
  dann echten Host-SITL-Loop (MCU-Klasse gegen simulierte Strecke) — separat vom Leaf-Diff.
- **[opt]** `gcs_supervisor` ist GCS-seitig, nicht in `mcu.slx`; falls onboard gespiegelt,
  analoges Sequenz-Fixture wie test_safety ergänzen.
```
