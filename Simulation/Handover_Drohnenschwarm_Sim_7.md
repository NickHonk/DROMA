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

### 3b. SITL-Re-Zert für `throttle` (Diffs stehen, Ausführung offen)
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

### 3d. GS-Seite (Simulink + Sende-Teensy)
- Simulink: pro Drohne `id` setzen, USB-Float-Frame `[sync|id|Bus_Cmd|checksum]`.
- Sende-Teensy: `pkt::pack` + `seq[0..2]`, `radio.write`. Broadcast-Adr/Kanal/
  Datenrate wählen (GS + 3 Drohnen teilen).

### 3e. HAL — offene HW-Details (im Code als TODO)
- **Status-LEDs:** nur Sammel-LED (Pin 5) + `STATUS_100%` (Pin 10) bekannt. Pins
  für 25/50/75 % fehlen im Schaltplan-Dump. **Und:** `mcu.slx`-`state`-Werte →
  %-Level-Tabelle nötig, um `drive_leds()` zu dekodieren.
- **ESC-Einlern-Timings** (Platzhalter 3 s/3 s) an reale ESCs anpassen.
- **Timing-Budget messen:** MPU-Burst @400 kHz ≈0.4 ms + `step()` + IO in 1 ms?
  (`micros()` um den Tick). Ggf. non-blocking/DMA-I²C.
- **RF24-Version:** unterstützt `begin(&SPI1)`? Sonst `SPI1.setMOSI/MISO/SCK` vor
  `begin()` oder RF24 (TMRh20) updaten.
- **ADO→GND-Bodge** auf dem PCB (Pull-Down R8 bestücken) → 0x68.
- **Batt-ADC-Umrouting** 40/41 verdrahten; prüfen dass 40/41 sonst frei sind.

### 3f. ARM-Codegen-Config (danach)
`configure_mcu_codegen.m`-Variante: `ProdHWDeviceType`→ARM Cortex-M7 (weg von
Intel x86-64/SSE2; `<emmintrin.h>` muss aus dem Output verschwinden), `double`
behalten, Solver DISCRETE fixed-step `Ts_inner`, **SingleTasking**, Klassenname
**`MCU`** gepinnt, kein `-ffast-math`, FPU round-to-nearest. GenCodeOnly (Kompilat
via Teensy-Toolchain, nicht aus MATLAB).

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
