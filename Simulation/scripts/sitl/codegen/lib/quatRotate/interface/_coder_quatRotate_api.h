/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_quatRotate_api.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 11:44:20
 */

#ifndef _CODER_QUATROTATE_API_H
#define _CODER_QUATROTATE_API_H

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
void quatRotate(real_T q[4], real_T vn[3], real_T vb[3]);

void quatRotate_api(const mxArray *const prhs[2], const mxArray **plhs);

void quatRotate_atexit(void);

void quatRotate_initialize(void);

void quatRotate_terminate(void);

void quatRotate_xil_shutdown(void);

void quatRotate_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_quatRotate_api.h
 *
 * [EOF]
 */
