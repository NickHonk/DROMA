/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * dcm2quat_local.h
 *
 * Code generation for function 'dcm2quat_local'
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
void dcm2quat_local(const emlrtStack *sp, const real_T R[9], real_T q[4]);

/* End of code generation (dcm2quat_local.h) */
