/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: safety_overspeed_initialize.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 07-Jul-2026 13:50:20
 */

/* Include Files */
#include "safety_overspeed_initialize.h"
#include "safety_overspeed.h"
#include "safety_overspeed_data.h"

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
void safety_overspeed_initialize(void)
{
  safety_overspeed_init();
  isInitialized_safety_overspeed = true;
}

/*
 * File trailer for safety_overspeed_initialize.c
 *
 * [EOF]
 */
