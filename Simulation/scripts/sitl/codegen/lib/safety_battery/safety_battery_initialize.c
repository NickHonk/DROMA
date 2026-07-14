/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: safety_battery_initialize.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 11:44:24
 */

/* Include Files */
#include "safety_battery_initialize.h"
#include "safety_battery.h"
#include "safety_battery_data.h"

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
void safety_battery_initialize(void)
{
  safety_battery_init();
  isInitialized_safety_battery = true;
}

/*
 * File trailer for safety_battery_initialize.c
 *
 * [EOF]
 */
