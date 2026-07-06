/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * quatRotate_initialize.c
 *
 * Code generation for function 'quatRotate_initialize'
 *
 */

/* Include files */
#include "quatRotate_initialize.h"
#include "_coder_quatRotate_mex.h"
#include "quatRotate_data.h"
#include "rt_nonfinite.h"

/* Function Declarations */
static void quatRotate_once(void);

/* Function Definitions */
static void quatRotate_once(void)
{
  mex_InitInfAndNan();
}

void quatRotate_initialize(void)
{
  static const volatile char_T *emlrtBreakCheckR2012bFlagVar = NULL;
  emlrtStack st = {
      NULL, /* site */
      NULL, /* tls */
      NULL  /* prev */
  };
  mexFunctionCreateRootTLS();
  st.tls = emlrtRootTLSGlobal;
  emlrtBreakCheckR2012bFlagVar = emlrtGetBreakCheckFlagAddressR2022b(&st);
  emlrtClearAllocCountR2012b(&st, false, 0U, NULL);
  emlrtEnterRtStackR2012b(&st);
  if (emlrtFirstTimeR2012b(emlrtRootTLSGlobal)) {
    quatRotate_once();
  }
}

/* End of code generation (quatRotate_initialize.c) */
