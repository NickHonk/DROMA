/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: quatConj.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 14-Jul-2026 09:28:51
 */

/* Include Files */
#include "quatConj.h"

/* Function Definitions */
/*
 * Arguments    : const double a[4]
 *                double r[4]
 * Return Type  : void
 */
void quatConj(const double a[4], double r[4])
{
  r[0] = a[0];
  r[1] = -a[1];
  r[2] = -a[2];
  r[3] = -a[3];
}

/*
 * File trailer for quatConj.c
 *
 * [EOF]
 */
