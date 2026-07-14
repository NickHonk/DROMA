/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: quatMul.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 09:28:47
 */

/* Include Files */
#include "quatMul.h"

/* Function Definitions */
/*
 * Hamilton-Produkt:  r = a (x) c
 *
 * Arguments    : const double a[4]
 *                const double c[4]
 *                double r[4]
 * Return Type  : void
 */
void quatMul(const double a[4], const double c[4], double r[4])
{
  r[0] = ((a[0] * c[0] - a[1] * c[1]) - a[2] * c[2]) - a[3] * c[3];
  r[1] = ((a[0] * c[1] + c[0] * a[1]) + a[2] * c[3]) - c[2] * a[3];
  r[2] = ((a[0] * c[2] - a[1] * c[3]) + c[0] * a[2]) + c[1] * a[3];
  r[3] = ((a[0] * c[3] + a[1] * c[2]) - c[1] * a[2]) + c[0] * a[3];
}

/*
 * File trailer for quatMul.c
 *
 * [EOF]
 */
