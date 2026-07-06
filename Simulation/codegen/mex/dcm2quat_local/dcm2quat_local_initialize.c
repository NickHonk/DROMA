/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * dcm2quat_local_initialize.c
 *
 * Code generation for function 'dcm2quat_local_initialize'
 *
 */

/* Include files */
#include "dcm2quat_local_initialize.h"
#include "_coder_dcm2quat_local_mex.h"
#include "dcm2quat_local_data.h"
#include "rt_nonfinite.h"

/* Function Declarations */
static void dcm2quat_local_once(void);

/* Function Definitions */
static void dcm2quat_local_once(void)
{
  mex_InitInfAndNan();
}

void dcm2quat_local_initialize(void)
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
    dcm2quat_local_once();
  }
}

/* End of code generation (dcm2quat_local_initialize.c) */
