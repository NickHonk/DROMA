/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * _coder_dcm2quat_local_mex.c
 *
 * Code generation for function '_coder_dcm2quat_local_mex'
 *
 */

/* Include files */
#include "_coder_dcm2quat_local_mex.h"
#include "_coder_dcm2quat_local_api.h"
#include "dcm2quat_local_data.h"
#include "dcm2quat_local_initialize.h"
#include "dcm2quat_local_terminate.h"
#include "rt_nonfinite.h"

/* Function Definitions */
void dcm2quat_local_mexFunction(int32_T nlhs, mxArray *plhs[1], int32_T nrhs,
                                const mxArray *prhs[1])
{
  emlrtStack st = {
      NULL, /* site */
      NULL, /* tls */
      NULL  /* prev */
  };
  const mxArray *outputs;
  st.tls = emlrtRootTLSGlobal;
  /* Check for proper number of arguments. */
  if (nrhs != 1) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:WrongNumberOfInputs", 5, 12, 1, 4,
                        14, "dcm2quat_local");
  }
  if (nlhs > 1) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:TooManyOutputArguments", 3, 4, 14,
                        "dcm2quat_local");
  }
  /* Call the function. */
  dcm2quat_local_api(prhs[0], &outputs);
  /* Copy over outputs to the caller. */
  emlrtReturnArrays(1, &plhs[0], &outputs);
}

void mexFunction(int32_T nlhs, mxArray *plhs[], int32_T nrhs,
                 const mxArray *prhs[])
{
  mexAtExit(&dcm2quat_local_atexit);
  dcm2quat_local_initialize();
  dcm2quat_local_mexFunction(nlhs, plhs, nrhs, prhs);
  dcm2quat_local_terminate();
}

emlrtCTX mexFunctionCreateRootTLS(void)
{
  emlrtCreateRootTLSR2022a(&emlrtRootTLSGlobal, &emlrtContextGlobal, NULL, 1,
                           NULL, "windows-1252", true);
  return emlrtRootTLSGlobal;
}

/* End of code generation (_coder_dcm2quat_local_mex.c) */
