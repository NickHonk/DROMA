/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_quatMul_api.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 07-Jul-2026 13:50:07
 */

#ifndef _CODER_QUATMUL_API_H
#define _CODER_QUATMUL_API_H

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
void quatMul(real_T a[4], real_T c[4], real_T r[4]);

void quatMul_api(const mxArray *const prhs[2], const mxArray **plhs);

void quatMul_atexit(void);

void quatMul_initialize(void);

void quatMul_terminate(void);

void quatMul_xil_shutdown(void);

void quatMul_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_quatMul_api.h
 *
 * [EOF]
 */
