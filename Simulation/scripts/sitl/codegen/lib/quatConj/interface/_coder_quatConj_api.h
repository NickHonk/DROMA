/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_quatConj_api.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 07-Jul-2026 13:50:11
 */

#ifndef _CODER_QUATCONJ_API_H
#define _CODER_QUATCONJ_API_H

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
void quatConj(real_T a[4], real_T r[4]);

void quatConj_api(const mxArray *prhs, const mxArray **plhs);

void quatConj_atexit(void);

void quatConj_initialize(void);

void quatConj_terminate(void);

void quatConj_xil_shutdown(void);

void quatConj_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_quatConj_api.h
 *
 * [EOF]
 */
