/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: quat2dcm_local.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 09:28:42
 */

/* Include Files */
#include "quat2dcm_local.h"

/* Function Definitions */
/*
 * Arguments    : const double q[4]
 *                double R[9]
 * Return Type  : void
 */
void quat2dcm_local(const double q[4], double R[9])
{
  double R_tmp;
  double b_R_tmp;
  double c_R_tmp;
  double d_R_tmp;
  double e_R_tmp;
  double f_R_tmp;
  double g_R_tmp;
  double h_R_tmp;
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

/*
 * File trailer for quat2dcm_local.c
 *
 * [EOF]
 */
