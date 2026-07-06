/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * quat2dcm_local.c
 *
 * Code generation for function 'quat2dcm_local'
 *
 */

/* Include files */
#include "quat2dcm_local.h"
#include "rt_nonfinite.h"

/* Function Definitions */
void quat2dcm_local(const emlrtStack *sp, const real_T q[4], real_T R[9])
{
  real_T R_tmp;
  real_T b_R_tmp;
  real_T c_R_tmp;
  real_T d_R_tmp;
  real_T e_R_tmp;
  real_T f_R_tmp;
  real_T g_R_tmp;
  real_T h_R_tmp;
  (void)sp;
  R_tmp = q[0] * q[0];
  b_R_tmp = q[1] * q[1];
  c_R_tmp = q[2] * q[2];
  d_R_tmp = q[3] * q[3];
  R[0] = ((R_tmp + b_R_tmp) - c_R_tmp) - d_R_tmp;
  e_R_tmp = q[1] * q[2];
  f_R_tmp = q[0] * q[3];
  R[3] = 2.0 * (e_R_tmp + f_R_tmp);
  g_R_tmp = q[1] * q[3];
  h_R_tmp = q[0] * q[2];
  R[6] = 2.0 * (g_R_tmp - h_R_tmp);
  R[1] = 2.0 * (e_R_tmp - f_R_tmp);
  R_tmp -= b_R_tmp;
  R[4] = (R_tmp + c_R_tmp) - d_R_tmp;
  b_R_tmp = q[2] * q[3];
  e_R_tmp = q[0] * q[1];
  R[7] = 2.0 * (b_R_tmp + e_R_tmp);
  R[2] = 2.0 * (g_R_tmp + h_R_tmp);
  R[5] = 2.0 * (b_R_tmp - e_R_tmp);
  R[8] = (R_tmp - c_R_tmp) + d_R_tmp;
}

/* End of code generation (quat2dcm_local.c) */
