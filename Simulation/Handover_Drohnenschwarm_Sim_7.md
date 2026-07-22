# Handover Drohnenschwarm-Sim — Teil 7

*Fortsetzung von Teil 6. §-Referenzen ohne Zusatz beziehen sich auf Teil 6/5.
Diese Session: **Step 4 — Hardware-Deployment Teensy vorbereitet.** OTA-Paket
entworfen + verifiziert, smallest-three-Codec (MATLAB + C++) gebaut und
bitgleich geprüft, `link_tx/rx` + `init_link` refactored, HAL-Skelett
(Teensy 4.1) geschrieben. Alle Codegen-/HAL-Entscheidungen gelockt.
Nächster Block: Codec-Cross-Check, SITL-Re-Zert für `throttle`, ARM-Codegen.*

---

## 0aa. TL;DR Session 10 (zuletzt) — Taster-Kill umgewidmet + Tilt-Cutoff

1. **`btn_ack` umgewidmet: Quittung → lokaler Kill.** Re-Armen läuft **nur** noch
   über die steigende Flanke von `Bus_Cmd.ack`. Eine steigende Taster-Flanke
   **latcht throttle→0** (`fault_src=4`); solange der Taster **gehalten** wird, ist
   Re-Armen gesperrt. ⇒ Akku sicher absteckbar, ohne dass die Props anlaufen. Das
   frühere OR (`btn_ack || Bus_Cmd.ack`) in `mcu.slx` ist **entfernt**.
2. **Tilt-Cutoff neu.** Kippwinkel **> 80° für 80 Zyklen (80 ms @1 kHz)** → gleicher
   Kill-Latch (`fault_src=3`), aus der geschätzten Lage `q_hat`:
   `cos(tilt) = w²−x²−y²+z²` (normiert, transponierungssicher). Schwellen als
   Parameter in `init_safety.m` (`tilt_max_deg=80`, `tilt_cos_min=cosd(80)`,
   `tilt_debounce_N=80`). `q_hat` (Mahony-Ausgang) ist dafür **neu** an
   `safety_overspeed` verdrahtet.
3. **`safety_overspeed` = 4 Kill-Quellen in einem Latch:** `1` Overspeed ·
   `2` Hard-Kill · `3` Tilt · `4` Taster. Re-Arm nur bei `Bus_Cmd.ack`-Flanke
   **und** ohne aktive Fehlerbedingung (kein Overspeed, kein Tilt, `estop≠2`,
   Taster los). Neue Signatur:
   `safety_overspeed(gyro_corr, q_hat, estop, ack, btn, safety)`.
4. **`mcu.slx`-Umbau** (headless, **ohne** `openProject` — Falle aus §0a.8): OR-Block
   raus, Wrapper `MATLAB Function` auf 6 Args, `q_hat`/`btn`/`Bus_Cmd.ack` neu
   verdrahtet. Danach Host- + ARM-Codegen, `throttle_poly`, Golden und
   `gen_lib_codegen` (Leaf-Signatur) neu.
5. **✅ Gate B grün: `ref` 40/40, `codegen` 39/39** (S9 wie immer ausgeblendet).
   Neue Regressionen: `test_safety` **T1–T4** (Tilt) + **BT1–BT3** (Taster);
   modellweit **`McuOverspeed`** (Bus-ack-Re-Arm), **`McuButton`**, **`McuTilt`**
   (letzterer beweist die `q_hat`-Verdrahtung: Estimator via `q_ext` in 85° gezogen,
   Kill greift). `verify_overspeed.m`-Scaffold grün.
6. ⚠️ **Flug-Erinnerung (Session 11 aktualisiert):** Betriebsart beim Flashen
   explizit wählen — `./build_sketches.sh --upload-drone-flight`. Kein
   Auskommentieren mehr nötig (`HAL_SELFTEST` gibt es nicht mehr, siehe
   §Betriebsarten). Der neue ARM-Code liegt in `hardware/mcu_arm/`.
   HAL-Code selbst unverändert (`btn_ack` + `Bus_Cmd.ack` gingen schon getrennt in
   die MCU); nur der Kommentar bei Pkt. „3b) btn_ack" ist nachgezogen.

---

## 0a. TL;DR Session 9 — Re-Arm/F_des-Sweep → Gyro-Bias-Bug

1. **⛔ Sim≠HW-Bug gefunden + gefixt: doppelte Gyro-Bias-Subtraktion.** Die HAL
   zieht den echten Bias ab, `mcu.slx` zog zusätzlich den *fiktiven* Sim-Bias ab
   → auf HW **10 °/s Schein-Drehrate je Achse**. **Erklärt `thr[5 19 5 17]` neu —
   es war NICHT die 6°-Schieflage.** Fix (strukturell): Bias-Logik **komplett aus
   `mcu.slx` entfernt** (`Constant1`, `Subtract`, Mahony-`b_ground`); `sensors.slx`
   prägt den Bias auf (**sättigt mit**) und der neue Sum `HAL gyro bias` zieht ihn
   dahinter ab. ⚠️ Dabei aufgefallen: der Bias frisst **77 % der Overspeed-Marge**
   (FSR 8.7266 vs 8.5) → offener Punkt 3. Details **§3h**.
2. **❌ Arming-Idle-Interlock verworfen** (Messdaten in §3h + Schlusskommentar
   `safety_overspeed.m`): Re-Arm ist ohnehin nicht motorfrei (8.404 % throttle),
   und der lokale Taster hing an der GCS. Code + `mcu.slx` zurückgebaut.
3. **Golden + Codegen (Host & ARM) neu**, beide frei von `0.94666`/`0.1745`.
4. **✅ Gate B 30/30 GRÜN** (MSVC 2022 BuildTools nachinstalliert). 31→30 = der
   entfernte `Overspeed.S10`. Golden + Codegen sind damit **zertifiziert**.
   `gen_lib_codegen.m` neu gelaufen (Leaf-Signatur), `prune_mcu_configs` (9 Dups
   raus, `mcu.slx` 756→312 KB).
5. **✅ HW-Tests T1–T6 alle GRÜN (§3h)** — der Bias-Fix ist auf Hardware bewiesen:
   `thr[11 12 12 11]` **symmetrisch** statt `[5 19 5 17]`, und der Wert deckt sich
   mit dem Sim-Sweep ⇒ **Sim == HW**. Failsafe/Re-Arm/Batt/Timing ebenfalls grün.
   Realer Gyro-Bias ist **6× kleiner** als die Sim-Annahme ⇒ Overspeed-Marge real
   entspannt (13 % statt 77 %).
   ⚠️ Default-Betriebsart ist **`HAL_MODE_BENCH`** ⇒ Motoren drehen nie.
   **Für Schubtest bzw. Flug explizit flashen:** `--upload-drone-thrust` bzw.
   `--upload-drone-flight` (siehe §Betriebsarten).
6. **✅ `models\bench.slx`** (§3h): Prüfstand-Harness = `quadcop` ohne
   Drohnen-Simulation, `FixedStep=Ts_gcs` (100 Hz statt 1 kHz), Accelerator.
   **Auf HW verifiziert: `missed ticks = 0`** (vorher 3700/6.76 s), **`link=8 ms`**
   (12,5× unter dem Watchdog). Ab jetzt: `quadcop.slx` = Simulation,
   `bench.slx` = Versuchsstand.
6. **Gate A abgeschafft** (§3h): `run_gate_a.m` gelöscht, `sil_check_mcu.m` nur
   noch Diagnose. **Gate B (30/30) ist die alleinige Zertifizierung.**
7. **✅ SSOT ausgerollt** (§3h): **alle 11** Function-Blöcke sind jetzt Wrapper auf
   `scripts\functions\*.m`; `traj_gen`/`pos_ctrl` neu extrahiert. Generierter Code
   byte-identisch, Golden unverändert, Gate B 30/30. **Blöcke nie wieder inline
   editieren.**
8. **⚠️ Falle:** `openProject` (in `run_mcu_recert`/`run_mcu_arm_codegen`) hat
   `mcu.slx` mit einer alten Version überschrieben → headless ohne `openProject`
   arbeiten (§3h).

---

## 0. TL;DR — was diese Session geändert hat

1. **`mcu.slx`-Änderung (Nutzer):** neuer Root-Outport **`throttle[4]`** =
   `polyval(quadcop.p_from_omega_sq, ω²)` (quadratisch in ω², aus F/τ des
   Reglers), gesättigt auf **`[0,100]`**. Auf der MCU nur noch → OneShot125.
   **Regen steht aus** (hochgeladenes `mcu.h` zeigt `ExtY` noch ohne `throttle`).
2. **OTA-Paket entworfen (29 B, Design gelockt):**
   `[0]id | [1]flags(bits1:0=estop 0/1/2, bit2=ack) | [2]seq | [3..4]F_des int16 |
   [5..8]q_des | [9..12]q_ref | [13..16]q_ext (je smallest-three uint32) |
   [17..22]Omega_ref 3×int16 | [23..28]tau_ref 3×int16`. nRF-HW-CRC deckt Integrität.
3. **smallest-three-Codec (MATLAB):** `pack_quat_sm3.m` / `unpack_quat_sm3.m`.
   Round-Trip **max 0.245° / RMS 0.088°**, Komponenten-LSB 1.38e-3, Identitäts-
   Quat-Code = 537395712 = 0x20080200. int16-Böden: F 1.2e-3 N, Ω 3.1e-4 rad/s,
   τ 6.1e-5 N·m.
   **KORREKTUR (Session 8):** Der C++-Codec `mcu_packet.hpp` und
   `quat_ota_roundtrip.m` waren in Session 7 **nicht** committet — „C++ ↔ MATLAB
   bitgleich" war aspirativ, nicht reproduzierbar. In Session 8 real gebaut +
   cross-gecheckt, siehe §3a (erledigt).
4. **`link_tx.m`/`link_rx.m`/`init_link.m` refactored:** int16 nur noch für die
   7 Skalar-/Vektorwerte `[F_des; Omega_ref(3); tau_ref(3)]`, Quaternionen über
   smallest-three (uint32). `link_params.fs` 19→7. Der Kanal trägt jetzt **drei**
   Signale: `pkt_i16` (int16 7×1), `pkt_q` (uint32 3×1), `flags` (2×1) — alle
   drei durch RT+Delay mit **identischem** `N_delay`; ICs aus `init_link`
   (`pkt_init_delay`, `q_init_delay`, `flags_init_delay`).
