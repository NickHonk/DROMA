/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: safety_overspeed.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 09:29:01
 */

/* Include Files */
#include "safety_overspeed.h"
#include "safety_overspeed_data.h"
#include "safety_overspeed_initialize.h"
#include <math.h>

/* Variable Definitions */
static boolean_T latched;

static unsigned short cnt;

static boolean_T ack_prev;

static unsigned char src;

/* Function Definitions */
/*
 * SAFETY_OVERSPEED  Onboard-KILL-Latch
 *
 *  Overspeed-Entprell-Latch ∪ Hard-Kill (estop==2).
 *  Aktion downstream: rotors_cmd = 0  (Override NACH Mixer / VOR
 * Motor-PT1+ESC). KILL dominiert LAND; KILL latcht; Re-Arm NIE in der Luft.
 *
 *  ARMING-IDLE-INTERLOCK: Re-Arm wirkt zusaetzlich NUR, wenn der befohlene
 * Schub F_des <= safety.F_rearm_idle ("Schub runter zum Armen"). Verhindert
 * einen Sprung-auf-Hover beim Latch-Loesen (z.B. Re-Arm waehrend die GCS Hover
 * sendet).
 *
 *  Warum eine Funktion fuer beide KILL-Quellen: Overspeed und Hard-Kill teilen
 *  denselben Latch, dieselbe Aktion (rotors_cmd=0) und dieselbe Re-Arm-Semantik
 *  (ack loescht). Eine gemeinsame Funktion haelt die Re-Arm-Bedingung an EINER
 *  Stelle und macht "KILL dominiert LAND" trivial. Der geregelte Soft-Land
 *  (estop==1) wird hier NICHT behandelt — das ist die GCS-Mode-Maschine.
 *
 *  Eingaenge
 *    gyro_corr : 3x1  bias-korrigierte Drehrate [rad/s]  (MESSUNG, nicht
 * Schaetzer!) estop : uint8  0 normal / 1 soft-land / 2 hard-kill (aus Bus_Cmd,
 * Uplink) ack : bool   Quittung, bereits ge-OR-t (Teensy-Taster-Flanke |
 * Bus_Cmd.ack) F_des : double  befohlener Gesamtschub [N] (aus Bus_Cmd) fuer
 * den Idle-Interlock safety : struct  .omega_max [rad/s], .debounce_N (>=1),
 * .use_norm (bool), .F_rearm_idle [N] (Idle-Schwelle fuer Re-Arm)
 *
 *  Ausgaenge
 *    kill : bool   latched -> nachgelagerter Switch zwingt rotors_cmd=0
 *    fault_src : uint8  0 none / 1 overspeed / 2 hard-kill   (LED/Debug)
 *    dbg : 3x1    [cnt; over_inst; ack_edge]           (verify/logging)
 *
 *  Re-Arm (FAULT->ARMED) NUR bei:  steigende ack-Flanke  &  ~over_inst  &
 * estop~=2. Die ack-FLANKE (nicht der Pegel) verhindert, dass ein gehaltenes
 * ack einen frischen Trip sofort wieder loescht oder mid-air in einen Sturz
 * re-armt. Die Boden-Bedingung ("nie in der Luft") wird prozedural durch den
 * physischen Teensy-Taster garantiert (Bediener hebt die Drohne auf, drueckt) —
 * onboard existiert keine Pos/Vel-Schaetzung fuer ein Logik-Interlock (sitzt
 * GCS-seitig). Optionaler Idle-Interlock siehe Hinweis am Dateiende.
 *
 * Arguments    : const double gyro_corr[3]
 *                unsigned char estop
 *                boolean_T ack
 *                double F_des
 *                boolean_T *kill
 *                unsigned char *fault_src
 *                double dbg[3]
 * Return Type  : void
 */
void safety_overspeed(const double gyro_corr[3], unsigned char estop,
                      boolean_T ack, double F_des, boolean_T *kill,
                      unsigned char *fault_src, double dbg[3])
{
  boolean_T ack_edge;
  boolean_T over_inst;
  if (!isInitialized_safety_overspeed) {
    safety_overspeed_initialize();
  }
  /*  --- Overspeed-Detektor, entprellt (N aufeinanderfolgende Samples) --- */
  if ((fabs(gyro_corr[0]) > 10.0) || (fabs(gyro_corr[1]) > 10.0) ||
      (fabs(gyro_corr[2]) > 10.0)) {
    over_inst = true;
    if (cnt < 4) {
      cnt++;
    }
  } else {
    over_inst = false;
    cnt = 0U;
    /*  ein gutes Sample setzt den Zaehler zurueck */
  }
  /*  --- Hard-Kill: sofort, keine Entprellung --- */
  /*  --- KILL setzen (latcht; Quelle nur beim ersten Setzen vermerkt) --- */
  if ((cnt >= 4) && (!latched)) {
    latched = true;
    src = 1U;
    /*  due to fast turning rates from gyro */
  }
  if ((estop == 2) && (!latched)) {
    latched = true;
    src = 2U;
  }
  /*  Re-Arm: steigende ack-Flanke + kein Overspeed + kein Hard-Kill + Schub im
   * Idle. */
  /*  Der Idle-Interlock (F_des <= F_rearm_idle) erzwingt "Schub runter zum
   * Armen" und */
  /*  verhindert damit einen Sprung-auf-Hover in dem Tick, in dem der Latch
   * loest. */
  if (ack && (!ack_prev)) {
    ack_edge = true;
  } else {
    ack_edge = false;
  }
  if (latched && ack_edge && (!over_inst) && (estop != 2) && (F_des <= 1.0)) {
    latched = false;
    cnt = 0U;
    src = 0U;
  }
  ack_prev = ack;
  *kill = latched;
  *fault_src = src;
  dbg[0] = cnt;
  dbg[1] = over_inst;
  dbg[2] = ack_edge;
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void safety_overspeed_init(void)
{
  latched = false;
  cnt = 0U;
  ack_prev = false;
  src = 0U;
}

/*
 * File trailer for safety_overspeed.c
 *
 * [EOF]
 */
