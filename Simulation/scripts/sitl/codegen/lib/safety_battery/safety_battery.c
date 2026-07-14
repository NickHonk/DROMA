/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: safety_battery.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 11:44:24
 */

/* Include Files */
#include "safety_battery.h"
#include "safety_battery_data.h"
#include "safety_battery_initialize.h"

/* Variable Definitions */
static boolean_T Vf_not_empty;

static unsigned char state;

static boolean_T landed;

/* Function Definitions */
/*
 * safety_battery Onboard-Batterie-Monitor, 4S LiPo via PM06 V2.
 *
 *  Kette: ADC-count --> V_batt --> V_filt --> led
 *         V_filt <= V_floor --(Latch bis Reboot) --> batt_land
 *
 *    WARN/CRIT -> LED. Bediener im Raum sieht LED, loest manuell softe Landung
 *                (estop=1 => Uplink) aus, solange Marge da ist.
 *    FLOOR     -> batt_land = true -> onboard blinde Landung
 *                (safety_landcmd.m) als Backstop, falls niemand reagiert.
 *  KILL (Overspeed/Hard-Kill) dominiert: rotors_cmd=0 nachgelagert gewinnt
 * immer
 *
 *  LATCH IST PERMANENT —> KEIN Re-Arm. Zwei Gruende:
 *    1) Akkuwechsel = Teensy-Reboot -> persistent wird genullt.
 *    2) WICHTIGER: Im Descent faellt der Schub auf 0.98*m*g -> weniger Strom ->
 *       V erholt sich UEBER den Floor. Ohne Latch wuerde batt_land wieder auf
 * null gesetzt, das GCS-Kommando (Hover @ m*g) kaeme zurueck, Last steigt, V
 * sackt wieder
 *       -> GRENZZYKLUS Sinken<->Schweben auf fast leerem Akku. Der Latch
 * verhindert das: einmal committed -> bis zum Boden sinken.
 *
 *  Eingaenge:
 *    batt_count : ADC-Rohwert (12 bit, 0..4095). In Sim aus simulierter V-Rampe
 *                 (count = round((V_batt - b)/k)); auf HW
 * analogRead(A17/Pin41). safety     : struct  .batt_k .batt_b .batt_alpha
 * .V_warn .V_crit .V_floor .V_hyst Ausgaenge: led        : uint8  0 NORMAL / 1
 * WARN / 2 CRIT   (-> GPIO-Blinkmuster) batt_land  : bool   latched ->
 * safety_landcmd Hard-Floor-Override V_filt     : double gefilterte
 * Batteriespannung [V]  (dbg/logging)
 *
 *  Hysterese: Recovery (Richtung NORMAL) braucht + V_hyst -> kein Flattern bei
 *  Last-Sag/Rauschen. (tau ~0.5..1 s) glaettet Sag + Rauschen UND verhindert,
 *  dass ein kurzer Spannungseinbruch unter Last den Floor faelschlich triggert.
 *
 * Arguments    : double batt_count
 *                unsigned char *led
 *                boolean_T *batt_land
 *                double *V_filt
 * Return Type  : void
 */
void safety_battery(double batt_count, unsigned char *led, boolean_T *batt_land,
                    double *V_filt)
{
  static double Vf;
  if (!isInitialized_safety_battery) {
    safety_battery_initialize();
  }
  if (!Vf_not_empty) {
    Vf = 0.014652161172161169 * batt_count;
    Vf_not_empty = true;
    /*  init */
  }
  /*  --- ADC -> Spannung --- */
  /*  --- Tiefpass: V_filt += alpha*(V_raw - V_filt) --- */
  Vf += 0.014 * (0.014652161172161169 * batt_count - Vf);
  /*  --- 3-stufige LED mit Hysterese (Batterienzeige) --- */
  /*  NORMAL(0) -> WARN(1) -> CRIT(2); Rueckweg braucht + V_hyst. */
  switch (state) {
  case 0U:
    /*  NORMAL */
    if (Vf <= 14.0) {
      state = 1U;
    }
    break;
  case 1U:
    /*  WARN */
    if (Vf <= 13.4) {
      state = 2U;
    } else if (Vf >= 14.2) {
      state = 0U;
    }
    break;
  default:
    /*  CRIT (2) */
    if (Vf >= 13.6) {
      state = 1U;
    }
    break;
  }
  /*  --- harte Landung --- */
  landed = ((Vf <= 12.0) || landed);
  *led = state;
  *batt_land = landed;
  *V_filt = Vf;
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void safety_battery_init(void)
{
  Vf_not_empty = false;
  state = 0U;
  landed = false;
}

/*
 * File trailer for safety_battery.c
 *
 * [EOF]
 */