5. **HAL-Skelett `drone_hal.cpp`** (Teensy 4.1) geschrieben: 1-kHz-Tick,
   MPU→Bus_IMU, ADC→batt_count, nRF-Unpack→Bus_Cmd, throttle→OneShot125,
   Watchdog→estop=2.

---

## 1. Gelockte Entscheidungen (maßgeblich)

- **Frame** z-up (nicht NED). `params.m` (IST) maßgeblich. Basisrate `Ts_inner=1e-3` (1 kHz).
- **Ziel-HW** Teensy 4.1 (i.MX RT1062, Cortex-M7). **`double` behalten** (HW-DP-FPU
  vorhanden, ~halbe Float-Rate; bei diesem Modellumfang @1 kHz unkritisch;
  Golden-Toleranz 1e-9 trägt 1:1).
- **Schwarm = 3 Drohnen.** GCS (Simulink) ist Dirigent, erzeugt 3 `Bus_Cmd`-Ströme
  mit je Ziel-`id`.
- **Sendekette (Design A):** Simulink `Serial Send` (USB, vollpräziser Float-Frame
  `[sync|id|Bus_Cmd|checksum]`) → **Sende-Teensy** ruft `pkt::pack(Bus_Cmd, id, seq)`
  → `radio.write(buf,29)`. Sende-Teensy = ID-Durchreicher, hält **seq pro Drohne**.
- **nRF-Adressierung (Design A):** **Broadcast**, Auto-Ack **AUS**, gemeinsame
  Adresse; jede Drohne nimmt nur `buf[0]==own_BCD` (App-ID-Gate). Passt zum
  Sim-Verlustmodell (`pdrop=0.02`, kein Retransmit, ZOH-Hold).
- **nRF-Pins:** SPI1 (SCK27/MOSI26/MISO1), CE14, CSN0, IRQ9. `RF24 radio(14,0)`,
  `begin(&SPI1)`.
- **Quaternionen scalar-first** `[w x y z]` — Codec indiziert überall gleich.
- **OTA-Serialisierung (Session 8 gelockt):** Multibyte-Felder (F int16, 3× sm3
  uint32, Ω/τ int16) **little-endian** (beide Enden ARM-LE, internes Protokoll).
  flags-Byte `bits[1:0]=estop (0/1/2)`, `bit[2]=ack`. MATLAB `round` =
  half-away-from-zero → im C++-Codec **`std::lround`** (nicht `nearbyint`),
  sonst Bit-Divergenz an Rundungsgrenzen.
- **IMU MPU-6050:** Wire(0) SDA18/SCL19, Adresse **0x68** (ADO→GND, HW-Bodge nötig),
  **`Wire.setClock(400000)`** (1-kHz-Budget). Gyro **FS_SEL=1** (±500 dps,
  65.5 LSB/dps), Acc **AFS_SEL=1** (±4 g, 8192 LSB/g). Achsdrehung Body←Sensor
  **`R_bs`: `[x_b;y_b;z_b]=[y_s;-x_s;z_s]`** (= Rz(−90°)) — **in der HAL**
  (`sensors.slx` gibt Body-Frame aus). Reihenfolge HAL: raw→SI→`R_bs`→Gyro-Bias→Bus_IMU.
- **Gyro-Bias — Kompensation gehört AUSSCHLIESSLICH in die HAL (Session 9 gelockt,
  strukturell erzwungen).** Die HAL mittelt 3 s im Startup und zieht den **echten**
  Bias ab (`drone_hal.cpp` Z.281); an der MCU-Grenze ist `imu_gyro` **bias-frei**.
  **`mcu.slx` enthält KEINE Bias-Logik mehr** — `Constant1`, `Subtract` und der
  Mahony-Eingang `b_ground` sind **gelöscht** (`mahony_filter` hat jetzt 7 Args).
  Damit *kann* die Doppel-Subtraktion aus §3h nicht zurückkehren.
  Die Sim bildet die HAL-Stufe nach: `sensors.slx` prägt `imu.gyro_bias` auf
  (→ **sättigt mit**, wie auf HW) und der Sum-Block **`sensors/HAL gyro bias`**
  zieht `imu.gyro_bias_hat` hinter dem Gyro-Block wieder ab
  (`gyro_bias_hat == gyro_bias` ⇔ perfekte Kalibrierung; abweichen lassen für
  Restfehler-Tests). Mahony-Kᵢ ist **deaktiviert** → die HAL-Mittelung ist die
  einzige Drift-Absicherung.
  ⚠️ **Bias vor der Saturation ⇒ er frisst Overspeed-Marge:** FSR 8.7266 vs
  `omega_max` 8.5 = **0.2266** Marge, davon `|bias|` = 0.1745 ≈ **77 %**. Real
  messbare Obergrenze ≈ 8.552 rad/s ⇒ effektive Marge **~0.05 rad/s**. Deshalb
  darf `gyro_bias` NICHT einfach 0 gesetzt werden — die Sim wäre sonst
  optimistischer als die HW. Bewertung offen (§3h).
- **Hebelarm** `r=[-0.014;-0.015;0.045]` m: **Option 1 gewählt — keine Kompensation**
  (weder Sim noch HAL noch `mcu.slx`). `sensors.slx` modelliert ihn; HW reproduziert
  ihn physikalisch → Sim=HW automatisch. Acc **hebelarm-roh** durch die HAL.
  (Falls je aggressiver geflogen wird: Option 2 = Zentripetal-Kompensation
  `f−ω×(ω×r)` aus `imu_gyro` **innerhalb `mcu.slx`**, dann Re-Cert.)
