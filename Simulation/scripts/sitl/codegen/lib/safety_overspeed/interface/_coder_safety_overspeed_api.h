/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_safety_overspeed_api.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 11:44:22
 */

#ifndef _CODER_SAFETY_OVERSPEED_API_H
#define _CODER_SAFETY_OVERSPEED_API_H

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
void safety_overspeed(real_T gyro_corr[3], uint8_T estop, boolean_T ack,
                      boolean_T *kill, uint8_T *fault_src, real_T dbg[3]);

void safety_overspeed_api(const mxArray *const prhs[4], int32_T nlhs,
                          const mxArray *plhs[3]);

void safety_overspeed_atexit(void);

void safety_overspeed_initialize(void);

void safety_overspeed_terminate(void);

void safety_overspeed_xil_shutdown(void);

void safety_overspeed_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_safety_overspeed_api.h
 *
 * [EOF]
 */
