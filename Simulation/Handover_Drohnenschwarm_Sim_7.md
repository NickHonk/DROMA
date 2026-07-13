# Handover Drohnenschwarm-Sim — Teil 7

*Fortsetzung von Teil 6. §-Referenzen ohne Zusatz beziehen sich auf Teil 6/5.
Diese Session: **Step 4 — Hardware-Deployment Teensy vorbereitet.** OTA-Paket
entworfen + verifiziert, smallest-three-Codec (MATLAB + C++) gebaut und
bitgleich geprüft, `link_tx/rx` + `init_link` refactored, HAL-Skelett
(Teensy 4.1) geschrieben. Alle Codegen-/HAL-Entscheidungen gelockt.
Nächster Block: Codec-Cross-Check, SITL-Re-Zert für `throttle`, ARM-Codegen.*

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
- **Gyro-Bias:** 3 s Startup-Mittelung (Drohne still), abziehen. Mahony-Kᵢ ist
  **deaktiviert** → das ist die einzige Drift-Absicherung.
- **Hebelarm** `r=[-0.014;-0.015;0.045]` m: **Option 1 gewählt — keine Kompensation**
  (weder Sim noch HAL noch `mcu.slx`). `sensors.slx` modelliert ihn; HW reproduziert
  ihn physikalisch → Sim=HW automatisch. Acc **hebelarm-roh** durch die HAL.
  (Falls je aggressiver geflogen wird: Option 2 = Zentripetal-Kompensation
  `f−ω×(ω×r)` aus `imu_gyro` **innerhalb `mcu.slx`**, dann Re-Cert.)
- **Batterie:** `batt_count = (double)analogRead(41)` (A17=SPANNUNG), **12 bit**,
  **rohe counts** (Volt-Umrechnung macht Modell-Subsystem S6). Strom (Pin40/A16)
  = nur Telemetrie, **nicht** ins Modell. Umrouting 34/35→40/41 war nötig (34/35
  haben keinen ADC).
- **ESC = OneShot125.** `analogWriteFrequency(pin,1000)` + `analogWriteResolution(12)`
  → count 512..1024 = 125..250 µs. Mapping **`count = 512 + throttle*5.12`**
  (`throttle` bereits [0,100] geclampt). Pins/Richtung: **M1=33 CCW, M2=2 CW,
  M3=4 CCW, M4=3 CW** (gegen Mixer-Nummerierung geprüft). Gegenlauf via HW
  (Phasen/Props), **nicht** übers Signal.
- **ESC-Einlernen + Arming:** Startup-FSM am Boden (max→min→arm), Endpunkte
  = Flug-Endpunkte 512/1024.
- **Failsafe:** kein gültiges Paket seit **100 ms** → `estop=2` (Hard-Kill,
  `safety_overspeed` latcht rotors=0; Re-Arm nur am Boden via ack-Flanke).
  Soft-Land (estop=1) ist GCS-getrieben → bei Link-Verlust unmöglich, daher
  estop=2 die einzige kohärente Onboard-Aktion.
- **Overspeed:** `safety.omega_max=8.5 rad/s` < Gyro-FSR 8.727 rad/s → detektierbar
  (Sättigung 8.727 > 8.5). Margin dünn, aber Entprellung robust.

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
| `run_gate_a.m` | Gate A headless-Wrapper (SIL interaktiv fahren) | `scripts\sitl\matlab\` |
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
| `drone_hal.cpp` `HAL_SELFTEST` | Bench-Selbsttest: Motoren min, I/O-Report ~10 Hz | `hardware\` |

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
Poly-Dump+Golden), `run_gate_a.m` (Gate A).

**Gate-Status:**
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
- **Batt-Pin:** 41 = Spannung (A17) → `batt_count`, 40 = Strom (Telemetrie).
  `init_battery_manag.batt_pin` 40→41 korrigiert (war stale, im Codegen ungenutzt).
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
  1-kHz-Tick mit >50 % Reserve. Der `HAL_SELFTEST`-Report druckt es live.
- **Gyro (ok):** Bias 3 s abgezogen, ~0 still, reagiert auf Bewegung.
- **Acc (kleiner Offset):** z ≈ +9.2 (z-up korrekt), aber |a| ≈ 9.27 (~5 % niedrig)
  + y-Offset ~−1.0 → leichte Schräglage/Accel-Offset. Für Bench ok; Accel-Kalib
  (Bias/Scale) bei Bedarf separat.
- **Batt (noch nicht prüfbar):** `batt=0(0.00V)` — nur USB, kein Flug-Akku am PM06.
  Erst mit angestecktem 4S-Akku verifizierbar.
- **⚠️⚠️ FAILSAFE-BUG (vor bestromtem Flug fixen!):** `estop==2` **setzt** `latched`
  (mcu.cpp Z. 226), und `latched → rotor_cmd=0` (Z. 662). ABER der in §3b neue
  `throttle`-Outport (`clamp(polyval(P, omega_sq))`, Z. 683–741) ist **NICHT** vom
  `latched`-Gate erfasst. Da der **HAL die ESCs aus `throttle`** treibt (nicht
  `rotor_cmd`), **stoppt der Failsafe die Motoren NICHT** (throttle bleibt 0–16 %,
  reagiert auf Lage). Bench-verifiziert.
  **Fix:** in `mcu.slx` `throttle` durch das `latched`-Gate → `throttle = latched ?
  0 : clamp(polyval(...))` (Switch am Ausgang, auf **0** schalten — NICHT Polynom
  mit 0 speisen, `polyval(P,0)=8.4 %`). Danach §3b-Re-Zert (Golden neu, Gate A+B;
  test_mcu_model muss dann throttle==0 bei latched prüfen).
- **Accel-Nutzung:** `imu_acc` speist die **Onboard-Attitude-Schätzung** (Mahony-
  artig, `mcu_DW.q`, fusioniert mit `q_ext`/Mocap); der Regler nutzt `q`, nicht
  rohen Accel. → Schiefmontage wirkt über die Schätzung, aber gedämpft durch
  `q_ext`. Fix ist eine **Alignment-Rotation** (oder physisch level montieren),
  KEINE Standard-6-Lagen-Accel-Kalib (die korrigiert Sensor-Bias/Scale, nicht
  Montage). Der ~5 %-Betragsfehler ist separat (Scale, minor).

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
