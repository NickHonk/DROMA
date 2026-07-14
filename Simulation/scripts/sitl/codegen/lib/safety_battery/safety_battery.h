/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: safety_battery.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 09:29:05
 */

#ifndef SAFETY_BATTERY_H
#define SAFETY_BATTERY_H

/* Include Files */
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
extern void safety_battery(double batt_count, unsigned char *led,
                           boolean_T *batt_land, double *V_filt);

void safety_battery_init(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for safety_battery.h
 *
 * [EOF]
 */