- **Batterie:** **12 bit**, **rohe counts** → Modell rechnet `Vf = k·batt_count`,
  `k=0.0166737` (HW-kal.). Strom = nur Telemetrie, **nicht** ins Modell. Umrouting
  34/35→40/41 war nötig (34/35 haben keinen ADC).
  **Pin-Belegung (Nutzer bestätigt, Session 9): Pin 41 = SPANNUNG (A17),
  Pin 40 = Strom (A16, nur Telemetrie).** `drone_hal.cpp` steht korrekt auf
  `PIN_BATT_V=41`. Die §3e-Notiz unten (Session 8, „Pin 40 = Spannung") ist damit
  **überholt**. ⚠️ **Offen:** `k=0.0166737` stammt aus einer Messung an **Pin 40**
  (944 counts ↔ 15.74 V). Weicht der Teiler an Pin 41 ab, sind Warn/Crit/Floor
  (14.0/13.4/12.0 V) verschoben → **`k` beim nächsten HW-Test verifizieren**
  (counts↔Volt an Pin 41), `init_battery_manag.m` ggf. korrigieren.
- **ESC = OneShot125.** `analogWriteFrequency(pin,1000)` + `analogWriteResolution(12)`
  → count 512..1024 = 125..250 µs. Mapping **`count = 512 + throttle*5.12`**
  (`throttle` bereits [0,100] geclampt). Pins/Richtung: **M1=33 CCW, M2=2 CW,
  M3=4 CCW, M4=3 CW** (gegen Mixer-Nummerierung geprüft). Gegenlauf via HW
  (Phasen/Props), **nicht** übers Signal.
- **ESC-Einlernen + Arming:** Startup-FSM am Boden (max→min→arm), Endpunkte
  = Flug-Endpunkte 512/1024.
- **Failsafe:** kein gültiges Paket seit **100 ms** → `estop=2` (Hard-Kill,
  `safety_overspeed` latcht rotors=0; Re-Arm nur über die `Bus_Cmd.ack`-Flanke).
  Soft-Land (estop=1) ist GCS-getrieben → bei Link-Verlust unmöglich, daher
  estop=2 die einzige kohärente Onboard-Aktion.
- **Onboard-Kill-Latch `safety_overspeed` (Session 10 gelockt) — 4 Quellen, 1 Latch,
  1 Re-Arm:**
  - `1` **Overspeed:** `safety.omega_max=8.5 rad/s` < Gyro-FSR 8.727 rad/s →
    detektierbar (Sättigung 8.727 > 8.5), über `debounce_N=4` entprellt. Margin
    dünn, aber Entprellung robust.
  - `2` **Hard-Kill:** `estop==2` (Uplink oder Link-Watchdog), sofort.
  - `3` **Tilt:** Kippwinkel > `tilt_max_deg=80°` über `tilt_debounce_N=80` Zyklen,
    aus `q_hat` (`cos(tilt)=w²−x²−y²+z²`). Fängt langsames Umkippen/Liegen, das die
    Drehraten-Schwelle nie überschreitet.
  - `4` **Taster:** steigende Flanke von `btn_ack` (lokaler Teensy-Taster, Pin 21).
    Lokaler „Motoren-jetzt-aus"-Knopf fürs sichere Akku-Abstecken.
  - **Re-Arm** (Fault→Armed) **nur** bei steigender `Bus_Cmd.ack`-Flanke **und**
    ohne aktive Fehlerbedingung: `~over_inst & ~tilt_inst & estop≠2 & ~btn`. Der
    Taster ist damit **keine Quittung mehr** (früher OR mit `Bus_Cmd.ack`), sondern
    Auslöser + Re-Arm-Sperre solange gehalten. `fault_src` (LED/Debug):
    0 keine / 1 overspeed / 2 hard-kill / 3 tilt / 4 taster.

---

## 2. Dateien dieser Session (im Projekt ablegen)

| Datei | Rolle | Ablage (Vorschlag) |
|---|---|---|
| `pack_quat_sm3.m` | sm3-Encoder (codegen) | `scripts\` (MATLAB-Pfad) |
| `unpack_quat_sm3.m` | sm3-Decoder (codegen) | `scripts\` |
| `quat_ota_roundtrip.m` | Round-Trip-Wrapper für link-Block | `scripts\` |
| `link_tx.m` (neu) | TX: int16(7)+sm3(3)+Verlust | `scripts\` (ersetzt alt) |
| `link_rx.m` (neu) | RX: dequant+sm3-decode | `scripts\` (ersetzt alt) |
| `init_link.m` (neu) | fs 19→7, q_init, Delay-ICs | `scripts\` (ersetzt alt) |
| `mcu_packet.hpp` | C++-Codec `pkt::pack/unpack` (SSOT) | `scripts\sitl\include\` + Firmware |
| `drone_hal.cpp` | Teensy-HAL-Skelett | Firmware-Repo (Teensyduino/PlatformIO) |

**Session 8 (Codec-Cross-Check, §3a erledigt):**

| Datei | Rolle | Ablage |
|---|---|---|
| `mcu_packet.hpp` | C++-Codec (jetzt real gebaut, LE, `std::lround`) | `scripts\sitl\include\` |
| `dump_link_codec_golden.m` | Golden-Dumper (`link_tx/rx`, pdrop=0, 219 Fälle) | `scripts\sitl\matlab\` |
| `link_codec_golden.csv` | Golden 219×54 | `scripts\sitl\data\` |
| `test_link_codec.cpp` | CTest `test_link_codec` (L1/L2 + Header) | `scripts\sitl\test\` |
| `configure_mcu_codegen.m` | **parametrisiert** (`target` host/arm) | `scripts\sitl\matlab\` |
| `run_mcu_recert.m` | §3b: Host-Regen + Poly-Dump + Golden | `scripts\sitl\matlab\` |
| ~~`run_gate_a.m`~~ | ~~Gate A headless-Wrapper~~ — **in Session 9 gelöscht** (§3h) | — |
| `run_mcu_arm_codegen.m` | §3f: ARM-Codegen → `hardware\mcu_arm\` | `scripts\sitl\matlab\` |
| `throttle_poly.hpp` | generiert (`P` aus `quadcop.p_from_omega_sq`) | `scripts\sitl\include\` |
| `mcu_io.hpp` | +`diff_throttle` | `scripts\sitl\include\` |
| `hardware\mcu_arm\mcu_ert_rtw\` | ARM-generierte MCU-Klasse (Cortex-M7) | `hardware\mcu_arm\` |
| `gcs_frame.hpp` | USB-Frame-Contract GS↔Sender (SSOT) | `scripts\sitl\include\` |
| `pack_gcs_frame.m` | MATLAB-Spiegel + Simulink-Serial-Send-Spec | `scripts\functions\` |
| `dump_gcs_frame_golden.m` + `.csv` | Golden 66×104 | `scripts\sitl\matlab\` / `data\` |
| `test_gcs_frame.cpp` | CTest `test_gcs_frame` (parse + CRC/Sync) | `scripts\sitl\test\` |
| `gcs_sender.cpp` | **Sende-Teensy-Firmware** (Design A) | `hardware\` |
| `build_sketches.sh` | assembliert flashbare Sketches → `hardware\build\` (+`--compile`/`--upload`) | `hardware\` |
| `prune_mcu_configs.m` | Modell-Hygiene: nummerierte Config-Set-Dups entfernen | `scripts\sitl\matlab\` |
| `i2c_scan.cpp` | Bench: MPU-Adresse 0x68/0x69 prüfen (ADO-Bodge) | `hardware\` |
| `esc_calibrate.cpp` | Bench: ESC-Einlernen + Motor-Test (Serial-geführt) | `hardware\` |
| `drone_hal.cpp` `HAL_MODE_*` | Betriebsart über `hal_mode.h`: BENCH (Motoren min + Report) / THRUST (Motoren + Report) / FLIGHT (Motoren, kein Report) | `hardware\` |

---

## 3. Offene Punkte / nächste Schritte

### 3a. Codec-Cross-Check — ✅ ERLEDIGT (Session 8)
Host-Test `test_link_codec` (CTest, codegen-frei): identischer `Bus_Cmd` durch
MATLAB `link_tx→link_rx` (chart_40/50) **und** C++ `pkt::pack→unpack`. Golden aus
`dump_link_codec_golden.m` (219 Fälle: sm3-imax-Branches, Sign-Flip, Komponenten
nahe ±1/√2, int16-Sättigung, estop∈{0,1,2}, ack, + 200 Zufall; **pdrop=0** →
Codec isoliert).
- **L1 (Wire):** int16[7], uint32[3] sm3, flags-Byte **bit-exakt**.
- **L2 (decode):** F/Ω/τ **und** Quaternionen **bit-exakt** (Diff = 0.0 auf x86;
  Tol-Schranke 1e-12 nur Reserve für ARM-libm). id/seq-Round-Trip grün.
- **25/25** Tests der Gesamtsuite grün. Schließt „Sim == HW" für den OTA-Codec.
- Referenz-Klärung: die refactorten TX/RX-Blöcke leben in `link.slx` als
  **chart_40** (`link_tx`, 3 Signale) / **chart_50** (`link_rx`); die alten
  **chart_22/31** (19×int16) sind tot, aber noch im Modell → bei Gelegenheit raus.
  `scripts\functions\link_tx.m`/`link_rx.m` wurden auf chart_40/50 re-synct.

### 3b. SITL-Re-Zert für `throttle` — ✅ ERLEDIGT (Session 8)
`mcu.slx` neu generiert (`throttle[4]` in `ExtY`, `Abs→Sqrt` für `rotor_cmd`,
`Polyval→Saturation[0,100]` für `throttle`). Golden neu (5001 Ticks, 37 Spalten,
`throttle.1..4`). Diffs angewandt: `OUT_NAMES += throttle` (`log_mcu_golden.m`),
`diff_throttle` (`mcu_io.hpp`), `test_mcu_model.cpp` (throttle-Golden-Diff ≤1e-9
+ Determinismus auf 9 Kanäle). Automations-Helfer: `run_mcu_recert.m` (Regen+
Poly-Dump+Golden), ~~`run_gate_a.m`~~ (Session 9 gelöscht — Gate A abgeschafft, §3h).

**Gate-Status:** *(Stand Session 9: **Gate B 30/30 = alleinige Zertifizierung;
Gate A abgeschafft** — siehe §3h. Die Zahlen unten sind der historische
Session-8-Stand.)*
- **Gate B (Host-Golden, MATLAB-frei, tick-exakt): 25/25 GRÜN.** Das ist die
  maßgebliche Zertifizierung (throttle-Golden-Diff ≤1e-9, Invariante ≤1e-9,
  Determinismus 9 Kanäle, + Codec-Tests).
- **Gate A (SIL): GRÜN (interaktiv gefahren).** rotor_cmd max|d|=1.137e-12,
  throttle max|d|=2.842e-14, led 0 Mismatches → „bit-nah". `grab`-uint8-Cast
  gefixt, throttle mitverglichen.
  *Hinweis:* headless (`-batch`) scheitert SIL an der MinGW-Toolchain
  (`rtwshared.bat` beim `_sharedutils`-Build „nicht gefunden") — aus Bash **und**
  PowerShell identisch, also `-batch`-SIL-Setup, nicht MSYS-PATH. **Gate A daher
  interaktiv in der MATLAB-IDE fahren** (Gate B läuft headless/CTest).

**Polyval-Invariante — Befund (wichtig):** bit-exakt am MCU-Rand **unmöglich**.
Der Polyval-Eingang im Modell ist das **vorzeichenbehaftete `omega_sq` VOR `abs`**;
am Rand existiert nur `rotor_cmd = sqrt(abs(omega_sq))`. `sqrt∘square ≠ id`
(~1 ULP) + Vorzeichenverlust → nur `clamp(polyval(P, rotor_cmd²))` rekonstruierbar,
und das nur bis **7.1e-15** (nicht 0.0). Entscheidung Session 8: **Toleranz-
Invariante ≤1e-9** (in `test_mcu_model.cpp`), `P` aus `throttle_poly.hpp` (dump
aus `quadcop.p_from_omega_sq`). Coverage-Lücke: dieser Golden triggert **weder
Sättigung** (throttle 18.96–28.92) **noch negatives `omega_sq`** — Sat-/Sign-Pfad
ungetestet; für deren Abdeckung bräuchte es einen Golden mit aggressiveren
Kommandos.

**Nebenfund/behoben:** `sil_check_mcu>grab` castete `led` (uint8) nicht auf
`double` → `interp1`-Crash, sobald `led`/`throttle` verdrahtet sind. Cast ergänzt.

Historische Notiz (ursprüngliche Diff-Liste):
1. `mcu.slx`: `throttle`-Outport + `[0,100]`-Clamp verifizieren, **neu generieren**
   (aktuelles `mcu.h` hat noch kein `throttle`). Clamp-Reihenfolge: Polynom zuerst,
   dann `[0,100]`.
2. Diffs anwenden:
   - `log_mcu_golden.m`: `OUT_NAMES = {'rotor_cmd','led','throttle'};` + `throttle`
     in `main` verdrahten (Terminator).
   - `mcu_io.hpp`: `diff_throttle(...)` (tol 1e-9) analog `diff_rotor`.
   - `test_mcu_model.cpp`: `throttle` mit asserten; optional Invariante
     `throttle == polyval([k2 k1 k0], rotor_cmd²)` mit `[k2 k1 k0]=quadcop.p_from_omega_sq`.
3. Golden neu, **Gate A + Gate B** grün (Runbook).

### 3c. `link.slx` neu verdrahten
Drei Signale statt zwei durch RT+Delay (siehe §0.4). ICs aus `init_link`.

### 3d. GS-Seite — ✅ Sende-Teensy + Frame-Contract erledigt, Simulink spezifiziert (Session 8)
**USB-Frame-Contract (SSOT, gelockt):** `gcs_frame.hpp` — 82 B, LE, fixe Länge:
`[0..1] AA 55 | [2] id | [3..78] 19×float32 (F_des, q_des[4], q_ref[4],
Omega_ref[3], tau_ref[3], q_ext[4]) | [79] estop | [80] ack | [81] crc8`
(CRC-8/SMBus Poly 0x07 über Bytes [2..80]).
- **Verify-first:** `test_gcs_frame` (CTest) — MATLAB `pack_gcs_frame.m` ↔ C++
  `gcs::parse` float32-exakt (66 Fälle) + CRC/Sync fangen Korruption. **27/27 grün.**
- **Sende-Teensy:** `hardware/gcs_sender.cpp` — Serial-Sync-Hunt → `gcs::parse` →
  float32→double → `pkt::pack(cmd, id, seq[id]++)` → `radio.write(29)`. seq **pro
  Drohne**. nRF identisch zum HAL: `0xE7E7E7E7E7`, **Kanal 76** (jetzt auch im
  Drohnen-HAL gepinnt), 1 Mbps, Auto-Ack aus.
- **Simulink-Seite (Spec, du baust):** in der GCS pro Drohne `id` setzen und
  `Bus_Cmd` **exakt nach `pack_gcs_frame.m`** in einen 82-B-uint8-Vektor packen
  (float32 LE via `typecast(single(...),'uint8')`, CRC-8 wie dort), dann per
  **Serial Send** (USB) rausschreiben. `pack_gcs_frame.m` ist die ausführbare Spec.

**Firmware-Compile verifiziert (Arduino-CLI, Teensy 4.1):**
- Teensy-Core `teensy:avr@1.60.0` + RF24 1.6.1 via `arduino-cli` installiert.
- `gcs_sender` **kompiliert** (FLASH code 14456). `drone_hal` **kompiliert** (FLASH
  code 53956, RAM1 ~448 KB frei) — inkl. ARM-generierter MCU-Klasse.
- **Bug gefangen + gefixt:** RF24-Teensy-Header macht `#define printf Serial.printf`
  → kollidiert mit unserem `Serial.printf` (Timing-Report). Im HAL nach dem
  RF24-Include mit `#undef printf` neutralisiert.
- **Compile-Rezept:** Sketch-Ordner (Headers + ggf. ARM-`mcu.*` ohne `ert_main.cpp`
  daneben), `arduino-cli compile -b teensy:avr:teensy41 <dir>`. Pfade **ohne
  Leerzeichen** (Projektpfad hat „MAS Versuchsaufbau" → im Scratchpad bauen).

### 3e. HAL — ✅ vervollständigt (Session 8, HW-Entscheidungen gelockt)
**Befund (wichtig):** der `led`-Ausgang ist **kein 25/50/75/100 %-Ladebalken**,
sondern eine **3-Zustands-Warn-FSM** aus `mcu.slx` (`MATLAB Function1`):
`led=state` ∈ {**0** NORMAL, **1** WARN (Vf≤14.0 V), **2** CRIT (Vf≤13.4 V)},
Hysterese 14.2/13.6. `landed` (Vf≤12.0) ist separat (Hard-Floor-Sinkflug intern,
nicht `led`). Die „25/50/75 %-Pins"-Suche war gegenstandslos.

**Gelockte Entscheidungen + im `drone_hal.cpp` umgesetzt:**
- **LED:** 2 diskrete LEDs — Pin5 = WARN (state≥1), Pin10 = CRIT (state==2).
- **ESC:** **keine** Boot-Kalibrierung (kein throttle-max-Sweep → sicher mit Props);
  nur Arming (min-Halten `ARM_MS`). ESCs extern vorkalibriert, Endpunkte 512/1024.
- **RF24:** `begin(&SPI1)` (Fallback `SPI1.setMOSI/MISO/SCK` als Kommentar).
- **Batt-Pin:** **40 = Spannung (A16, Wiring-Ist)** → `batt_count`, 41 = Strom
  (Telemetrie). `init_battery_manag.batt_pin=40` (Doku, im Codegen ungenutzt).
- **Timing-Budget:** `micros()` um den Tick, max-Dauer + Overruns, ~1×/s per
  `Serial [tick]`-Report (statt Platzhalter — im Betrieb ablesen).
- **API-Reconcile:** `mcu_packet.hpp` um `id_matches(buf,id)` + `unpack(buf,cmd)`
  (2-arg) ergänzt; HAL nutzt jetzt `pkt::Cmd`/`pkt::SIZE` (vorher `PktCmd`/`N_BYTES`
  → kompilierte nicht). SITL-Tests unberührt (25/25).

**HW-Bring-up-Log (Session 8):**
- **ESC (erledigt):** BLHeli_S + OneShot125 laufen mit **Default-Endpunkten** sauber
  (Motor spin ab ~5–10 % zuverlässig) → **kein Einlernen nötig** (Endpunkte sind bei
  OneShot125 protokoll-definiert 125/250 µs). **Drehrichtungen bereits korrekt**
  (M1/M3 CCW, M2/M4 CW). `esc_calibrate.cpp` bleibt als Bench-Werkzeug/Motortest.
  Bestätigt die arm-only-Entscheidung der Flug-Firmware.
- **MPU (erledigt):** `i2c_scan` findet **0x68** → ADO-Bodge sitzt.
- **nRF SPI1 (BUG gefunden + gefixt):** `RF24.begin(&SPI1)` **hing** (auch mit
  verdrahtetem Modul) — Teensy braucht SPI1 EXPLIZIT: `SPI1.setMOSI(26);
  setMISO(1); setSCK(27); SPI1.begin();` **vor** `RF24.begin(&SPI1)`, sonst
  blockiert der erste SPI-Transfer. In `drone_hal.cpp` **und** `gcs_sender.cpp`
  gefixt. Bench: `nRF ok=1 chip=1`.
- **Timing-Budget (bestätigt):** `tickmax ≈ 464 µs`, `overruns = 0/1000` →
  1-kHz-Tick mit >50 % Reserve. Der Report (`HAL_REPORT`) druckt es live.
- **Gyro (ok):** Bias 3 s abgezogen, ~0 still, reagiert auf Bewegung.
- **Acc (kleiner Offset):** z ≈ +9.2 (z-up korrekt), aber |a| ≈ 9.27 (~5 % niedrig)
  + y-Offset ~−1.0 → leichte Schräglage/Accel-Offset. Für Bench ok; Accel-Kalib
  (Bias/Scale) bei Bedarf separat.
  ⚠️ **Session 9 — die „6°-Schieflage" erklärt die Motor-Asymmetrie NICHT.** Siehe
  §3h: die Asymmetrie `thr[5 19 5 17]` ist die doppelte Gyro-Bias-Subtraktion.
  Nach dem Fix ist `throttle` bei Hover **symmetrisch** (`[23.42 ×4]`).
- **Batt (noch nicht prüfbar):** `batt=0(0.00V)` — nur USB, kein Flug-Akku am PM06.
  Erst mit angestecktem 4S-Akku verifizierbar.
- **✅ FAILSAFE-BUG GEFIXT (§3b-Re-Zert #2):** Der in §3b neue `throttle`-Outport
  war NICHT vom `latched`-Gate erfasst → da der HAL die ESCs aus `throttle` treibt,
  stoppte der Failsafe die Motoren nicht (Bench: `thr[2 16 …]` bei estop=2). **Fix
  (Nutzer):** in `mcu.slx` `throttle` durch das `latched`-Gate → generierter Code
  Z. 663 `if (latched) throttle[0..3]=0`. Regeneriert (Host + ARM), Golden neu,
  **Gate B 28/28** inkl. neuem `McuFailsafe.Estop2KillsThrottleAndRotor`
  (estop=2 → throttle==0 UND rotor_cmd==0). **Offen:** Drohne neu flashen →
  Bench-Gegentest `thr[0 0 0 0]`; Gate A (SIL) interaktiv.
- **✅ Accel: nichts zu tun.** `imu_acc` speist die Onboard-Attitude-Schätzung
  (Mahony `mcu_DW.q`, fusioniert mit `q_ext`/Mocap). Gains `ka=1.0`, `kE=25.0` →
  Mocap 26:1 dominant → ~6°-Schiefmontage ergibt nur **~0.23°** stationären Bias
  (`≈6°·ka/(ka+kE)`). Zudem wird der Accel **normiert** → der ~5 %-Betragsfehler
  kürzt sich raus (nur Richtung zählt). **`ka=1.0` lassen** (einzige Drift-
  Absicherung bei `Kᵢ=0`), **kein `R_align`, keine Kalib.**
- **✅ Batt: kalibriert + Modell-Semantik gefixt (Session 8).**
  1. *Verdrahtung:* Spannung lag versehentlich an **Pin 40** (nicht 41). HAL auf
     `PIN_BATT_V=40` gestellt (Pin 41 = Strom/Telemetrie). Danach `batt_count≈944`.
  2. *Scale:* realer Teiler **~20.7:1**, NICHT 18.182 (Datenblatt zu optimistisch).
     Messpunkt `batt_count=944 ↔ 15.74 V` → **`safety.batt_k = 15.74/944 = 0.0166737`
     V/count**, `b=0`. In `init_battery_manag.m` gesetzt + HW-kommentiert.
  3. *⚠️ Semantik-Bruch (gefunden + behoben):* mcu-Inport `batt_count` wurde im
     Modell als **Volt** behandelt — der Block **„volts to 12-bit counts"**
     (`na=round(V/k)`, ein ADC-Quantisierer) saß fälschlich *im* mcu-Subsystem
     (=Firmware). Auf HW (rohe counts rein) ergab das `Vf=k·round(944/k)=944` →
     Warn-FSM (14/13.4/12 V) blind, **Batterieschutz tot**. **Fix (Option A):**
     Quantisierer aus `mcu.slx` gelöscht, Inport direkt auf „MATLAB Function1"
     (macht `Vf=k·na`) → jetzt `Vf=k·batt_count=15.74 V`. Plant `main.slx`
     Gain „batt_voltage" auf **944** (counts) gesetzt. Host+ARM neu generiert
     (`Vf = 0.0166737 * batt_count`, Quantisierer weg), **Gate B 28/28** grün.
     HAL sendet weiter rohe `analogRead`-counts (passt zum Inport), Telemetrie-
     Print auf neues k. Offen: Selbsttest reflashen → Report muss **15.74 V** zeigen.
- **log_mcu_golden robust:** kommentiert die GS-`Serial`-Blöcke (die der Nutzer für
  §3d in `main`/`quadcop` einbaute) für die headless-Sim in-memory aus (Disk unberührt),
  sonst „Serial Configuration: No ports selected".
- **✅ Taster-Re-Arm `btn_ack` (Session 8).** `safety_overspeed` löst den Kill-Latch
  über `ack` (steigende Flanke), definiert als `Bus_Cmd.ack OR btn_ack`. Der
  physische Teensy-Taster war in `mcu.slx` eine **Konstante** → auf HW re-armte nur
  der Uplink. Jetzt: „push button drone"-Konstante → **Inport `btn_ack`** (boolean,
  ExtU-4. Eingang), generierter Code `ack = Bus_Cmd_l.ack || btn_ack`. HAL liest
  **Pin 21** `INPUT_PULLUP`, active-low (`btn_ack = digitalRead(21)==LOW`), Report
  zeigt `btn=`. Plant `quadcop.slx`: Constant1 (boolean false) speist den Port fürs
  Golden. ⚠️ Modellname `quadcop` kollidiert mit der Params-Var `quadcop` → Simulink
  warnt („shadowed by a variable"), nur Warnung.
- **✅ Safety im generierten Code getestet (Session 8).** `safety_overspeed`/
  `safety_battery` waren als **Algorithmus** unit-getestet (`test_safety.cpp` S1–S9,
  B1–B6, gegen `safety_helpers_ref.cpp`), aber das Golden speist nur benigne
  Eingänge → im **generierten** `mcu.cpp` liefen Trip/Eskalation nie. Zwei
  Integrationstests ergänzt (treiben ExtU direkt): `McuOverspeed` (‖gyro‖=9>8.5 →
  rotor/throttle=0; Latch hält ohne Flanke; `btn_ack`-Flanke → Freigabe) und
  `McuBattery` (Rampe → led 0→1→2 an 14.0/13.4 V, HW-kal. k). **Gate B 30/30**.
- **Regen-Runbook headless (Session 8):** `regen_full.m` (Scratchpad) mit
  `onCleanup`-Guard, der Modelle vor Exit `Dirty=off` setzt + `bdclose('all')` →
  **keine Disk-Kollision** mehr, wenn die interaktive MATLAB-Session gleichzeitig
  offen ist (sonst „file changed on disk"/Save-Dialog-Absturz). Nach Inport-
  Änderung: `IN_NAMES` in `log_mcu_golden.m` mitziehen, sonst Zähler-Assert.
- **✅ GS-Kette end-to-end auf HW bewiesen (Session 8).** `gcs_sender.cpp` auf den
  Sende-Teensy geflasht (nRF-Params IDENTISCH zum Drohnen-HAL: Adr `0xE7E7E7E7E7`,
  Kanal 76, 1MBPS, Auto-Ack aus, 29-B; Bring-up-Heartbeat auf Pin 13 = LED blinkt
  nur bei CRC-ok Frames). Kette `Simulink GCS → pack_gcs_frame → USB → gcs::parse
  → pkt::pack → nRF → Drohne (id-Gate → unpack → Regler → throttle)` verifiziert:
  `estop=0`, `link` 1–30 ms, `thr` folgt `F_des` (`0.2·m·g → [5 19 5 17]`; Asymmetrie
  = Attitude-Korrektur der 6°-Schieflage). Simulink braucht **Simulation Pacing 1.0×**
  (sonst Frame-Burst → Watchdog killt sofort) und `id` = BCD-ID der Drohne.
- **⚠️ Operatives Boot-/Re-Arm-Verhalten (WICHTIG für jeden Test).** Die Drohne
  bootet mit `estop=2` (kein Link) → `safety_overspeed`-Hard-Kill-**Latch gesetzt**.
  Sobald der Link steht (`estop=0`), bleiben rotor/throttle **weiter 0**, bis eine
  **steigende `ack`-Flanke** (Taster Pin 21 ODER GCS `ack=1`-Puls) den Latch löst
  (nur bei `~overspeed & estop≠2`). Danach bleibt scharf, solange Link steht; jeder
  Link-Verlust (`estop=2`) killt+latcht erneut → erneuter Re-Arm nötig.
- **⚠️ `batt_land` ist PERMANENT (kein Re-Arm).** Fällt `Vf ≤ V_floor=12.0 V`, latcht
  `safety_battery` → `safety_landcmd` überschreibt `F_des` auf `0.99·m·g` (Notabstieg,
  throttle springt HOCH). Bleibt bis **Power-Cycle** aktiv, auch wenn Spannung sich
  erholt (per Design gegen Sink↔Schweben-Grenzzyklus). Beim Testen mit Netzteil also
  stets > 12 V halten, sonst latcht der Notabstieg und nur Neustart löst ihn.
- **❌ Arming-Idle-Interlock (Session 8) — in Session 9 VERWORFEN, siehe §3h.**
  War: Re-Arm nur bei `F_des <= safety.F_rearm_idle` (=0.1·m·g = 0.9467 N).
  Zurückgebaut, weil er das Re-Armen nicht motorfrei machte (throttle springt
  ohnehin auf 8.404 %) und den lokalen Taster von der GCS abhängig machte.
  Re-Arm hängt wieder allein an `ack-Flanke & ~over_inst & estop≠2`.
- **Standalone-Safety-Leaves (`gen_lib_codegen.m`):** Build-Dir nutzt
  `SAFETY_IMPL=codegen` → `test_safety` läuft gegen den generierten Standalone-
  `safety_overspeed` (Shim), NICHT die Referenz. Nach Signatur-Änderung MUSS
  `gen_lib_codegen.m` neu laufen (Args + `os_p.F_rearm_idle` mitziehen), sonst
  Shim-Compile-Fehler. Referenz `safety_helpers_ref.cpp` parallel gepflegt.

**Firmware-Build-Hinweis:** `drone_hal.cpp` braucht auf dem Include-Pfad die
ARM-`mcu.h` (`hardware\mcu_arm\mcu_ert_rtw\`) **und** die SSOT `mcu_packet.hpp`
(`scripts\sitl\include\`) — beide ins Teensy/PlatformIO-Projekt ziehen.

### 3f. ARM-Codegen-Config — ✅ ERLEDIGT (Session 8)
`configure_mcu_codegen(mdl, target)` **parametrisiert**: `target='host'` (Default,
SITL/x86, Verhalten wie bisher, Config `ert_cpp_sitl`) | `target='arm'`
(Cortex-M7, Config `ert_cpp_arm`). ARM setzt `ProdHWDeviceType='ARM
Compatible->ARM Cortex-M'`, `ProdEndianess=LittleEndian`, `ProdLongLongMode=on`,
`ProdEqTarget=on`; gemeinsam: C++ class `MCU`, SingleTasking, DISCRETE `Ts_inner`,
GenCodeOnly, kein `-ffast-math`.
Runner `run_mcu_arm_codegen.m` lenkt CodeGen-/Cache-Ordner nach **`hardware\mcu_arm\`**
um (eigenes `slprj`) → **SITL-`scripts\sitl\mcu_ert_rtw\` bleibt unberührt** (Gate B
weiter 25/25).
**Verifiziert (ohne ARM-Binary, GenCodeOnly):**
- x86-Intrinsics: Host 1 Quelldatei (`<emmintrin.h>`), **ARM 0** ✓
- `mcu.cpp`-Kommentar: **„ARM Compatible->ARM Cortex-M"** (Host: Intel x86-64) ✓
- `rtwtypes.h` ARM: `int32_T=int`, `int64_T=long long` (long=32→64 via long long),
  **`real_T=double`** (double behalten) ✓
- `class MCU final`, genau **ein `step()`**, ExtY `rotor_cmd[4]`+`throttle[4]` ✓
**Offen für Deployment:** Kompilat via Teensy/PlatformIO-Toolchain (nicht aus
MATLAB); `-ffast-math` aus, FPU round-to-nearest im Firmware-Compiler setzen.
Der ARM-Code (`hardware\mcu_arm\mcu_ert_rtw\`) sitzt neben `drone_hal.cpp`.

### 3h. Session 9 — Re-Arm/F_des-Sweep → Gyro-Bias-Bug gefunden

**Auslöser:** Commit `b4073ea` („Need to investigate if it useful as implemented")
+ F_des-Sweep. Der Sweep hat den Interlock beantwortet **und** einen Sim≠HW-Bug
freigelegt. Alles gegen `mcu.slx` in MATLAB gefahren (kein Compiler, s.u.).

#### ⛔ BEFUND 1 (kritisch): doppelte Gyro-Bias-Subtraktion — GEFIXT
`mcu.slx` rechnete `gyro_corr = imu_gyro − imu.gyro_bias` (`Constant1`), und der
Mahony zog `b_ground` (**dieselbe** Konstante, Inport 4) intern nochmal ab. Der
Block-Header sagt es selbst: *„imu_gyro … (mit Bias+Rauschen)"* — das Modell
erwartet **rohes** Gyro. `drone_hal.cpp` Z.281 sendet aber **bias-korrigiertes**.

| | liefert `imu_gyro` | `mcu` zieht ab | `gyro_corr` |
|---|---|---|---|
| Sim | `sensors.slx` addiert `imu.gyro_bias` | `imu.gyro_bias` | wahr ✓ |
| HW | HAL zieht **echten** Bias ab | `imu.gyro_bias` (fiktiv!) | wahr **− [10,−10,10]°/s** ✗ |

`imu.gyro_bias = deg2rad([10;-10;10])` ist ein *fiktiver* Sensor-Modellwert
(„repräsentativ, vor Kalibrierung") — auf HW permanente Schein-Drehrate ⇒
Störmoment `tau = kΩ·bias ≈ [0.023,−0.018,0.038] N·m` + verfälschter Mahony.
**Gate B konnte das per Konstruktion nicht sehen** (in der Sim heben sich beide auf).

Verifikation (`mcu.slx`, level, `F_des=0`, armed):

| `imu_gyro` | `throttle` [%] | `rotor_cmd` |
|---|---|---|
| `0` (was der HAL sendet) | `[1.78, 15.87, 1.80, 13.83]` | `[729, 784, 728, 667]` |
| `imu.gyro_bias` (roh) | `[8.404 ×4]`, Abw. **0.0e+00** | `[0,0,0,0]` |

- **Erklärt `thr[5 19 5 17]` neu:** die Sim reproduziert das Muster (1,3 niedrig /
  2,4 hoch) bei **perfekt levelem** Sensor → **nicht** die 6°-Schieflage.
- **Nebenbefund:** dabei wird `ω²` negativ (Kanal 1,3) → `rotor_cmd=sqrt(|ω²|)=728`
  (Sim: Rotor dreht) vs. `throttle=1.78 %` (HW: Motor fast aus) — die Ausgänge
  widersprechen sich. Genau der in §3b als ungetestet geflaggte Sign-Pfad.
- **Zwischenschritt (verworfen):** erst nur `imu.gyro_bias = zeros(3,1)` — beide
  Enden auf 0, kein Modell-Eingriff. Funktionierte, war aber **fragil** (Logik
  blieb scharf, sobald jemand den Parameter ≠0 setzt) **und optimistischer als die
  HW** (s.u.). Auf Nutzer-Vorschlag ersetzt durch den strukturellen Umbau.
- **✅ Fix (final, strukturell):** Bias-Logik **aus der Firmware entfernt**,
  HAL-Nachbau in die Sim:
  - `mcu.slx`: `Constant1` **gelöscht**, `Subtract` **gelöscht**
    (`Bus Selector <imu_gyro>` → `MATLAB Function` IN1 direkt), Mahony-Inport
    `b_ground` **gelöscht**; `mahony_filter.m` (SSOT) jetzt 7 Args
    (`omega = imu_gyro + omega_mes`, `Omega_hat = imu_gyro`).
  - `sensors.slx`: neuer Sum **`HAL gyro bias`** (+−) + Constant `gyro_bias_hat`
    zwischen Gyroskop und `Bus Creator` (mit Annotation).
  - `init_sensors.m`: `imu.gyro_bias = deg2rad([10;-10;10])` (**roher Sensor-Bias,
    zurück**), neu `imu.gyro_bias_hat = imu.gyro_bias` (HAL-Schätzung).
- **⚠️ WARUM nicht einfach `gyro_bias = 0`** (der entscheidende Grund): Im
  Aerospace-Gyro-Block wirkt der Bias **VOR der Saturation** — Kette:
  `ω→ZOH→×M ─┐ Measurement bias ─┼→Sum4(+++)→Dynamics→Sum1(+Rauschen)→**Saturation**→out`.
  Mit `gyro_bias=0` sähe die Saturation den Bias nie. Sie ist aber knapp
  (FSR 8.7266 vs `omega_max` 8.5 ⇒ 0.2266 Marge; `|bias|`=0.1745 ⇒ **77 %**).
  Real: Sensor sättigt inkl. Bias, HAL zieht danach ab ⇒ Obergrenze ≈ 8.552,
  **effektive Marge ~0.05 rad/s**. Der Umbau bildet das ab; `=0` hätte die Sim
  **optimistischer als die Hardware** gemacht.
- **Verifiziert (Gate B 30/30 nach dem Umbau):**
  - Golden: `imu_gyro` mean = `[-0.00095, -0.00184, -0.00177]` statt `±0.1745`
    ⇒ HAL-Nachbau zieht den aufgeprägten Bias sauber ab, MCU-Grenze bias-frei.
  - Host **und** ARM: `0.1745`/`0.94666` je **0×**; `over_inst` rechnet direkt auf
    `imu_gyro`; ARM: 0 x86-Intrinsics, „ARM Cortex-M".
  - Frühere Messung (vor dem Fix): `imu_gyro=0` → `throttle=[8.404 ×4]` exakt,
    `rotor_cmd=[0,0,0,0]`, Abw. `0.0e+00`; Hover → `[23.4194 ×4]` **symmetrisch**.

#### ❌ BEFUND 2: Arming-Idle-Interlock verworfen (Entscheidung Nutzer)
F_des-Sweep gegen `mcu.slx` (bias-korrekt, level):
- Schwelle **bit-exakt**: Re-Arm bis `0.946665` N, blockiert ab `0.946666` N (`<=`).
- **Aber `throttle` im Löse-Tick ist nicht 0, sondern `polyval(P,0)=8.404 %`** —
  OneShot125 ~555 counts/135 µs, **über** der Anlaufschwelle (~5–10 %): die Props
  laufen beim Re-Armen ohnehin an. „Schub runter zum Armen" ist nicht motorfrei.
- Gewinn nur **9.94 % statt 23.43 %** throttle (13.5 Prozentpunkte).
- Preis: der Taster (Pin 21) — die einzige **lokale** Freigabe — war wirkungslos,
  solange die GCS >10 % Hover sendet, **ohne Rückmeldung am Gerät**.
→ Zurückgebaut in `safety_overspeed.m` (Begründung im Schlusskommentar — **nicht
ohne neue Argumente wieder einbauen**), `init_safety.m`, `safety_helpers.h`,
`safety_helpers_ref.cpp`, `codegen_shim_overspeed.cpp`, `gen_lib_codegen.m`,
`test_safety.cpp` (S10 raus), `test_mcu_model.cpp` (jetzt
`McuOverspeed.KillHoldsAndReArmsOnAckEdge` + „gehaltenes ack löscht frischen Trip
nicht"), `mcu.slx` (Block 4→3 Inports, `<F_des>`-Linie gelöst).
Generierter Code Z.199 jetzt ohne `F_des`-Term. **Bedien-Hinweis §3e („erst F_des
≤10 % runter") ist damit hinfällig.**

#### ⚠️ BEFUND 3: throttle-Sättigung über `F_des` NICHT erreichbar
Bis `F_des=60 N` (6,3× Hover) nur **87,6 %** throttle; rechnerisch bräuchte es
~83 N (8,8× Hover). Die §3b-Coverage-Lücke (Sättigung) lässt sich **nur über
τ/Attitude-Fehler** schließen — ein F_des-Sweep genügt nicht.

#### Stand / was verifiziert ist
- `mcu.slx`: 3 Inports, `Constant1=[0 0 0]`, kompiliert, **persistent** geprüft.
- **Golden neu** (`golden_mcu_io.csv`, 5001×40). Ersatz-Check ohne Compiler:
  `mcu.slx` standalone mit den Golden-Eingängen replayt → **max|d| = 3.07e-11**
  (Gate-B-Toleranz 1e-9) ⇒ Aufzeichnung/ZOH/Spalten konsistent.
  Golden ggü. HEAD stark geändert (`rotor_cmd` bis 185): erwartet — `imu_gyro`
  trägt den Bias nicht mehr, und die alte Kompensation war um `(M−I)·b` ohnehin
  ungenau (Bias lief durch `imu.gyro_M`) ⇒ Trajektorien-Divergenz über 5 s.
- **Codegen neu, Host + ARM**, beide verifiziert frei von `0.94666` und `0.1745`;
  ARM zusätzlich: 0 x86-Intrinsics, „ARM Cortex-M". Config zurück auf `host`.
- `throttle_poly.hpp`: nur ULP-Drift (`…5487e-13`→`…5336e-13`) — `polyfit` ist
  nicht bit-stabil über Maschinen; 1e-9-Toleranz trägt es.

#### ✅ Gate B: 30/30 GRÜN (zertifiziert)
MSVC 2022 **BuildTools** nachinstalliert (`vswhere` →
`C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools`), MATLAB-`mex`
darauf gesetzt. Damit ist der Fix **verifiziert**, nicht nur plausibel:
- `McuGolden.RotorCmdMatchesGolden` grün ⇒ **generierter Code == neuer Golden**
  (throttle-Diff ≤1e-9 + Polyval-Invariante).
- `McuOverspeed.KillHoldsAndReArmsOnAckEdge` grün (neue Re-Arm-Semantik).
- **31→30** ist erwartet: genau der entfernte `Overspeed.S10`.
  `test_safety` zeigt S1–S8 (S9 bei `SAFETY_IMPL=codegen` bauartbedingt aus,
  S10 entfernt) ⇒ läuft wirklich gegen den generierten Shim.
- `gen_lib_codegen.m` neu gelaufen → Leaf-Signatur jetzt
  `safety_overspeed(gyro_corr, estop, ack, kill, fault_src, dbg)`.
- `prune_mcu_configs('mcu')`: 9 Dups entfernt (`ert_cpp_sitl1..5`,
  `ert_cpp_arm1..4`), behalten `Configuration/Reference/ert_cpp_sitl/ert_cpp_arm`,
  aktiv `ert_cpp_sitl`. **`mcu.slx` 756 KB → 312 KB.** Gate B danach erneut 30/30.

**Build-Rezept (Session 9, reproduziert):**
```
cmake -S <sitl> -B C:\dsb -G "Visual Studio 17 2022" -A x64 ^
  -DQUAT_IMPL=codegen -DSAFETY_IMPL=codegen -DCODEGEN_ROOT=<sitl>/codegen ^
  -DMATLAB_ROOT="C:/Program Files/MATLAB/R2025b" -DGOLDEN_TOL=1e-9
cmake --build C:\dsb --config Release && ctest -C Release
```
- ⚠️ **Build-Dir KURZ halten** (`C:\dsb`): MSBuilds FileTracker bricht bei langen
  Pfaden mit `FTK1011`/`MSB8029` ab (MAX_PATH, kein Temp-Verzeichnis).
- ⚠️ Der eingecheckte `scripts\sitl\build\` hat einen **Fremd-Cache**
  (`C:/Users/Nick/thesis_doctoral/…`) → nicht verwenden, neu konfigurieren.

#### 🔬 HW-TESTPLAN (Session 10) — Bench, Drohne am Boden

**⚠️ Die gebaute Firmware ist PRÜFSTAND-Firmware.** `drone_hal.cpp` **Z.43**
`#define HAL_SELFTEST` ist **hart aktiv** (nicht auskommentiert!). Folge:
- `esc_arm()` entfällt, und der Tick schreibt **`analogWrite(PIN_PWM[i], ESC_MIN)`**
  statt `esc_write_all(y.throttle)` ⇒ **Motoren drehen NIE**, egal was `thr` sagt.
- `selftest_report()` druckt ~10 Hz über Serial: `id, gyro[3], acc[3], batt(V),
  bias[3], link(ms), estop, btn, thr[4], tickmax`.
⇒ **Für alle Tests unten ideal und sicher** (`thr` ist ablesbar, ohne dass sich
etwas dreht). **Für den Flug MUSS Z.43 auskommentiert und neu geflasht werden.**
> **NACHTRAG Session 11 — der obige Nebenbefund war eine Fehldiagnose.**
> Die byte-identischen `.hex` waren **doch** ein Bug: die Teensy-Recipe in
> `platform.txt` referenziert `compiler.cpp.extra_flags` überhaupt nicht, das
> `-D` verpuffte also **immer** wirkungslos. Dass es trotzdem passte, lag allein
> am fest eingebauten `#define` — die beiden Fehler haben sich gegenseitig
> verdeckt. Praktische Folge: **`--upload-drone` hat nie Flug-Firmware geflasht**,
> sondern immer die Bench-Variante mit toten Motoren.
> Behoben durch drei benannte Betriebsarten über einen generierten Header
> (`hal_mode.h`), siehe §Betriebsarten. Nachgewiesen an drei unterschiedlichen
> FLASH-Größen (BENCH 55236 / THRUST 55492 / FLIGHT 55044).
> **Lehre:** „gleiche Binärgröße trotz unterschiedlichem Flag" nie als harmlos
> abtun — das ist der Standardbefund für ein wirkungsloses Flag.

**Flashen:** `hardware\build\drone_hal\drone_hal.ino` in der IDE (Board Teensy 4.1),
oder `./build_sketches.sh --upload-drone COM<N>`. Sende-Teensy: `--upload-sender`.
Vorher Serial-Monitor auf dem Ziel schließen (sonst hängt der Loader).

| # | Test | Ergebnis |
|---|---|---|
| **T1** ⭐ | **Gyro-Bias-Fix** — der Nachweis von §3h | ✅ **PASS.** `thr[11 12 12 11]` bei `F_des=1.893 N` — **symmetrisch** (Spreizung 1 statt 14). Vorher `[5 19 5 17]`. Deckt sich mit dem Sim-Sweep (2.0 N → 11.66 %) ⇒ **Sim == HW** wiederhergestellt |
| **T2** | **Failsafe** *(offen seit §3b-Re-Zert #2)* | ✅ **PASS.** `link=8417ms → estop=2, thr[0 0 0 0]` exakt |
| **T3** | **Batt-`k` an Pin 41** | ✅ **PASS** (Nutzer verifiziert) |
| **T4** | **Batt-Report** *(offen seit §3e)* | ✅ **PASS.** `batt=902(15.04V)` — plausibel für 4S, konsistent (`902·0.0166737=15.04`), nicht 0/944 |
| **T5** | **Re-Arm ohne Interlock** *(neu, §3h)* | ✅ **PASS.** `btn=0→thr[0 0 0 0]`, `btn=1→thr[11 12 12 11]` bei **20 % Hover** — mit dem alten Interlock (10 %) hätte der Taster NICHT gelöst |
| **T6** | **Gyro/Bias-Plausibilität** | ✅ **PASS.** `bias[0.027 0.015 -0.029]`, `gyro≈0` still. Timing `tickmax=461µs, overruns=0/1000` |

**⭐ Damit ist der Session-9-Befund auf Hardware bewiesen** — die doppelte
Bias-Subtraktion war die Ursache der Motor-Asymmetrie, nicht die Schieflage.

**Zwei Messwerte, die Annahmen korrigieren:**
1. **Realer Gyro-Bias ist 6× kleiner als die Sim-Annahme:** gemessen
   `[1.5, 0.8, -1.7] °/s` vs. Modell `[10, -10, 10] °/s` (Datenblatt-Worst-Case).
   ⇒ **Overspeed-Marge real viel besser**: 0.029 rad/s = **13 %** der 0.2266-Marge
   (statt 77 %), effektive Obergrenze ≈ 8.698 statt 8.552 rad/s. Der offene Punkt
   „Overspeed-Marge" entschärft sich damit deutlich; die Sim rechnet konservativ,
   was für Robustheit gut ist. `imu.gyro_bias` **bewusst NICHT** auf den Messwert
   gesenkt (Worst-Case behalten).
2. **Die 6°-Schieflage ist real**, aber harmlos: `acc[-0.03 -1.07 9.44]` →
   `atan(1.07/9.44) ≈ 6.5°` um X. Erklärt (wie §3e rechnet) nur ~0.23°
   stationären Fehler — und **nicht** die `thr`-Asymmetrie.

⚠️ **`batt_land` ist PERMANENT:** bei `Vf ≤ 12.0 V` latcht der Notabstieg bis
Power-Cycle (§3e). Am Netzteil **stets > 12 V** halten.

#### 🆕 `bench.slx` — Prüfstand-Harness (Session 9)
**Problem:** `quadcop.slx` schafft nur ~45 % Echtzeit (3700 missed Ticks in 6.76 s
am `Real-Time Synchronization`). Ursache ist **die Basisrate, nicht die Blockzahl**:
`quadcop` hat `FixedStep = Ts_inner` = **1 ms** (Plant/MCU brauchen das) ⇒ 1000
Ticks/s inkl. Plant+Sensoren+MCU+Link. **Deshalb sind Variant Subsystems hier die
falsche Wahl** — sie schalten Blöcke um, aber **nicht den Solver-Step**.
Zusatzeffekt: `Serial Send` hat keine eigene SampleTime und **erbt die Basisrate**
⇒ in `quadcop` bis zu **1000 Frames/s** an den Sende-Teensy (daher der §3d-Zwang
zu „Pacing 1.0×, sonst Frame-Burst → Watchdog killt").

**Lösung:** `models\bench.slx` = **`quadcop` minus Drohnen-Simulation**, abgeleitet
per `save_system` (Serial-Kette bleibt 1:1), **18 statt 27** Top-Level-Blöcke:
- **`FixedStep = Ts_gcs` (10 ms)** ⇒ 10× weniger Ticks, `Serial Send` erbt
  100 Hz ⇒ **100 Frames/s**. Die Drohne rechnet ihre 1 kHz selbst auf dem Teensy.
- `SimulationMode = accelerator`, `StopTime = inf`.
- **Entfernt:** `simulation of the plant/sensors/link`, `running on the quadrocopter
  MCU`, `Rate Transition`(1), `batt_count`, `Constant`/`Constant1`, beide `Terminator`.
- **Behalten:** Test-Konstanten → `Bus Creator` → `MATLAB Function1`
  (`pack_gcs_frame_sl`) + `quadcop_id` → `Serial Send` (COM4, 115200) +
  `Serial Configuration`, `Real-Time Synchronization`.
- `gcu` bleibt **Model-Referenz** (SSOT, kein Drift), läuft mit Dummy-Bussen mit;
  sein `Bus_Cmd` geht vorerst auf `gcu_cmd_unused` (Terminator) — der Uplink kommt
  wie gehabt aus den Test-Konstanten (so war es auch in `quadcop`!).

#### ✅ Motive/OptiTrack angebunden (Session 9)
**Plugin:** `DROMA\Motive\OptiTrack_MATLAB_Plugin_1.1.0\` (entpackt, **nicht im
Repo** — Third-Party + DLLs, `Motive/` ist untracked). Es ist ein **MATLAB**-
Plugin (`natnet.m` + `NatNetML.dll`, .NET), **kein** Simulink-Blockset.

**Neu:**
| Datei | Rolle |
|---|---|
| `scripts\motive\MotiveMocap.m` | `matlab.System`-Quelle: NatNet → `mocap_pos(3)`, `mocap_quat(4)`, `valid` |
| `scripts\motive\setup_motive_path.m` | Pfade + schreibt `assemblypath.txt` |
| `init_sensors.m` | neu: `mocap.host_ip`, `.client_ip`, `.streaming_id` |
| `bench.slx` | `Motive` (MATLAB System) → `Bus_Mocap_from_Motive` → `gcu` IN1; `mocap_valid` (Display) |

**Gelockte Konventionen (je EINE Stelle — Lehre aus §3h!):**
- **Quaternion:** NatNet liefert `qx,qy,qz,qw` = **scalar-last**; das Projekt ist
  **scalar-first** `[w x y z]`. Umsortierung **einmalig in `MotiveMocap`**
  (`[qw;qx;qy;qz]`). **Downstream NICHT nochmal drehen.**
- **⚠️ Up-Axis:** Motive **MUSS auf Z-Up** streamen (Motive → Settings →
  Streaming → *Up Axis = Z*). `MotiveMocap` transformiert **bewusst NICHT** —
  eine zweite Korrekturstelle wäre exakt der Doppel-Kompensations-Fehler aus §3h.
  Steht Motive auf Y-Up, ist die Pose falsch (fällt im Plausibilitätscheck auf).
- **Einheit:** NatNet liefert **Meter** (das OptiSample skaliert nur für die
  mm-Anzeige mit 1000). Kein Skalieren.

**Fallstricke (alle gelöst — nicht erneut hineinlaufen):**
- `natnet.setAssemblyPath` öffnet ein **`uigetfile`-Fenster** → hätte Simulink
  blockiert. `getLastAssemblyPath` liest `<plugin>\Matlab\assemblypath.txt`;
  die wird von `setup_motive_path` vorab geschrieben ⇒ kein Dialog.
  Verifiziert: `NatNetML 4.1.9210.17151` lädt.
- `matlab.System` braucht einen **expliziten** Name-Value-Konstruktor
  (`setProperties`), sonst „No matching constructor found for superclass".
- **`.NET` ist nicht codegen-fähig** — schon `try/catch` bricht mit *„Try and
  catch are not supported for code generation"*. Der Modus wird deshalb
  **klassenseitig erzwungen** (`getSimulateUsingImpl` → `'Interpreted execution'`,
  `showSimulateUsingImpl` → `false`); der Block-Parameter ist dadurch **read-only**
  und kann nicht versehentlich auf Codegen zurückfallen.
- **⚠️ Simulink wertet CHAR-Dialogfelder eines MATLAB-System-Blocks NICHT aus.**
  `StreamingID='mocap.streaming_id'` (numerisch) → wird zu `1` ✔, aber
  `HostIP='mocap.host_ip'` kam **wortwörtlich** als String an (Log zeigte
  „Host mocap.host_ip"). Damit `params.m` die einzige Konfigurationsstelle bleibt
  (statt IPs im binären `.slx` zu vergraben), löst `MotiveMocap.resolveIP` das
  selbst auf: sieht der Wert wie eine IP aus → direkt, sieht er wie ein
  Variablenname aus → `evalin('base',…)`. Verifiziert: „verbunden (Host 127.0.0.1)".
- **Robust ohne Motive:** kein Absturz, `valid=false` + ZOH-Pose ⇒ der Prüfstand
  bleibt ohne Motive testbar. Verifiziert (Smoke-Test: `ReachedStopTime`).

**⚠️ `.gitignore`-Falle (Session 9, zweimal zugeschlagen):**
1. Patterns mit `/` sind relativ zur `.gitignore` (Repo-Wurzel `DROMA\`). Alle
   Regeln standen ohne `Simulation/`-Präfix ⇒ **keine griff**; der komplette
   Codegen-Output wurde jahrelang mitcommittet. Korrigiert; zusätzlich mit
   `git rm -r --cached` aus dem Index genommen (1318 Einträge, **keine Quellen**).
2. `Motive/` **ohne führenden Slash** matcht JEDEN `Motive`-Ordner im Baum — und
   Git ist auf Windows **case-insensitive**, also auch `Simulation\scripts\motive\`
   ⇒ `MotiveMocap.m` wäre nie ins Repo gekommen. Jetzt **`/Motive/`**.
   Prüfen mit `git check-ignore -v <PFAD>` (der Befehl braucht einen Pfad).

**⚠️ Regelkreis noch OFFEN (bewusst):** Der Uplink kommt weiterhin aus den
**Test-Konstanten**; `gcu`-`Bus_Cmd` hängt noch am `gcu_cmd_unused`-Terminator.
Erst Mocap prüfen (`mocap_valid=1`, Pose plausibel), **dann** `gcu`-`Bus_Cmd`
statt des `Bus Creator` auf `pack_gcs_frame_sl` legen.
`gcu` ist dafür **schon vorbereitet**: `gcu/Switch`+`Switch1` haben
`Criteria = u2 > 0` mit `u2 = Constant9 = 1` ⇒ es gewinnt **immer der
Luenberger-Observer** aus `mocap_pos`; `Bus_State` wird bereits ignoriert und ist
im Stand nur eine Attrappe (`dummy_Bus_State`).

**Inbetriebnahme:** `setup_motive_path()` (einmal pro Session, oder ins
`DROMA.prj`-StartupFcn), Motive-Streaming an, Rigid-Body-ID → `mocap.streaming_id`,
dann `bench.slx` starten und `mocap_valid` beobachten.

**Arbeitsteilung ab jetzt:** `quadcop.slx` = reine Simulation (1 kHz, Golden/Gate B),
`bench.slx` = Versuchsstand (100 Hz, Uplink). Beide teilen `gcu`/`pack_gcs_frame`
als SSOT.

**✅ AUF HW VERIFIZIERT (Session 9):**
- **`missed ticks = 0`** (vorher 3700 in 6.76 s ⇒ ~45 % Echtzeit). Der Prüfstand
  läuft jetzt echtzeitfähig.
- **`link = 8 ms` (Mittelwert)** — passt zum 10-ms-Sendeintervall und liegt
  **12,5× unter** dem 100-ms-Watchdog. Uplink gesund.
⇒ Die Diagnose ist damit bestätigt: es war die **Basisrate**, nicht die Blockzahl.
Der §3d-Zwang zu „Simulation Pacing 1.0×" entfällt für `bench.slx` — bei 100 Hz
gibt es keinen Frame-Burst mehr (Pacing bleibt für `quadcop.slx` relevant).

#### 🔴 OFFEN — Analyse (nicht Bench)
1. **⚠️ Overspeed-Marge bewerten (neu, Session 9).** Der Bias frisst 77 % der
   Marge zwischen `omega_max`=8.5 und FSR=8.7266 (effektiv ~0.05 rad/s). Jetzt,
   wo die Sim den Bias VOR der Saturation modelliert, ist das messbar. Optionen:
   `omega_max` senken, Gyro auf **FS_SEL=2 (±1000 dps)** stellen (halbiert die
   Auflösung, verdoppelt den Headroom), oder belassen und per Sim-Kampagne
   belegen, dass die Entprellung (`debounce_N=4`) trägt. Vorher **keine**
   aggressiven Flüge nahe der Rate-Grenze.
2. **Sättigungs-Coverage** (§3h Befund 3): über `F_des` nicht erreichbar, nur über
   τ/Attitude-Fehler.
#### ❌ Gate A ABGESCHAFFT (Session 9) — Gate B ist die alleinige Zertifizierung
`run_gate_a.m` **gelöscht**; `sil_check_mcu.m` bleibt nur als Diagnose-Werkzeug
(Header sagt es jetzt explizit). Begründung:
- Laut eigenem Header nur ein **grober Äquivalenz-Check, KEIN Bit-Diff** — Gate B
  ist strikt schärfer (tick-exakt ≤1e-9, 9 Kanäle, Determinismus, Safety).
- Der Golden stammt **selbst aus dem geschlossenen Kreis** (`quadcop`) → SIL fährt
  dieselbe Trajektorie. Kein Erkenntnisgewinn.
- Deckt nur Simulinks **Modellreferenz-Integration** ab — die es auf der Drohne
  nicht gibt (dort verdrahtet `drone_hal.cpp` ExtU/ExtY von Hand). Das fängt weder
  A noch B, nur der HW-Test.
- Headless scheitert er an `rtwshared` — **auch mit MSVC 2022** ⇒ es ist das
  `-batch`-SIL-Setup, nicht die Toolchain. Nur interaktiv, also teuer.

#### 🔧 Arduino-Toolchain auf DIESER Maschine eingerichtet (Session 9)
Es fehlte **alles** (nur `arduino:avr` + Pololu waren da):
- Board-Manager-URL **PJRC**: `https://www.pjrc.com/teensy/package_teensy_index.json`
- **`teensy:avr@1.60.0`** (bewusst NICHT 1.62.0 — 1.60.0 ist die in §3d verifizierte)
- **`RF24@1.6.1`** (TMRh20). `Wire`/`SPI` kommen mit dem Teensy-Core.
```
CLI="C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe"
& $CLI config add board_manager.additional_urls https://www.pjrc.com/teensy/package_teensy_index.json
& $CLI core update-index ; & $CLI core install teensy:avr@1.60.0 ; & $CLI lib install "RF24@1.6.1"
```
**Verifiziert:** `./build_sketches.sh --compile` → alle 5 Sketches kompilieren mit
dem NEUEN ARM-Code. `drone_hal` FLASH **54916**, RAM1 447 KB frei; `gcs_sender`
15432; `i2c_scan` 36964; `esc_calibrate` 37204. Projektpfad hat **keine
Leerzeichen** → die §3d-Falle entfällt hier.
Flashen: `hardware\build\<name>\<name>.ino` in der IDE öffnen (Board „Teensy 4.1")
oder `./build_sketches.sh --upload-drone COM<N>`.
⚠️ **Auffällig:** `drone_hal` und `drone_hal -DHAL_SELFTEST` ergeben **exakt
dieselbe** FLASH-Größe (54916) → das Define kommt vermutlich nicht an.
✅ **Session 11: bestätigt und behoben.** Der Verdacht stimmte — die Teensy-Recipe
kennt `compiler.cpp.extra_flags` nicht. Betriebsart läuft jetzt über den
generierten Header `hal_mode.h` statt über `-D`.

#### ✅ SSOT für MATLAB-Function-Blöcke — PoC erfolgreich (Session 9)
**Befund (Audit über alle 6 Modelle, 11 Function-Blöcke):** 5 identisch,
**2 driften** (`safety_battery` in `mcu`, `pack_gcs_frame` in `quadcop`),
**4 haben gar keine `.m`** (`traj_gen`, `pos_ctrl`, 2× landcmd — nur inline).
Der Drift ist **nur in Kommentaren** — aber `safety_battery` trug im Block noch
`analogRead(A16/Pin40)`, während die Datei korrekt `A17/Pin41` sagt. Der
Mechanismus ist also real, er hat nur noch keinen Code erwischt. Verschärfend:
`.slx` ist **binär** → Code-Änderungen sind im git-Diff unsichtbar (in Session 9
hat `openProject` genau so unbemerkt Blockcode zurückgerollt).

**PoC an `mcu/MATLAB Function`:** Block hält nur noch einen Wrapper
`safety_overspeed_sl(...)`, der `scripts\functions\safety_overspeed.m` aufruft.
- Name MUSS abweichen (`…_sl`), sonst schattet er die externe Funktion → Rekursion.
- Argument-/Ausgangsnamen MÜSSEN gleich bleiben, sonst verliert Simulink die
  Port-/Parameter-Zuordnung (z.B. `safety` ist ein Parameter, kein Inport).
- **Generierter `mcu.cpp` ist BYTE-IDENTISCH** zur Inline-Variante (einziger Diff:
  „Model version" + Zeitstempel im Header-Kommentar) ⇒ Wrapper ist transparent.
- **`persistent`-State überlebt** (`mcu_DW.latched` 9×, `ack_prev` 2× im Code).
- Coder findet die externe Funktion über den Projekt-/`addpath`-Pfad.

**✅ AUSGEROLLT auf ALLE Blöcke (Session 9): 11 Wrapper, 0 inline.**
| Modell | Block | Funktion |
|---|---|---|
| `mcu` | MATLAB Function / 1 | `safety_overspeed`, `safety_battery` |
| `mcu` | Mahony filter, geo attitude ctrl, hard descent | `mahony_filter`, `geo_attitude_ctrl`, `safety_landcmd` |
| `link` | MATLAB Function2 / 3 | `link_tx`, `link_rx` |
| `gcu` | MATLAB Function, Positionsregler, soft/hard landing | **`traj_gen`\***, **`pos_ctrl`\***, `gcs_supervisor` |
| `quadcop` | MATLAB Function1 | `pack_gcs_frame` |

\* **neu extrahiert** nach `scripts\functions\` (existierten vorher NUR inline).
Korrektur zum ersten Audit: es waren nur **2** Funktionen ohne `.m`, nicht 4 —
`safety_landcmd`/`gcs_supervisor` hatten längst eine SSOT, der erste Parser
scheiterte nur an ihren mehrzeiligen `...`-Signaturen.
**Verifiziert:** alle 4 berührten Modelle kompilieren; Golden **unverändert**
(`imu_gyro` mean identisch `[-0.00095, -0.00184, -0.00177]`); Host+ARM neu,
je `0.94666`/`0.1745` = 0; **Gate B 30/30**; alle 5 Sketches kompilieren
(`drone_hal` FLASH 54916).
Ab jetzt gilt: **`scripts\functions\*.m` ist die SSOT** — Blöcke nie wieder inline
editieren (der Wrapper ruft nur auf). Damit ist Modell-Code endlich git-diffbar.

#### ⚠️ FALLE: `openProject` überschreibt `mcu.slx`
`run_mcu_recert.m` / `run_mcu_arm_codegen.m` rufen `openProject`. Dessen
„unsaved changes"-Tracking (`LoadedFileViewer`) hat beim Exit eine **alte**
In-Memory-Version über die frisch gespeicherte `mcu.slx` geschrieben (Modell
war danach inkonsistent: alter 5-Arg-Blockcode, aber `F_des`-Linie schon weg →
kompilierte nicht). **Headless ohne `openProject` arbeiten:** `params.m` via
`evalin('base', "run('…params.m')")` (in einer Funktion landet `run` sonst im
lokalen Scope → Codegen findet `Ts_inner`/`controller.kR` nicht), dann
`load_system('mcu')`, generieren, `save_system('mcu','','OverwriteIfChangedOnDisk',true)`.
Nach jedem Lauf **Blockcode + Inport-Zahl gegenprüfen** (frisch laden!).

### 3g. Danach
HIL, dann Schwarm (kein onboard-EKF — aus Roadmap gestrichen, Teil 5).

---

## 4. Für den neuen Chat — Dateien hochladen

**Immer:** `Handover_Drohnenschwarm_Sim_7.md` (dieses), `params.m`, `SITL_Runbook.md`.

**Für Codec-Cross-Check + SITL (§3a/3b/3c):**
`pack_quat_sm3.m`, `unpack_quat_sm3.m`, `quat_ota_roundtrip.m`, `link_tx.m`,
`link_rx.m`, `init_link.m`, `mcu_packet.hpp`, `mcu_io.hpp`, `log_mcu_golden.m`,
`setup_buses.m`, generiertes `mcu.h` (+ `mcu_types.h`, falls möglich).

**Für HAL (§3e):**
`drone_hal.cpp`, `mcu.h`, `mcu_packet.hpp`, `Drohne_Teensy.pdf` (PCB),
`geo_attitude_ctrl.m`, `safety_overspeed.m`.

**Für ARM-Config (§3f):**
`configure_mcu_codegen.m`, (`mcu.slx` falls direkt gearbeitet wird).

---

## 5. Arbeitsstil
Entscheidungen vor dem Code festzurren, Inkonsistenzen explizit flaggen,
numerisch verifizieren, knappe präzise Darstellung.
