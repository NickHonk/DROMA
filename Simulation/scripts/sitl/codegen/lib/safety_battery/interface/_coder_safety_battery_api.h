/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_safety_battery_api.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 09:29:05
 */

#ifndef _CODER_SAFETY_BATTERY_API_H
#define _CODER_SAFETY_BATTERY_API_H

/* Include Files */
#include "emlrt.h"
#include "mex.h"
#include "tmwtypes.h"
#include <string.h>

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void safety_battery(real_T batt_count, uint8_T *led, boolean_T *batt_land,
                    real_T *V_filt);

void safety_battery_api(const mxArray *const prhs[2], int32_T nlhs,
                        const mxArray *plhs[3]);

void safety_battery_atexit(void);

void safety_battery_initialize(void);

void safety_battery_terminate(void);

void safety_battery_xil_shutdown(void);

void safety_battery_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_safety_battery_api.h
 *
 * [EOF]
 */
