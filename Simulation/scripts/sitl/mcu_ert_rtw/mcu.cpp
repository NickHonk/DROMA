//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: mcu.cpp
//
// Code generated for Simulink model 'mcu'.
//
// Model version                  : 1.245
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Jul  7 16:24:37 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: Intel->x86-64 (Windows64)
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "mcu.h"
#include "rtwtypes.h"
#include <emmintrin.h>
#include <cmath>
#include "mcu_private.h"

static void rate_scheduler(MCU::RT_MODEL_mcu_T *const mcu_M);

//
//         This function updates active task flag for each subrate.
//         The function is called at model base rate, hence the
//         generated code self-manages all its subrates.
//
static void rate_scheduler(MCU::RT_MODEL_mcu_T *const mcu_M)
{
  // Compute which subrates run during the next base time step.  Subrates
  //  are an integer multiple of the base rate counter.  Therefore, the subtask
  //  counter is reset when it reaches its limit (zero means run).

  (mcu_M->Timing.TaskCounters.TID[1])++;
  if ((mcu_M->Timing.TaskCounters.TID[1]) > 999) {// Sample time: [1.0s, 0.0s]
    mcu_M->Timing.TaskCounters.TID[1] = 0;
  }
}

// Function for MATLAB Function: '<S3>/MATLAB Function'
real_T MCU::mcu_norm(const real_T x[3])
{
  real_T absxk;
  real_T scale;
  real_T t;
  real_T y;
  scale = 3.3121686421112381E-170;
  absxk = std::abs(x[0]);
  if (absxk > 3.3121686421112381E-170) {
    y = 1.0;
    scale = absxk;
  } else {
    t = absxk / 3.3121686421112381E-170;
    y = t * t;
  }

  absxk = std::abs(x[1]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  absxk = std::abs(x[2]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  return scale * std::sqrt(y);
}

// Function for MATLAB Function: '<S3>/MATLAB Function'
real_T MCU::mcu_norm_j(const real_T x[4])
{
  real_T absxk;
  real_T scale;
  real_T t;
  real_T y;
  scale = 3.3121686421112381E-170;
  absxk = std::abs(x[0]);
  if (absxk > 3.3121686421112381E-170) {
    y = 1.0;
    scale = absxk;
  } else {
    t = absxk / 3.3121686421112381E-170;
    y = t * t;
  }

  absxk = std::abs(x[1]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  absxk = std::abs(x[2]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  absxk = std::abs(x[3]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  return scale * std::sqrt(y);
}

real_T rt_roundd(real_T u)
{
  real_T y;
  if (std::abs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = std::floor(u + 0.5);
    } else if (u > -0.5) {
      y = 0.0;
    } else {
      y = std::ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

// Model step function
void MCU::step()
{
  __m128d tmp_3;
  __m128d tmp_5;
  __m128d tmp_6;
  __m128d tmp_7;
  real_T R[9];
  real_T Rdes[9];
  real_T Rdes_0[9];
  real_T S_tmp[9];
  real_T S_tmp_0[9];
  real_T rtb_F_cmd_0[4];
  real_T rtb_Add[3];
  real_T rtb_tau_ref[3];
  real_T rtb_tau_ref_0[3];
  real_T theta[3];
  real_T tmp_2[3];
  real_T tmp_4[2];
  real_T R_tmp;
  real_T R_tmp_0;
  real_T R_tmp_1;
  real_T R_tmp_2;
  real_T Rdes_tmp;
  real_T Rdes_tmp_0;
  real_T Rdes_tmp_1;
  real_T Rdes_tmp_2;
  real_T na;
  real_T q_err_idx_1;
  real_T q_err_idx_2;
  real_T q_err_idx_3;
  real_T rtb_q_des_idx_1;
  real_T rtb_q_des_idx_2;
  real_T rtb_q_des_idx_3;
  real_T rtb_q_ref_idx_2;
  real_T rtb_q_ref_idx_3;
  real_T tmp;
  real_T tmp_0;
  real_T tmp_1;
  int32_T S_tmp_tmp;
  int32_T S_tmp_tmp_0;
  int32_T i;
  int32_T i_0;
  boolean_T over_inst;
  static const int8_T b_b[3]{ 0, 0, 1 };

  // Sum: '<Root>/Add' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Gain: '<Root>/Gain1'
  //   Inport: '<Root>/Bus_IMU'
  //   Sum: '<Root>/Subtract'

  _mm_storeu_pd(&rtb_Add[0], _mm_add_pd(_mm_sub_pd(_mm_loadu_pd
    (&mcu_U.Bus_IMU_k.imu_gyro[0]), _mm_set_pd(-0.17453292519943295,
    0.17453292519943295)), _mm_loadu_pd(&mcu_ConstB.Gain1[0])));
  rtb_Add[2] = (mcu_U.Bus_IMU_k.imu_gyro[2] - 0.17453292519943295) +
    mcu_ConstB.Gain1[2];

  // MATLAB Function: '<Root>/MATLAB Function' incorporates:
  //   Inport: '<Root>/Bus_Cmd'
  //   Logic: '<Root>/Logical Operator'
  //   Sum: '<Root>/Add'

  over_inst = (std::sqrt((rtb_Add[0] * rtb_Add[0] + rtb_Add[1] * rtb_Add[1]) +
    rtb_Add[2] * rtb_Add[2]) > 10.0);
  if (over_inst) {
    if (mcu_DW.cnt < 4) {
      mcu_DW.cnt = static_cast<uint16_T>(mcu_DW.cnt + 1);
    }
  } else {
    mcu_DW.cnt = 0U;
  }

  mcu_DW.latched = (((mcu_DW.cnt >= 4) && (!mcu_DW.latched)) || mcu_DW.latched);
  mcu_DW.latched = (((mcu_U.Bus_Cmd_l.estop == 2) && (!mcu_DW.latched)) ||
                    mcu_DW.latched);
  if (mcu_DW.latched && (mcu_U.Bus_Cmd_l.ack && (!mcu_DW.ack_prev)) &&
      (!over_inst) && (mcu_U.Bus_Cmd_l.estop != 2)) {
    mcu_DW.latched = false;
    mcu_DW.cnt = 0U;
  }

  mcu_DW.ack_prev = mcu_U.Bus_Cmd_l.ack;

  // MATLAB Function: '<S3>/MATLAB Function' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Constant: '<S3>/Constant1'
  //   Constant: '<S3>/Constant3'
  //   Constant: '<S3>/Constant4'
  //   Inport: '<Root>/Bus_Cmd'
  //   Inport: '<Root>/Bus_IMU'

  if (!mcu_DW.q_not_empty) {
    mcu_DW.q[0] = 1.0;
    mcu_DW.q[1] = 0.0;
    mcu_DW.q[2] = 0.0;
    mcu_DW.q[3] = 0.0;
    mcu_DW.q_not_empty = true;
  }

  na = mcu_norm(mcu_U.Bus_IMU_k.imu_acc);
  if (na > 1.0E-6) {
    R_tmp = mcu_DW.q[0] * mcu_DW.q[0];
    q_err_idx_2 = mcu_DW.q[1] * mcu_DW.q[1];
    q_err_idx_3 = mcu_DW.q[2] * mcu_DW.q[2];
    q_err_idx_1 = mcu_DW.q[3] * mcu_DW.q[3];
    S_tmp[0] = ((R_tmp + q_err_idx_2) - q_err_idx_3) - q_err_idx_1;
    tmp_6 = _mm_set1_pd(mcu_DW.q[0]);
    tmp_7 = _mm_set1_pd(2.0);
    tmp_5 = _mm_mul_pd(_mm_add_pd(_mm_mul_pd(_mm_set1_pd(mcu_DW.q[1]),
      _mm_loadu_pd(&mcu_DW.q[2])), _mm_mul_pd(_mm_mul_pd(tmp_6, _mm_set_pd
      (mcu_DW.q[2], mcu_DW.q[3])), _mm_set_pd(-1.0, 1.0))), tmp_7);
    _mm_storeu_pd(&tmp_4[0], tmp_5);
    S_tmp[3] = tmp_4[0];
    S_tmp[6] = tmp_4[1];
    S_tmp[1] = (mcu_DW.q[1] * mcu_DW.q[2] - mcu_DW.q[0] * mcu_DW.q[3]) * 2.0;
    R_tmp -= q_err_idx_2;
    S_tmp[4] = (R_tmp + q_err_idx_3) - q_err_idx_1;
    tmp_6 = _mm_mul_pd(_mm_add_pd(_mm_mul_pd(_mm_set_pd(mcu_DW.q[1], mcu_DW.q[2]),
      _mm_set1_pd(mcu_DW.q[3])), _mm_mul_pd(tmp_6, _mm_loadu_pd(&mcu_DW.q[1]))),
                       tmp_7);
    _mm_storeu_pd(&tmp_4[0], tmp_6);
    S_tmp[7] = tmp_4[0];
    S_tmp[2] = tmp_4[1];
    S_tmp[5] = (mcu_DW.q[2] * mcu_DW.q[3] - mcu_DW.q[0] * mcu_DW.q[1]) * 2.0;
    S_tmp[8] = (R_tmp - q_err_idx_3) + q_err_idx_1;
    rtb_Add[0] = mcu_U.Bus_IMU_k.imu_acc[0] / na;
    rtb_Add[1] = mcu_U.Bus_IMU_k.imu_acc[1] / na;
    rtb_Add[2] = mcu_U.Bus_IMU_k.imu_acc[2] / na;
    na = 0.0;
    rtb_q_des_idx_1 = 0.0;
    rtb_q_des_idx_2 = 0.0;
    for (i = 0; i < 3; i++) {
      R_tmp = b_b[i];
      tmp_6 = _mm_add_pd(_mm_mul_pd(_mm_loadu_pd(&S_tmp[3 * i]), _mm_set1_pd
        (R_tmp)), _mm_set_pd(rtb_q_des_idx_1, na));
      _mm_storeu_pd(&tmp_4[0], tmp_6);
      na = tmp_4[0];
      rtb_q_des_idx_1 = tmp_4[1];
      rtb_q_des_idx_2 += S_tmp[3 * i + 2] * R_tmp;
    }

    _mm_storeu_pd(&theta[0], _mm_sub_pd(_mm_mul_pd(_mm_set_pd(na, rtb_Add[1]),
      _mm_set_pd(rtb_Add[2], rtb_q_des_idx_2)), _mm_mul_pd(_mm_set_pd(rtb_Add[0],
      rtb_q_des_idx_1), _mm_set_pd(rtb_q_des_idx_2, rtb_Add[2]))));
    theta[2] = rtb_Add[0] * rtb_q_des_idx_1 - na * rtb_Add[1];
  } else {
    theta[0] = 0.0;
    theta[1] = 0.0;
    theta[2] = 0.0;
  }

  na = mcu_norm_j(mcu_U.Bus_Cmd_l.q_ext);
  if (na > 0.5) {
    tmp_6 = _mm_set1_pd(na);
    _mm_storeu_pd(&tmp_4[0], _mm_div_pd(_mm_loadu_pd(&mcu_U.Bus_Cmd_l.q_ext[0]),
      tmp_6));
    na = tmp_4[0];
    rtb_q_des_idx_1 = tmp_4[1];
    _mm_storeu_pd(&tmp_4[0], _mm_div_pd(_mm_loadu_pd(&mcu_U.Bus_Cmd_l.q_ext[2]),
      tmp_6));
    rtb_q_des_idx_2 = tmp_4[0];
    rtb_q_des_idx_3 = tmp_4[1];
    _mm_storeu_pd(&tmp_4[0], _mm_add_pd(_mm_add_pd(_mm_add_pd(_mm_mul_pd
      (_mm_set1_pd(mcu_DW.q[0]), _mm_set_pd(tmp_4[0], rtb_q_des_idx_1)),
      _mm_mul_pd(_mm_mul_pd(_mm_set_pd(-mcu_DW.q[1], na), _mm_set_pd(tmp_4[1],
      -mcu_DW.q[1])), _mm_set_pd(-1.0, 1.0))), _mm_mul_pd(_mm_set_pd(na,
      -mcu_DW.q[2]), _mm_set_pd(-mcu_DW.q[2], tmp_4[1]))), _mm_mul_pd(_mm_mul_pd
      (_mm_set_pd(rtb_q_des_idx_1, tmp_4[0]), _mm_set1_pd(-mcu_DW.q[3])),
      _mm_set_pd(1.0, -1.0))));
    q_err_idx_1 = tmp_4[0];
    q_err_idx_2 = tmp_4[1];
    q_err_idx_3 = ((mcu_DW.q[0] * rtb_q_des_idx_3 + -mcu_DW.q[1] *
                    rtb_q_des_idx_2) - rtb_q_des_idx_1 * -mcu_DW.q[2]) + na *
      -mcu_DW.q[3];
    if (((mcu_DW.q[0] * na - -mcu_DW.q[1] * rtb_q_des_idx_1) - -mcu_DW.q[2] *
         rtb_q_des_idx_2) - -mcu_DW.q[3] * rtb_q_des_idx_3 < 0.0) {
      q_err_idx_1 = -tmp_4[0];
      q_err_idx_2 = -tmp_4[1];
      q_err_idx_3 = -q_err_idx_3;
    }

    rtb_tau_ref[0] = 2.0 * q_err_idx_1;
    rtb_tau_ref[1] = 2.0 * q_err_idx_2;
    rtb_tau_ref[2] = 2.0 * q_err_idx_3;
  } else {
    rtb_tau_ref[0] = 0.0;
    rtb_tau_ref[1] = 0.0;
    rtb_tau_ref[2] = 0.0;
  }

  rtb_Add[0] = mcu_U.Bus_IMU_k.imu_gyro[0] - 0.17453292519943295;
  theta[0] = ((25.0 * rtb_tau_ref[0] + theta[0]) + (mcu_U.Bus_IMU_k.imu_gyro[0]
    - 0.17453292519943295)) * 0.001;
  rtb_Add[1] = mcu_U.Bus_IMU_k.imu_gyro[1] - -0.17453292519943295;
  theta[1] = ((25.0 * rtb_tau_ref[1] + theta[1]) + (mcu_U.Bus_IMU_k.imu_gyro[1]
    - -0.17453292519943295)) * 0.001;
  rtb_Add[2] = mcu_U.Bus_IMU_k.imu_gyro[2] - 0.17453292519943295;
  theta[2] = ((25.0 * rtb_tau_ref[2] + theta[2]) + (mcu_U.Bus_IMU_k.imu_gyro[2]
    - 0.17453292519943295)) * 0.001;
  q_err_idx_1 = mcu_norm(theta);
  if (q_err_idx_1 > 1.0E-9) {
    na = q_err_idx_1 / 2.0;
    rtb_q_des_idx_3 = std::sin(na);
    na = std::cos(na);
    tmp_6 = _mm_mul_pd(_mm_div_pd(_mm_loadu_pd(&theta[0]), _mm_set1_pd
      (q_err_idx_1)), _mm_set1_pd(rtb_q_des_idx_3));
    _mm_storeu_pd(&tmp_4[0], tmp_6);
    rtb_q_des_idx_1 = tmp_4[0];
    rtb_q_des_idx_2 = tmp_4[1];
    rtb_q_des_idx_3 *= theta[2] / q_err_idx_1;
  } else {
    na = 1.0;
    tmp_6 = _mm_mul_pd(_mm_set1_pd(0.5), _mm_loadu_pd(&theta[0]));
    _mm_storeu_pd(&tmp_4[0], tmp_6);
    rtb_q_des_idx_1 = tmp_4[0];
    rtb_q_des_idx_2 = tmp_4[1];
    rtb_q_des_idx_3 = 0.5 * theta[2];
  }

  R_tmp = mcu_DW.q[0];
  q_err_idx_2 = mcu_DW.q[1];
  q_err_idx_3 = mcu_DW.q[2];
  q_err_idx_1 = mcu_DW.q[3];
  rtb_q_ref_idx_2 = mcu_DW.q[0];
  rtb_q_ref_idx_3 = mcu_DW.q[1];
  R_tmp_0 = mcu_DW.q[2];
  R_tmp_1 = mcu_DW.q[3];
  R_tmp_2 = mcu_DW.q[0];
  Rdes_tmp = mcu_DW.q[1];
  Rdes_tmp_0 = mcu_DW.q[2];
  Rdes_tmp_1 = mcu_DW.q[3];
  Rdes_tmp_2 = mcu_DW.q[0];
  tmp = mcu_DW.q[1];
  tmp_0 = mcu_DW.q[2];
  tmp_1 = mcu_DW.q[3];
  mcu_DW.q[0] = ((R_tmp * na - q_err_idx_2 * rtb_q_des_idx_1) - q_err_idx_3 *
                 rtb_q_des_idx_2) - q_err_idx_1 * rtb_q_des_idx_3;
  mcu_DW.q[1] = ((rtb_q_ref_idx_2 * rtb_q_des_idx_1 + na * rtb_q_ref_idx_3) +
                 R_tmp_0 * rtb_q_des_idx_3) - rtb_q_des_idx_2 * R_tmp_1;
  mcu_DW.q[2] = ((R_tmp_2 * rtb_q_des_idx_2 - Rdes_tmp * rtb_q_des_idx_3) + na *
                 Rdes_tmp_0) + rtb_q_des_idx_1 * Rdes_tmp_1;
  mcu_DW.q[3] = ((Rdes_tmp_2 * rtb_q_des_idx_3 + tmp * rtb_q_des_idx_2) -
                 rtb_q_des_idx_1 * tmp_0) + na * tmp_1;
  tmp_6 = _mm_set1_pd(mcu_norm_j(mcu_DW.q));
  tmp_7 = _mm_div_pd(_mm_loadu_pd(&mcu_DW.q[0]), tmp_6);
  _mm_storeu_pd(&mcu_DW.q[0], tmp_7);
  tmp_6 = _mm_div_pd(_mm_loadu_pd(&mcu_DW.q[2]), tmp_6);
  _mm_storeu_pd(&mcu_DW.q[2], tmp_6);

  // RateTransition: '<Root>/Rate Transition'
  over_inst = ((&mcu_M)->Timing.TaskCounters.TID[1] == 0);
  if (over_inst) {
    // RateTransition: '<Root>/Rate Transition'
    mcu_B.RateTransition = mcu_DW.RateTransition_Buffer0;
  }

  // End of RateTransition: '<Root>/Rate Transition'

  // MATLAB Function: '<Root>/handing the hard descent in case of low battery' incorporates:
  //   Inport: '<Root>/Bus_Cmd'

  if (mcu_B.RateTransition) {
    q_err_idx_1 = 9.3719834999999989;
    na = 1.0;
    q_err_idx_2 = 1.0;
    rtb_q_des_idx_1 = 0.0;
    q_err_idx_3 = 0.0;
    rtb_q_des_idx_2 = 0.0;
    rtb_q_ref_idx_2 = 0.0;
    rtb_q_des_idx_3 = 0.0;
    rtb_q_ref_idx_3 = 0.0;
    theta[0] = 0.0;
    rtb_tau_ref[0] = 0.0;
    theta[1] = 0.0;
    rtb_tau_ref[1] = 0.0;
    theta[2] = 0.0;
    rtb_tau_ref[2] = 0.0;
  } else {
    q_err_idx_1 = mcu_U.Bus_Cmd_l.F_des;
    na = mcu_U.Bus_Cmd_l.q_des[0];
    q_err_idx_2 = mcu_U.Bus_Cmd_l.q_ref[0];
    rtb_q_des_idx_1 = mcu_U.Bus_Cmd_l.q_des[1];
    q_err_idx_3 = mcu_U.Bus_Cmd_l.q_ref[1];
    rtb_q_des_idx_2 = mcu_U.Bus_Cmd_l.q_des[2];
    rtb_q_ref_idx_2 = mcu_U.Bus_Cmd_l.q_ref[2];
    rtb_q_des_idx_3 = mcu_U.Bus_Cmd_l.q_des[3];
    rtb_q_ref_idx_3 = mcu_U.Bus_Cmd_l.q_ref[3];
    theta[0] = mcu_U.Bus_Cmd_l.Omega_ref[0];
    rtb_tau_ref[0] = mcu_U.Bus_Cmd_l.tau_ref[0];
    theta[1] = mcu_U.Bus_Cmd_l.Omega_ref[1];
    rtb_tau_ref[1] = mcu_U.Bus_Cmd_l.tau_ref[1];
    theta[2] = mcu_U.Bus_Cmd_l.Omega_ref[2];
    rtb_tau_ref[2] = mcu_U.Bus_Cmd_l.tau_ref[2];
  }

  // End of MATLAB Function: '<Root>/handing the hard descent in case of low battery' 

  // MATLAB Function: '<S4>/MATLAB Function'
  R_tmp = mcu_DW.q[0] * mcu_DW.q[0];
  R_tmp_0 = mcu_DW.q[1] * mcu_DW.q[1];
  R_tmp_1 = mcu_DW.q[2] * mcu_DW.q[2];
  R_tmp_2 = mcu_DW.q[3] * mcu_DW.q[3];
  R[0] = ((R_tmp + R_tmp_0) - R_tmp_1) - R_tmp_2;
  tmp_6 = _mm_set1_pd(mcu_DW.q[0]);
  tmp_7 = _mm_set1_pd(2.0);
  tmp_5 = _mm_mul_pd(_mm_add_pd(_mm_mul_pd(_mm_set1_pd(mcu_DW.q[1]),
    _mm_loadu_pd(&mcu_DW.q[2])), _mm_mul_pd(_mm_mul_pd(tmp_6, _mm_set_pd
    (mcu_DW.q[2], mcu_DW.q[3])), _mm_set_pd(-1.0, 1.0))), tmp_7);
  _mm_storeu_pd(&R[1], tmp_5);
  R[3] = (mcu_DW.q[1] * mcu_DW.q[2] - mcu_DW.q[0] * mcu_DW.q[3]) * 2.0;
  R_tmp -= R_tmp_0;
  R[4] = (R_tmp + R_tmp_1) - R_tmp_2;
  tmp_6 = _mm_mul_pd(_mm_add_pd(_mm_mul_pd(_mm_set_pd(mcu_DW.q[1], mcu_DW.q[2]),
    _mm_set1_pd(mcu_DW.q[3])), _mm_mul_pd(tmp_6, _mm_loadu_pd(&mcu_DW.q[1]))),
                     tmp_7);
  _mm_storeu_pd(&R[5], tmp_6);
  R[7] = (mcu_DW.q[2] * mcu_DW.q[3] - mcu_DW.q[0] * mcu_DW.q[1]) * 2.0;
  R[8] = (R_tmp - R_tmp_1) + R_tmp_2;
  R_tmp = na * na;
  R_tmp_0 = rtb_q_des_idx_1 * rtb_q_des_idx_1;
  R_tmp_1 = rtb_q_des_idx_2 * rtb_q_des_idx_2;
  R_tmp_2 = rtb_q_des_idx_3 * rtb_q_des_idx_3;
  Rdes[0] = ((R_tmp + R_tmp_0) - R_tmp_1) - R_tmp_2;
  Rdes_tmp = rtb_q_des_idx_1 * rtb_q_des_idx_2;
  Rdes_tmp_0 = na * rtb_q_des_idx_3;
  Rdes[1] = (Rdes_tmp + Rdes_tmp_0) * 2.0;
  Rdes_tmp_1 = rtb_q_des_idx_1 * rtb_q_des_idx_3;
  Rdes_tmp_2 = na * rtb_q_des_idx_2;
  Rdes[2] = (Rdes_tmp_1 - Rdes_tmp_2) * 2.0;
  Rdes[3] = (Rdes_tmp - Rdes_tmp_0) * 2.0;
  R_tmp -= R_tmp_0;
  Rdes[4] = (R_tmp + R_tmp_1) - R_tmp_2;
  R_tmp_0 = rtb_q_des_idx_2 * rtb_q_des_idx_3;
  Rdes_tmp = na * rtb_q_des_idx_1;
  Rdes[5] = (R_tmp_0 + Rdes_tmp) * 2.0;
  Rdes[6] = (Rdes_tmp_1 + Rdes_tmp_2) * 2.0;
  Rdes[7] = (R_tmp_0 - Rdes_tmp) * 2.0;
  Rdes[8] = (R_tmp - R_tmp_1) + R_tmp_2;
  for (i = 0; i < 3; i++) {
    for (i_0 = 0; i_0 < 3; i_0++) {
      S_tmp_tmp = 3 * i + i_0;
      S_tmp[S_tmp_tmp] = R[3 * i_0 + i];
      Rdes_0[S_tmp_tmp] = (Rdes[3 * i_0 + 1] * R[3 * i + 1] + Rdes[3 * i_0] * R
                           [3 * i]) + Rdes[3 * i_0 + 2] * R[3 * i + 2];
      S_tmp_0[S_tmp_tmp] = 0.0;
    }
  }

  for (i = 0; i < 3; i++) {
    rtb_q_des_idx_3 = S_tmp_0[3 * i];
    S_tmp_tmp = 3 * i + 1;
    R_tmp_0 = S_tmp_0[S_tmp_tmp];
    S_tmp_tmp_0 = 3 * i + 2;
    R_tmp_1 = S_tmp_0[S_tmp_tmp_0];
    for (i_0 = 0; i_0 < 3; i_0++) {
      R_tmp = Rdes[3 * i + i_0];
      tmp_6 = _mm_add_pd(_mm_mul_pd(_mm_loadu_pd(&S_tmp[3 * i_0]), _mm_set1_pd
        (R_tmp)), _mm_set_pd(R_tmp_0, rtb_q_des_idx_3));
      _mm_storeu_pd(&tmp_4[0], tmp_6);
      rtb_q_des_idx_3 = tmp_4[0];
      R_tmp_0 = tmp_4[1];
      R_tmp_1 += S_tmp[3 * i_0 + 2] * R_tmp;
    }

    S_tmp_0[S_tmp_tmp_0] = R_tmp_1;
    S_tmp_0[S_tmp_tmp] = R_tmp_0;
    S_tmp_0[3 * i] = rtb_q_des_idx_3;
  }

  for (i = 0; i <= 6; i += 2) {
    tmp_6 = _mm_loadu_pd(&Rdes_0[i]);
    tmp_7 = _mm_loadu_pd(&S_tmp_0[i]);
    _mm_storeu_pd(&R[i], _mm_mul_pd(_mm_sub_pd(tmp_6, tmp_7), _mm_set1_pd(0.5)));
  }

  for (i = 8; i < 9; i++) {
    R[i] = (Rdes_0[i] - S_tmp_0[i]) * 0.5;
  }

  // Switch: '<Root>/Switch' incorporates:
  //   MATLAB Function: '<Root>/MATLAB Function'
  //   MATLAB Function: '<S4>/MATLAB Function'
  //   SignalConversion generated from: '<Root>/Gain'

  if (mcu_DW.latched) {
    // Outport: '<Root>/rotor_cmd' incorporates:
    //   Constant: '<Root>/Constant2'

    mcu_Y.rotor_cmd[0] = 0.0;
    mcu_Y.rotor_cmd[1] = 0.0;
    mcu_Y.rotor_cmd[2] = 0.0;
    mcu_Y.rotor_cmd[3] = 0.0;
  } else {
    // MATLAB Function: '<S4>/MATLAB Function' incorporates:
    //   Constant: '<S4>/Constant'
    //   Constant: '<S4>/Constant1'
    //   MATLAB Function: '<S3>/MATLAB Function'

    na = R[5];
    rtb_q_des_idx_1 = R[6];
    rtb_q_des_idx_2 = R[1];
    rtb_q_des_idx_3 = q_err_idx_2 * q_err_idx_2;
    R_tmp = q_err_idx_3 * q_err_idx_3;
    R_tmp_0 = rtb_q_ref_idx_2 * rtb_q_ref_idx_2;
    R_tmp_1 = rtb_q_ref_idx_3 * rtb_q_ref_idx_3;
    Rdes_0[0] = ((rtb_q_des_idx_3 + R_tmp) - R_tmp_0) - R_tmp_1;
    R_tmp_2 = q_err_idx_3 * rtb_q_ref_idx_2;
    Rdes_tmp = q_err_idx_2 * rtb_q_ref_idx_3;
    Rdes_0[1] = (R_tmp_2 + Rdes_tmp) * 2.0;
    Rdes_tmp_0 = q_err_idx_3 * rtb_q_ref_idx_3;
    Rdes_tmp_1 = q_err_idx_2 * rtb_q_ref_idx_2;
    Rdes_0[2] = (Rdes_tmp_0 - Rdes_tmp_1) * 2.0;
    Rdes_0[3] = (R_tmp_2 - Rdes_tmp) * 2.0;
    rtb_q_des_idx_3 -= R_tmp;
    Rdes_0[4] = (rtb_q_des_idx_3 + R_tmp_0) - R_tmp_1;
    R_tmp = rtb_q_ref_idx_2 * rtb_q_ref_idx_3;
    R_tmp_2 = q_err_idx_2 * q_err_idx_3;
    Rdes_0[5] = (R_tmp + R_tmp_2) * 2.0;
    Rdes_0[6] = (Rdes_tmp_0 + Rdes_tmp_1) * 2.0;
    Rdes_0[7] = (R_tmp - R_tmp_2) * 2.0;
    Rdes_0[8] = (rtb_q_des_idx_3 - R_tmp_0) + R_tmp_1;
    for (i = 0; i < 3; i++) {
      rtb_q_des_idx_3 = 0.0;
      R_tmp_0 = 0.0;
      R_tmp_1 = 0.0;
      for (i_0 = 0; i_0 < 3; i_0++) {
        R_tmp = Rdes_0[3 * i + i_0];
        tmp_6 = _mm_add_pd(_mm_mul_pd(_mm_loadu_pd(&S_tmp[3 * i_0]), _mm_set1_pd
          (R_tmp)), _mm_set_pd(R_tmp_0, rtb_q_des_idx_3));
        _mm_storeu_pd(&tmp_4[0], tmp_6);
        rtb_q_des_idx_3 = tmp_4[0];
        R_tmp_0 = tmp_4[1];
        R_tmp_1 += S_tmp[3 * i_0 + 2] * R_tmp;
      }

      S_tmp_0[3 * i + 2] = R_tmp_1;
      S_tmp_0[3 * i + 1] = R_tmp_0;
      S_tmp_0[3 * i] = rtb_q_des_idx_3;
    }

    rtb_q_des_idx_3 = theta[1];
    q_err_idx_2 = theta[0];
    q_err_idx_3 = theta[2];
    for (i = 0; i <= 0; i += 2) {
      tmp_6 = _mm_loadu_pd(&S_tmp_0[i + 3]);
      tmp_7 = _mm_loadu_pd(&S_tmp_0[i]);
      tmp_5 = _mm_loadu_pd(&S_tmp_0[i + 6]);
      tmp_3 = _mm_loadu_pd(&rtb_Add[i]);
      _mm_storeu_pd(&theta[i], _mm_sub_pd(tmp_3, _mm_add_pd(_mm_add_pd
        (_mm_mul_pd(tmp_6, _mm_set1_pd(rtb_q_des_idx_3)), _mm_mul_pd(tmp_7,
        _mm_set1_pd(q_err_idx_2))), _mm_mul_pd(tmp_5, _mm_set1_pd(q_err_idx_3)))));
      tmp_6 = _mm_loadu_pd(&rtb_tau_ref[i]);
      _mm_storeu_pd(&rtb_tau_ref_0[i], _mm_sub_pd(tmp_6, _mm_add_pd(_mm_add_pd
        (_mm_mul_pd(_mm_loadu_pd(&mcu_ConstP.Constant_Value_m[i + 3]),
                    _mm_set1_pd(rtb_q_des_idx_1)), _mm_mul_pd(_mm_loadu_pd
        (&mcu_ConstP.Constant_Value_m[i]), _mm_set1_pd(na))), _mm_mul_pd
        (_mm_loadu_pd(&mcu_ConstP.Constant_Value_m[i + 6]), _mm_set1_pd
         (rtb_q_des_idx_2)))));
      _mm_storeu_pd(&tmp_2[i], _mm_set1_pd(0.0));
    }

    for (i = 2; i < 3; i++) {
      _mm_storeu_pd(&tmp_4[0], _mm_sub_pd(_mm_set_pd(rtb_tau_ref[i], rtb_Add[i]),
        _mm_add_pd(_mm_add_pd(_mm_mul_pd(_mm_set_pd
        (mcu_ConstP.Constant_Value_m[i + 3], S_tmp_0[i + 3]), _mm_set_pd
        (rtb_q_des_idx_1, rtb_q_des_idx_3)), _mm_mul_pd(_mm_set_pd
        (mcu_ConstP.Constant_Value_m[i], S_tmp_0[i]), _mm_set_pd(na, q_err_idx_2))),
                   _mm_mul_pd(_mm_set_pd(mcu_ConstP.Constant_Value_m[i + 6],
        S_tmp_0[i + 6]), _mm_set_pd(rtb_q_des_idx_2, q_err_idx_3)))));
      theta[i] = tmp_4[0];
      rtb_tau_ref_0[i] = tmp_4[1];
    }

    R_tmp = 0.0;
    q_err_idx_2 = 0.0;
    q_err_idx_3 = 0.0;
    for (i = 0; i < 3; i++) {
      _mm_storeu_pd(&tmp_4[0], _mm_add_pd(_mm_mul_pd(_mm_loadu_pd
        (&mcu_ConstP.Constant1_Value_o[3 * i]), _mm_set1_pd(theta[i])),
        _mm_set_pd(q_err_idx_2, R_tmp)));
      R_tmp = tmp_4[0];
      q_err_idx_2 = tmp_4[1];
      q_err_idx_3 += mcu_ConstP.Constant1_Value_o[3 * i + 2] * theta[i];
    }

    tmp_2[2] = q_err_idx_3;
    tmp_2[1] = q_err_idx_2;
    tmp_2[0] = R_tmp;

    // SignalConversion generated from: '<Root>/Gain' incorporates:
    //   MATLAB Function: '<S4>/MATLAB Function'

    rtb_F_cmd_0[0] = q_err_idx_1;
    tmp_6 = _mm_sub_pd(_mm_loadu_pd(&rtb_tau_ref_0[0]), _mm_loadu_pd(&tmp_2[0]));
    _mm_storeu_pd(&rtb_F_cmd_0[1], tmp_6);

    // SignalConversion generated from: '<Root>/Gain' incorporates:
    //   MATLAB Function: '<S4>/MATLAB Function'

    rtb_F_cmd_0[3] = rtb_tau_ref_0[2] - q_err_idx_3;

    // Gain: '<Root>/Gain'
    na = 0.0;
    rtb_q_des_idx_1 = 0.0;
    rtb_q_des_idx_2 = 0.0;
    q_err_idx_1 = 0.0;
    for (i = 0; i < 4; i++) {
      i_0 = i << 2;
      tmp_6 = _mm_set1_pd(rtb_F_cmd_0[i]);
      _mm_storeu_pd(&tmp_4[0], _mm_add_pd(_mm_mul_pd(_mm_loadu_pd
        (&mcu_ConstP.Gain_Gain[i_0]), tmp_6), _mm_set_pd(rtb_q_des_idx_1, na)));
      na = tmp_4[0];
      rtb_q_des_idx_1 = tmp_4[1];
      _mm_storeu_pd(&tmp_4[0], _mm_add_pd(_mm_mul_pd(_mm_loadu_pd
        (&mcu_ConstP.Gain_Gain[i_0 + 2]), tmp_6), _mm_set_pd(q_err_idx_1,
        rtb_q_des_idx_2)));
      rtb_q_des_idx_2 = tmp_4[0];
      q_err_idx_1 = tmp_4[1];
    }

    // End of Gain: '<Root>/Gain'

    // Outport: '<Root>/rotor_cmd' incorporates:
    //   Abs: '<Root>/Abs'
    //   Sqrt: '<Root>/Sqrt'

    mcu_Y.rotor_cmd[0] = std::sqrt(std::abs(na));
    mcu_Y.rotor_cmd[1] = std::sqrt(std::abs(rtb_q_des_idx_1));
    mcu_Y.rotor_cmd[2] = std::sqrt(std::abs(rtb_q_des_idx_2));
    mcu_Y.rotor_cmd[3] = std::sqrt(std::abs(q_err_idx_1));
  }

  // End of Switch: '<Root>/Switch'
  if (over_inst) {
    // MATLAB Function: '<Root>/volts to 12-bit counts' incorporates:
    //   Inport: '<Root>/batt_count'

    na = rt_roundd(mcu_U.batt_count / 0.014652161172161169);

    // MATLAB Function: '<Root>/MATLAB Function1'
    if (!mcu_DW.Vf_not_empty) {
      mcu_DW.Vf = 0.014652161172161169 * na;
      mcu_DW.Vf_not_empty = true;
    }

    mcu_DW.Vf += (0.014652161172161169 * na - mcu_DW.Vf) * 0.76034896355822423;
    switch (mcu_DW.state) {
     case 0U:
      if (mcu_DW.Vf <= 14.0) {
        mcu_DW.state = 1U;
      }
      break;

     case 1U:
      if (mcu_DW.Vf <= 13.4) {
        mcu_DW.state = 2U;
      } else if (mcu_DW.Vf >= 14.2) {
        mcu_DW.state = 0U;
      }
      break;

     default:
      if (mcu_DW.Vf >= 13.6) {
        mcu_DW.state = 1U;
      }
      break;
    }

    mcu_DW.landed = ((mcu_DW.Vf <= 12.0) || mcu_DW.landed);

    // Outport: '<Root>/led' incorporates:
    //   MATLAB Function: '<Root>/MATLAB Function1'

    mcu_Y.led = mcu_DW.state;

    // Update for RateTransition: '<Root>/Rate Transition'
    mcu_DW.RateTransition_Buffer0 = mcu_DW.landed;
  }

  rate_scheduler((&mcu_M));
}

// Model initialize function
void MCU::initialize()
{
  // (no initialization code required)
}

// Model terminate function
void MCU::terminate()
{
  // (no terminate code required)
}

const char_T* MCU::RT_MODEL_mcu_T::getErrorStatus() const
{
  return (errorStatus);
}

void MCU::RT_MODEL_mcu_T::setErrorStatus(const char_T* const volatile
  aErrorStatus)
{
  (errorStatus = aErrorStatus);
}

// Constructor
MCU::MCU() :
  mcu_U(),
  mcu_Y(),
  mcu_B(),
  mcu_DW(),
  mcu_M()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
MCU::~MCU() = default;

// Real-Time Model get method
MCU::RT_MODEL_mcu_T * MCU::getRTM()
{
  return (&mcu_M);
}

//
// File trailer for generated code.
//
// [EOF]
//
