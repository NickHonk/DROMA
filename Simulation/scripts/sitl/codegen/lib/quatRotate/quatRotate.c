/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: quatRotate.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 09:28:55
 */

/* Include Files */
#include "quatRotate.h"
#include <emmintrin.h>
#include <string.h>

/* Function Definitions */
/*
 * vb = R(q) * vn
 *
 * Arguments    : const double q[4]
 *                const double vn[3]
 *                double vb[3]
 * Return Type  : void
 */
void quatRotate(const double q[4], const double vn[3], double vb[3])
{
  __m128d r;
  __m128d r1;
  double b_q[9];
  double b_q_tmp;
  double c_q_tmp;
  double d_q_tmp;
  double e_q_tmp;
  double f_q_tmp;
  double g_q_tmp;
  double h_q_tmp;
  double q_tmp;
  q_tmp = q[0] * q[0];
  b_q_tmp = q[1] * q[1];
  c_q_tmp = q[2] * q[2];
  d_q_tmp = q[3] * q[3];
  b_q[0] = ((q_tmp + b_q_tmp) - c_q_tmp) - d_q_tmp;
  e_q_tmp = q[1] * q[2];
  f_q_tmp = q[0] * q[3];
  b_q[3] = 2.0 * (e_q_tmp + f_q_tmp);
  g_q_tmp = q[1] * q[3];
  h_q_tmp = q[0] * q[2];
  b_q[6] = 2.0 * (g_q_tmp - h_q_tmp);
  b_q[1] = 2.0 * (e_q_tmp - f_q_tmp);
  q_tmp -= b_q_tmp;
  b_q[4] = (q_tmp + c_q_tmp) - d_q_tmp;
  b_q_tmp = q[2] * q[3];
  e_q_tmp = q[0] * q[1];
  b_q[7] = 2.0 * (b_q_tmp + e_q_tmp);
  b_q[2] = 2.0 * (g_q_tmp + h_q_tmp);
  b_q[5] = 2.0 * (b_q_tmp - e_q_tmp);
  b_q[8] = (q_tmp - c_q_tmp) + d_q_tmp;
  memset(&vb[0], 0, 3U * sizeof(double));
  r = _mm_loadu_pd(&b_q[0]);
  r1 = _mm_loadu_pd(&vb[0]);
  _mm_storeu_pd(&vb[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(vn[0]))));
  vb[2] += vn[0] * b_q[2];
  r = _mm_loadu_pd(&b_q[3]);
  r1 = _mm_loadu_pd(&vb[0]);
  _mm_storeu_pd(&vb[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(vn[1]))));
  vb[2] += vn[1] * b_q[5];
  r = _mm_loadu_pd(&b_q[6]);
  r1 = _mm_loadu_pd(&vb[0]);
  _mm_storeu_pd(&vb[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(vn[2]))));
  vb[2] += vn[2] * b_q[8];
}

/*
 * File trailer for quatRotate.c
 *
 * [EOF]
 */
