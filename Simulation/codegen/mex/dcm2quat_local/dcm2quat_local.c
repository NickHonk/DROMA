/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * dcm2quat_local.c
 *
 * Code generation for function 'dcm2quat_local'
 *
 */

/* Include files */
#include "dcm2quat_local.h"
#include "rt_nonfinite.h"
#include "mwmathutil.h"
#include <emmintrin.h>

/* Variable Definitions */
static emlrtRSInfo b_emlrtRSI = {
    13,               /* lineNo */
    "dcm2quat_local", /* fcnName */
    "C:\\Users\\Nick\\thesis_doctoral\\MAS "
    "Versuchsaufbau\\Drohnen\\DROMA\\Simulation\\scripts\\functions\\dcm2quat_"
    "local.m" /* pathName */
};

static emlrtRSInfo c_emlrtRSI = {
    19,               /* lineNo */
    "dcm2quat_local", /* fcnName */
    "C:\\Users\\Nick\\thesis_doctoral\\MAS "
    "Versuchsaufbau\\Drohnen\\DROMA\\Simulation\\scripts\\functions\\dcm2quat_"
    "local.m" /* pathName */
};

static emlrtRSInfo d_emlrtRSI = {
    25,               /* lineNo */
    "dcm2quat_local", /* fcnName */
    "C:\\Users\\Nick\\thesis_doctoral\\MAS "
    "Versuchsaufbau\\Drohnen\\DROMA\\Simulation\\scripts\\functions\\dcm2quat_"
    "local.m" /* pathName */
};

static emlrtRTEInfo emlrtRTEI = {
    13,     /* lineNo */
    9,      /* colNo */
    "sqrt", /* fName */
    "C:\\Program "
    "Files\\Matlab\\R2025b\\toolbox\\eml\\lib\\matlab\\elfun\\sqrt.m" /* pName
                                                                       */
};

/* Function Definitions */
void dcm2quat_local(const emlrtStack *sp, const real_T R[9], real_T q[4])
{
  __m128d r;
  __m128d r1;
  emlrtStack st;
  real_T q0;
  real_T q1;
  real_T q2;
  real_T q3;
  real_T scale;
  real_T t;
  real_T tr;
  st.prev = sp;
  st.tls = sp->tls;
  /*  Shepperd-Methode: numerisch robust, auch bei grossen Drehwinkeln.  */
  /*  Rueckgabe q = [q0;q1;q2;q3] (Spalte), normiert. */
  tr = (R[0] + R[4]) + R[8];
  if (tr > 0.0) {
    tr = 2.0 * muDoubleScalarSqrt(tr + 1.0);
    q0 = 0.25 * tr;
    q1 = (R[7] - R[5]) / tr;
    q2 = (R[2] - R[6]) / tr;
    q3 = (R[3] - R[1]) / tr;
  } else if ((R[0] > R[4]) && (R[0] > R[8])) {
    st.site = &b_emlrtRSI;
    tr = ((R[0] + 1.0) - R[4]) - R[8];
    if (tr < 0.0) {
      emlrtErrorWithMessageIdR2018a(
          &st, &emlrtRTEI, "Coder:toolbox:ElFunDomainError",
          "Coder:toolbox:ElFunDomainError", 3, 4, 4, "sqrt");
    }
    tr = muDoubleScalarSqrt(tr);
    tr *= 2.0;
    q0 = (R[7] - R[5]) / tr;
    q1 = 0.25 * tr;
    q2 = (R[1] + R[3]) / tr;
    q3 = (R[2] + R[6]) / tr;
  } else if (R[4] > R[8]) {
    st.site = &c_emlrtRSI;
    tr = ((R[4] + 1.0) - R[0]) - R[8];
    if (tr < 0.0) {
      emlrtErrorWithMessageIdR2018a(
          &st, &emlrtRTEI, "Coder:toolbox:ElFunDomainError",
          "Coder:toolbox:ElFunDomainError", 3, 4, 4, "sqrt");
    }
    tr = muDoubleScalarSqrt(tr);
    tr *= 2.0;
    q0 = (R[2] - R[6]) / tr;
    q1 = (R[1] + R[3]) / tr;
    q2 = 0.25 * tr;
    q3 = (R[5] + R[7]) / tr;
  } else {
    st.site = &d_emlrtRSI;
    tr = ((R[8] + 1.0) - R[0]) - R[4];
    if (tr < 0.0) {
      emlrtErrorWithMessageIdR2018a(
          &st, &emlrtRTEI, "Coder:toolbox:ElFunDomainError",
          "Coder:toolbox:ElFunDomainError", 3, 4, 4, "sqrt");
    }
    tr = muDoubleScalarSqrt(tr);
    tr *= 2.0;
    q0 = (R[3] - R[1]) / tr;
    q1 = (R[2] + R[6]) / tr;
    q2 = (R[5] + R[7]) / tr;
    q3 = 0.25 * tr;
  }
  q[0] = q0;
  q[1] = q1;
  q[2] = q2;
  q[3] = q3;
  scale = 3.3121686421112381E-170;
  tr = muDoubleScalarAbs(q0);
  if (tr > 3.3121686421112381E-170) {
    q0 = 1.0;
    scale = tr;
  } else {
    t = tr / 3.3121686421112381E-170;
    q0 = t * t;
  }
  tr = muDoubleScalarAbs(q1);
  if (tr > scale) {
    t = scale / tr;
    q0 = q0 * t * t + 1.0;
    scale = tr;
  } else {
    t = tr / scale;
    q0 += t * t;
  }
  tr = muDoubleScalarAbs(q2);
  if (tr > scale) {
    t = scale / tr;
    q0 = q0 * t * t + 1.0;
    scale = tr;
  } else {
    t = tr / scale;
    q0 += t * t;
  }
  tr = muDoubleScalarAbs(q3);
  if (tr > scale) {
    t = scale / tr;
    q0 = q0 * t * t + 1.0;
    scale = tr;
  } else {
    t = tr / scale;
    q0 += t * t;
  }
  q0 = scale * muDoubleScalarSqrt(q0);
  r = _mm_loadu_pd(&q[0]);
  r1 = _mm_set1_pd(q0);
  _mm_storeu_pd(&q[0], _mm_div_pd(r, r1));
  r = _mm_loadu_pd(&q[2]);
  _mm_storeu_pd(&q[2], _mm_div_pd(r, r1));
}

/* End of code generation (dcm2quat_local.c) */
