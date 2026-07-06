/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * quatRotate.h
 *
 * Code generation for function 'quatRotate'
 *
 */

#pragma once

/* Include files */
#include "rtwtypes.h"
#include "emlrt.h"
#include "mex.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function Declarations */
void quatRotate(const emlrtStack *sp, const real_T q[4], const real_T vn[3],
                real_T vb[3]);

/* End of code generation (quatRotate.h) */
