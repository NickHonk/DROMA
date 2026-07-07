/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: safety_overspeed.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 07-Jul-2026 13:50:20
 */

#ifndef SAFETY_OVERSPEED_H
#define SAFETY_OVERSPEED_H

/* Include Files */
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
extern void safety_overspeed(const double gyro_corr[3], unsigned char estop,
                             boolean_T ack, boolean_T *kill,
                             unsigned char *fault_src, double dbg[3]);

void safety_overspeed_init(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for safety_overspeed.h
 *
 * [EOF]
 */
