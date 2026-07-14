/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_quat2dcm_local_api.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 11:44:14
 */

#ifndef _CODER_QUAT2DCM_LOCAL_API_H
#define _CODER_QUAT2DCM_LOCAL_API_H

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
void quat2dcm_local(real_T q[4], real_T R[9]);

void quat2dcm_local_api(const mxArray *prhs, const mxArray **plhs);

void quat2dcm_local_atexit(void);

void quat2dcm_local_initialize(void);

void quat2dcm_local_terminate(void);

void quat2dcm_local_xil_shutdown(void);

void quat2dcm_local_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_quat2dcm_local_api.h
 *
 * [EOF]
 */
