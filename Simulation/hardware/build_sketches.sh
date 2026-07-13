#!/usr/bin/env bash
# build_sketches.sh — flashbare Arduino-Sketch-Ordner aus den kanonischen Quellen
# assemblieren (SSOT bleibt in hardware/ + scripts/sitl/include/ + hardware/mcu_arm/).
# Ergebnis: hardware/build/{gcs_sender,drone_hal}/  -> in Arduino IDE oeffnen & flashen,
# oder mit --compile / --upload direkt via arduino-cli.
#
#   ./build_sketches.sh                      # nur assemblieren
#   ./build_sketches.sh --compile            # + fuer Teensy 4.1 kompilieren (Verifikation)
#   ./build_sketches.sh --upload-sender COM7 # + Sende-Teensy flashen
#   ./build_sketches.sh --upload-drone  COM8 # + eine Drohne flashen
set -euo pipefail

HW="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"      # .../hardware
INC="$HW/../scripts/sitl/include"
ARM="$HW/mcu_arm/mcu_ert_rtw"
OUT="$HW/build"
FQBN="teensy:avr:teensy41"
CLI="${ARDUINO_CLI:-/c/Program Files/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe}"
CFG="${ARDUINO_CFG:-$HOME/AppData/Local/Arduino15/arduino-cli.yaml}"

echo "== assembliere Sketches nach $OUT =="
# in-place ueberschreiben (KEIN rm -rf) -> kollidiert nicht mit offener Arduino-IDE.
mkdir -p "$OUT/gcs_sender" "$OUT/drone_hal"

# --- Sende-Teensy: Frame-Contract + Codec ---
cp "$HW/gcs_sender.cpp" "$OUT/gcs_sender/gcs_sender.ino"
cp "$INC/gcs_frame.hpp" "$INC/mcu_packet.hpp" "$OUT/gcs_sender/"

# --- Drohne: HAL + Codec + ARM-generierte MCU-Klasse (OHNE ert_main.cpp) ---
cp "$HW/drone_hal.cpp" "$OUT/drone_hal/drone_hal.ino"
cp "$INC/mcu_packet.hpp" "$OUT/drone_hal/"
if [ ! -d "$ARM" ]; then
  echo "FEHLER: $ARM fehlt. Erst run_mcu_arm_codegen.m laufen lassen (§3f)." >&2; exit 1
fi
for f in mcu.cpp mcu_data.cpp mcu.h mcu_types.h mcu_private.h rtwtypes.h; do
  cp "$ARM/$f" "$OUT/drone_hal/"
done

# --- Bench-Sketches (standalone, keine geteilten Header) ---
mkdir -p "$OUT/i2c_scan" "$OUT/esc_calibrate"
cp "$HW/i2c_scan.cpp"      "$OUT/i2c_scan/i2c_scan.ino"
cp "$HW/esc_calibrate.cpp" "$OUT/esc_calibrate/esc_calibrate.ino"

for s in gcs_sender drone_hal i2c_scan esc_calibrate; do echo "  $s/: $(ls "$OUT/$s" | tr '\n' ' ')"; done

# compile [sketch] [extra_flags]
compile() { echo "== compile $1 ${2:+($2)} =="; "$CLI" compile -b "$FQBN" "$OUT/$1" --config-file "$CFG" \
    ${2:+--build-property "compiler.cpp.extra_flags=$2"} \
    2>&1 | grep -iE "error|Memory Usage|FLASH|RAM|Build.*status" | grep -viE "Fehler beim Initialisieren|Download failed"; }
# Teensy-Uploads brauchen den "teensy port" (usb:...), NICHT COMx. Auto-erkennen.
teensy_port() { "$CLI" board list --config-file "$CFG" 2>/dev/null \
    | grep "$FQBN" | awk '{print $1}' | head -1; }
# upload [sketch] [port_hint] [extra_flags] — compile+upload (Define wirkt sicher).
# Nutzt den erkannten teensy-Port; port_hint nur Fallback, falls keiner gefunden wird.
upload()  {
    local tp; tp="$(teensy_port)"; tp="${tp:-$2}"
    if [ -z "$tp" ]; then echo "FEHLER: kein Teensy gefunden (USB angesteckt? Nur 1 Teensy?)." >&2; return 1; fi
    echo "== compile+upload $1 -> $tp ${3:+($3)} =="
    echo "   (Serial-Monitor auf dem Ziel-Teensy vorher SCHLIESSEN, sonst haengt der Loader.)"
    "$CLI" compile --upload -b "$FQBN" -p "$tp" "$OUT/$1" --config-file "$CFG" \
        ${3:+--build-property "compiler.cpp.extra_flags=$3"} \
    2>&1 | grep -iE "error|Memory Usage|upload|verif|bytes|programming|Build.*status" \
         | grep -viE "Fehler beim Initialisieren|Download failed"; }

while [ $# -gt 0 ]; do
  case "$1" in
    --compile)                compile gcs_sender; compile drone_hal; compile drone_hal "-DHAL_SELFTEST";
                              compile i2c_scan;   compile esc_calibrate; shift;;
    --upload-sender)          upload gcs_sender    "$2"; shift 2;;
    --upload-drone)           upload drone_hal     "$2"; shift 2;;
    --upload-drone-selftest)  upload drone_hal     "$2" "-DHAL_SELFTEST"; shift 2;;
    --upload-scan)            upload i2c_scan      "$2"; shift 2;;
    --upload-esccal)          upload esc_calibrate "$2"; shift 2;;
    *) echo "unbekannt: $1" >&2; exit 2;;
  esac
done
echo "== fertig. IDE: $OUT/<name>/<name>.ino oeffnen & flashen. =="
