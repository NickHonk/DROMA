/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: dcm2quat_local.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 11:44:05
 */

/* Include Files */
#include "dcm2quat_local.h"
#include "norm.h"
#include <emmintrin.h>
#include <math.h>

/* Function Definitions */
/*
 * Shepperd-Methode: numerisch robust, auch bei grossen Drehwinkeln.
 *  Rueckgabe q = [q0;q1;q2;q3] (Spalte), normiert.
 *
 * Arguments    : const double R[9]
 *                double q[4]
 * Return Type  : void
 */
void dcm2quat_local(const double R[9], double q[4])
{
  __m128d r;
  __m128d r1;
  double q0;
  double q1;
  double q2;
  double tr;
  tr = (R[0] + R[4]) + R[8];
  if (tr > 0.0) {
    tr = 2.0 * sqrt(tr + 1.0);
    q0 = 0.25 * tr;
    q1 = (R[7] - R[5]) / tr;
    q2 = (R[2] - R[6]) / tr;
    tr = (R[3] - R[1]) / tr;
  } else if ((R[0] > R[4]) && (R[0] > R[8])) {
    tr = 2.0 * sqrt(((R[0] + 1.0) - R[4]) - R[8]);
    q0 = (R[7] - R[5]) / tr;
    q1 = 0.25 * tr;
    q2 = (R[1] + R[3]) / tr;
    tr = (R[2] + R[6]) / tr;
  } else if (R[4] > R[8]) {
    tr = 2.0 * sqrt(((R[4] + 1.0) - R[0]) - R[8]);
    q0 = (R[2] - R[6]) / tr;
    q1 = (R[1] + R[3]) / tr;
    q2 = 0.25 * tr;
    tr = (R[5] + R[7]) / tr;
  } else {
    tr = 2.0 * sqrt(((R[8] + 1.0) - R[0]) - R[4]);
    q0 = (R[3] - R[1]) / tr;
    q1 = (R[2] + R[6]) / tr;
    q2 = (R[5] + R[7]) / tr;
    tr *= 0.25;
  }
  q[0] = q0;
  q[1] = q1;
  q[2] = q2;
  q[3] = tr;
  r = _mm_loadu_pd(&q[0]);
  r1 = _mm_set1_pd(b_norm(q));
  _mm_storeu_pd(&q[0], _mm_div_pd(r, r1));
  r = _mm_loadu_pd(&q[2]);
  _mm_storeu_pd(&q[2], _mm_div_pd(r, r1));
}

/*
 * File trailer for dcm2quat_local.c
 *
 * [EOF]
 */
