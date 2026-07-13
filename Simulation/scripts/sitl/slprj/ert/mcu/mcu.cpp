//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: mcu.cpp
//
// Code generated for Simulink model 'mcu'.
//
// Model version                  : 1.256
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Fri Jul 10 10:28:05 2026
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
#include "norm_XZxrXx9N.h"
#include "norm_bQtbKOLy.h"
#include "rt_roundd.h"
#include "mcu_private.h"

// Output and update for referenced model: 'mcu'
void MCU::step(const real_T rtu_Bus_IMU_imu_gyro[3], const real_T
               rtu_Bus_IMU_imu_acc[3], const real_T *rtu_Bus_Cmd_F_des, const
               real_T rtu_Bus_Cmd_q_des[4], const real_T rtu_Bus_Cmd_q_ref[4],
               const real_T rtu_Bus_Cmd_Omega_ref[3], const real_T
               rtu_Bus_Cmd_tau_ref[3], const real_T rtu_Bus_Cmd_q_ext[4], const
               uint8_T *rtu_Bus_Cmd_estop, const boolean_T *rtu_Bus_Cmd_ack,
               const real_T *rtu_batt_count, real_T rty_rotor_cmd[4], uint8_T
               *rty_led, real_T rty_throttle[4])
{
  __m128d tmp_4;
  __m128d tmp_6;
  __m128d tmp_7;
  __m128d tmp_8;
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
  real_T tmp_3[3];
  real_T tmp_5[2];
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
  real_T rtb_q_ref_idx_3;
  real_T tmp;
  real_T tmp_0;
  real_T tmp_1;
  real_T tmp_2;
  int32_T S_tmp_tmp;
  int32_T S_tmp_tmp_0;
  int32_T i;
  int_T idx;
  boolean_T over_inst;
  static const int8_T b_b[3]{ 0, 0, 1 };

  // Sum: '<Root>/Subtract' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Gain: '<Root>/Gain1'
  //   Sum: '<Root>/Add'

  tmp_7 = _mm_add_pd(_mm_sub_pd(_mm_loadu_pd(&rtu_Bus_IMU_imu_gyro[0]),
    _mm_set_pd(-0.17453292519943295, 0.17453292519943295)), _mm_loadu_pd
                     (&mcu_ConstB.Gain1[0]));

  // Sum: '<Root>/Add' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Sum: '<Root>/Subtract'

  _mm_storeu_pd(&rtb_Add[0], tmp_7);
  rtb_Add[2] = (rtu_Bus_IMU_imu_gyro[2] - 0.17453292519943295) +
    mcu_ConstB.Gain1[2];

  // MATLAB Function: '<Root>/MATLAB Function' incorporates:
  //   Sum: '<Root>/Add'

  over_inst = (std::sqrt((rtb_Add[0] * rtb_Add[0] + rtb_Add[1] * rtb_Add[1]) +
    rtb_Add[2] * rtb_Add[2]) > 8.5);
  if (over_inst) {
    if (mcu_DW.cnt < 4) {
      mcu_DW.cnt = static_cast<uint16_T>(mcu_DW.cnt + 1);
    }
  } else {
    mcu_DW.cnt = 0U;
  }

  if ((mcu_DW.cnt >= 4) && (!mcu_DW.latched)) {
    mcu_DW.latched = true;
    mcu_DW.src = 1U;
  }

  if ((*rtu_Bus_Cmd_estop == 2) && (!mcu_DW.latched)) {
    mcu_DW.latched = true;
    mcu_DW.src = 2U;
  }

  if (mcu_DW.latched && ((*rtu_Bus_Cmd_ack) && (!mcu_DW.ack_prev)) &&
      (!over_inst) && (*rtu_Bus_Cmd_estop != 2)) {
    mcu_DW.latched = false;
    mcu_DW.cnt = 0U;
    mcu_DW.src = 0U;
  }

  mcu_DW.ack_prev = *rtu_Bus_Cmd_ack;

  // MATLAB Function: '<S3>/MATLAB Function' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Constant: '<S3>/Constant1'
  //   Constant: '<S3>/Constant3'
  //   Constant: '<S3>/Constant4'

  if (!mcu_DW.q_not_empty) {
    mcu_DW.q[0] = 1.0;
    mcu_DW.q[1] = 0.0;
    mcu_DW.q[2] = 0.0;
    mcu_DW.q[3] = 0.0;
    mcu_DW.q_not_empty = true;
  }

  na = norm_XZxrXx9N(rtu_Bus_IMU_imu_acc);
  if (na > 1.0E-6) {
    R_tmp = mcu_DW.q[0] * mcu_DW.q[0];
    q_err_idx_1 = mcu_DW.q[1] * mcu_DW.q[1];
    q_err_idx_2 = mcu_DW.q[2] * mcu_DW.q[2];
    q_err_idx_3 = mcu_DW.q[3] * mcu_DW.q[3];
    S_tmp[0] = ((R_tmp + q_err_idx_1) - q_err_idx_2) - q_err_idx_3;
    tmp_7 = _mm_set1_pd(mcu_DW.q[0]);
    tmp_8 = _mm_set1_pd(2.0);
    tmp_6 = _mm_mul_pd(_mm_add_pd(_mm_mul_pd(_mm_set1_pd(mcu_DW.q[1]),
      _mm_loadu_pd(&mcu_DW.q[2])), _mm_mul_pd(_mm_mul_pd(tmp_7, _mm_set_pd
      (mcu_DW.q[2], mcu_DW.q[3])), _mm_set_pd(-1.0, 1.0))), tmp_8);
    _mm_storeu_pd(&tmp_5[0], tmp_6);
    S_tmp[3] = tmp_5[0];
    S_tmp[6] = tmp_5[1];
    S_tmp[1] = (mcu_DW.q[1] * mcu_DW.q[2] - mcu_DW.q[0] * mcu_DW.q[3]) * 2.0;
    R_tmp -= q_err_idx_1;
    S_tmp[4] = (R_tmp + q_err_idx_2) - q_err_idx_3;
    tmp_7 = _mm_mul_pd(_mm_add_pd(_mm_mul_pd(_mm_set_pd(mcu_DW.q[1], mcu_DW.q[2]),
      _mm_set1_pd(mcu_DW.q[3])), _mm_mul_pd(tmp_7, _mm_loadu_pd(&mcu_DW.q[1]))),
                       tmp_8);
    _mm_storeu_pd(&tmp_5[0], tmp_7);
    S_tmp[7] = tmp_5[0];
    S_tmp[2] = tmp_5[1];
    S_tmp[5] = (mcu_DW.q[2] * mcu_DW.q[3] - mcu_DW.q[0] * mcu_DW.q[1]) * 2.0;
    S_tmp[8] = (R_tmp - q_err_idx_2) + q_err_idx_3;
    rtb_Add[0] = rtu_Bus_IMU_imu_acc[0] / na;
    rtb_Add[1] = rtu_Bus_IMU_imu_acc[1] / na;
    rtb_Add[2] = rtu_Bus_IMU_imu_acc[2] / na;
    na = 0.0;
    rtb_q_des_idx_1 = 0.0;
    rtb_q_des_idx_2 = 0.0;
    for (idx = 0; idx < 3; idx++) {
      R_tmp = b_b[idx];
      tmp_7 = _mm_add_pd(_mm_mul_pd(_mm_loadu_pd(&S_tmp[3 * idx]), _mm_set1_pd
        (R_tmp)), _mm_set_pd(rtb_q_des_idx_1, na));
      _mm_storeu_pd(&tmp_5[0], tmp_7);
      na = tmp_5[0];
      rtb_q_des_idx_1 = tmp_5[1];
      rtb_q_des_idx_2 += S_tmp[3 * idx + 2] * R_tmp;
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

  na = norm_bQtbKOLy(rtu_Bus_Cmd_q_ext);
  if (na > 0.5) {
    tmp_7 = _mm_set1_pd(na);
    tmp_8 = _mm_div_pd(_mm_loadu_pd(&rtu_Bus_Cmd_q_ext[0]), tmp_7);
    _mm_storeu_pd(&tmp_5[0], tmp_8);
    na = tmp_5[0];
    rtb_q_des_idx_1 = tmp_5[1];
    tmp_7 = _mm_div_pd(_mm_loadu_pd(&rtu_Bus_Cmd_q_ext[2]), tmp_7);
    _mm_storeu_pd(&tmp_5[0], tmp_7);
    rtb_q_des_idx_2 = tmp_5[0];
    rtb_q_des_idx_3 = tmp_5[1];
    _mm_storeu_pd(&tmp_5[0], _mm_add_pd(_mm_add_pd(_mm_add_pd(_mm_mul_pd
      (_mm_set1_pd(mcu_DW.q[0]), _mm_set_pd(tmp_5[0], rtb_q_des_idx_1)),
      _mm_mul_pd(_mm_mul_pd(_mm_set_pd(-mcu_DW.q[1], na), _mm_set_pd(tmp_5[1],
      -mcu_DW.q[1])), _mm_set_pd(-1.0, 1.0))), _mm_mul_pd(_mm_set_pd(na,
      -mcu_DW.q[2]), _mm_set_pd(-mcu_DW.q[2], tmp_5[1]))), _mm_mul_pd(_mm_mul_pd
      (_mm_set_pd(rtb_q_des_idx_1, tmp_5[0]), _mm_set1_pd(-mcu_DW.q[3])),
      _mm_set_pd(1.0, -1.0))));
    q_err_idx_1 = tmp_5[0];
    q_err_idx_2 = tmp_5[1];
    q_err_idx_3 = ((mcu_DW.q[0] * rtb_q_des_idx_3 + -mcu_DW.q[1] *
                    rtb_q_des_idx_2) - rtb_q_des_idx_1 * -mcu_DW.q[2]) + na *
      -mcu_DW.q[3];
    if (((mcu_DW.q[0] * na - -mcu_DW.q[1] * rtb_q_des_idx_1) - -mcu_DW.q[2] *
         rtb_q_des_idx_2) - -mcu_DW.q[3] * rtb_q_des_idx_3 < 0.0) {
      q_err_idx_1 = -tmp_5[0];
      q_err_idx_2 = -tmp_5[1];
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

  rtb_Add[0] = rtu_Bus_IMU_imu_gyro[0] - 0.17453292519943295;
  theta[0] = ((25.0 * rtb_tau_ref[0] + theta[0]) + (rtu_Bus_IMU_imu_gyro[0] -
    0.17453292519943295)) * 0.001;
  rtb_Add[1] = rtu_Bus_IMU_imu_gyro[1] - -0.17453292519943295;
  theta[1] = ((25.0 * rtb_tau_ref[1] + theta[1]) + (rtu_Bus_IMU_imu_gyro[1] -
    -0.17453292519943295)) * 0.001;
  rtb_Add[2] = rtu_Bus_IMU_imu_gyro[2] - 0.17453292519943295;
  theta[2] = ((25.0 * rtb_tau_ref[2] + theta[2]) + (rtu_Bus_IMU_imu_gyro[2] -
    0.17453292519943295)) * 0.001;
  rtb_q_des_idx_3 = norm_XZxrXx9N(theta);
  if (rtb_q_des_idx_3 > 1.0E-9) {
    na = rtb_q_des_idx_3 / 2.0;
    q_err_idx_1 = std::sin(na);
    na = std::cos(na);
    tmp_7 = _mm_mul_pd(_mm_div_pd(_mm_loadu_pd(&theta[0]), _mm_set1_pd
      (rtb_q_des_idx_3)), _mm_set1_pd(q_err_idx_1));
    _mm_storeu_pd(&tmp_5[0], tmp_7);
    rtb_q_des_idx_1 = tmp_5[0];
    rtb_q_des_idx_2 = tmp_5[1];
    rtb_q_des_idx_3 = theta[2] / rtb_q_des_idx_3 * q_err_idx_1;
  } else {
    na = 1.0;
    tmp_7 = _mm_mul_pd(_mm_set1_pd(0.5), _mm_loadu_pd(&theta[0]));
    _mm_storeu_pd(&tmp_5[0], tmp_7);
    rtb_q_des_idx_1 = tmp_5[0];
    rtb_q_des_idx_2 = tmp_5[1];
    rtb_q_des_idx_3 = 0.5 * theta[2];
  }

  R_tmp = mcu_DW.q[0];
  q_err_idx_1 = mcu_DW.q[1];
  q_err_idx_2 = mcu_DW.q[2];
  q_err_idx_3 = mcu_DW.q[3];
  rtb_q_ref_idx_3 = mcu_DW.q[0];
  R_tmp_0 = mcu_DW.q[1];
  R_tmp_1 = mcu_DW.q[2];
  R_tmp_2 = mcu_DW.q[3];
  Rdes_tmp = mcu_DW.q[0];
  Rdes_tmp_0 = mcu_DW.q[1];
  Rdes_tmp_1 = mcu_DW.q[2];
  Rdes_tmp_2 = mcu_DW.q[3];
  tmp = mcu_DW.q[0];
  tmp_0 = mcu_DW.q[1];
  tmp_1 = mcu_DW.q[2];
  tmp_2 = mcu_DW.q[3];
  mcu_DW.q[0] = ((R_tmp * na - q_err_idx_1 * rtb_q_des_idx_1) - q_err_idx_2 *
                 rtb_q_des_idx_2) - q_err_idx_3 * rtb_q_des_idx_3;
  mcu_DW.q[1] = ((rtb_q_ref_idx_3 * rtb_q_des_idx_1 + na * R_tmp_0) + R_tmp_1 *
                 rtb_q_des_idx_3) - rtb_q_des_idx_2 * R_tmp_2;
  mcu_DW.q[2] = ((Rdes_tmp * rtb_q_des_idx_2 - Rdes_tmp_0 * rtb_q_des_idx_3) +
                 na * Rdes_tmp_1) + rtb_q_des_idx_1 * Rdes_tmp_2;
  mcu_DW.q[3] = ((tmp * rtb_q_des_idx_3 + tmp_0 * rtb_q_des_idx_2) -
                 rtb_q_des_idx_1 * tmp_1) + na * tmp_2;
  tmp_7 = _mm_set1_pd(norm_bQtbKOLy(mcu_DW.q));
  tmp_8 = _mm_div_pd(_mm_loadu_pd(&mcu_DW.q[0]), tmp_7);
  _mm_storeu_pd(&mcu_DW.q[0], tmp_8);
  tmp_7 = _mm_div_pd(_mm_loadu_pd(&mcu_DW.q[2]), tmp_7);
  _mm_storeu_pd(&mcu_DW.q[2], tmp_7);

  // RateTransition: '<Root>/Rate Transition'
  over_inst = (getRTM()->isSampleHit(1));
  if (over_inst) {
    // RateTransition: '<Root>/Rate Transition'
    mcu_B.RateTransition = mcu_DW.RateTransition_Buffer0;
  }

  // End of RateTransition: '<Root>/Rate Transition'

  // MATLAB Function: '<Root>/handing the hard descent in case of low battery'
  if (mcu_B.RateTransition) {
    // SignalConversion generated from: '<Root>/Gain'
    rtb_F_cmd_0[0] = 9.3719834999999989;
    na = 1.0;
    q_err_idx_1 = 1.0;
    rtb_q_des_idx_1 = 0.0;
    q_err_idx_2 = 0.0;
    rtb_q_des_idx_2 = 0.0;
    q_err_idx_3 = 0.0;
    rtb_q_des_idx_3 = 0.0;
    rtb_q_ref_idx_3 = 0.0;
    theta[0] = 0.0;
    rtb_tau_ref[0] = 0.0;
    theta[1] = 0.0;
    rtb_tau_ref[1] = 0.0;
    theta[2] = 0.0;
    rtb_tau_ref[2] = 0.0;
  } else {
    // SignalConversion generated from: '<Root>/Gain'
    rtb_F_cmd_0[0] = *rtu_Bus_Cmd_F_des;
    na = rtu_Bus_Cmd_q_des[0];
    q_err_idx_1 = rtu_Bus_Cmd_q_ref[0];
    rtb_q_des_idx_1 = rtu_Bus_Cmd_q_des[1];
    q_err_idx_2 = rtu_Bus_Cmd_q_ref[1];
    rtb_q_des_idx_2 = rtu_Bus_Cmd_q_des[2];
    q_err_idx_3 = rtu_Bus_Cmd_q_ref[2];
    rtb_q_des_idx_3 = rtu_Bus_Cmd_q_des[3];
    rtb_q_ref_idx_3 = rtu_Bus_Cmd_q_ref[3];
    theta[0] = rtu_Bus_Cmd_Omega_ref[0];
    rtb_tau_ref[0] = rtu_Bus_Cmd_tau_ref[0];
    theta[1] = rtu_Bus_Cmd_Omega_ref[1];
    rtb_tau_ref[1] = rtu_Bus_Cmd_tau_ref[1];
    theta[2] = rtu_Bus_Cmd_Omega_ref[2];
    rtb_tau_ref[2] = rtu_Bus_Cmd_tau_ref[2];
  }

  // End of MATLAB Function: '<Root>/handing the hard descent in case of low battery' 

  // MATLAB Function: '<S4>/MATLAB Function' incorporates:
  //   Constant: '<S4>/Constant'
  //   Constant: '<S4>/Constant1'
  //   MATLAB Function: '<S3>/MATLAB Function'

  R_tmp = mcu_DW.q[0] * mcu_DW.q[0];
  R_tmp_0 = mcu_DW.q[1] * mcu_DW.q[1];
  R_tmp_1 = mcu_DW.q[2] * mcu_DW.q[2];
  R_tmp_2 = mcu_DW.q[3] * mcu_DW.q[3];
  R[0] = ((R_tmp + R_tmp_0) - R_tmp_1) - R_tmp_2;
  tmp_7 = _mm_set1_pd(mcu_DW.q[0]);
  tmp_8 = _mm_set1_pd(2.0);
  tmp_6 = _mm_mul_pd(_mm_add_pd(_mm_mul_pd(_mm_set1_pd(mcu_DW.q[1]),
    _mm_loadu_pd(&mcu_DW.q[2])), _mm_mul_pd(_mm_mul_pd(tmp_7, _mm_set_pd
    (mcu_DW.q[2], mcu_DW.q[3])), _mm_set_pd(-1.0, 1.0))), tmp_8);
  _mm_storeu_pd(&R[1], tmp_6);
  R[3] = (mcu_DW.q[1] * mcu_DW.q[2] - mcu_DW.q[0] * mcu_DW.q[3]) * 2.0;
  R_tmp -= R_tmp_0;
  R[4] = (R_tmp + R_tmp_1) - R_tmp_2;
  tmp_7 = _mm_mul_pd(_mm_add_pd(_mm_mul_pd(_mm_set_pd(mcu_DW.q[1], mcu_DW.q[2]),
    _mm_set1_pd(mcu_DW.q[3])), _mm_mul_pd(tmp_7, _mm_loadu_pd(&mcu_DW.q[1]))),
                     tmp_8);
  _mm_storeu_pd(&R[5], tmp_7);
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
  for (idx = 0; idx < 3; idx++) {
    for (i = 0; i < 3; i++) {
      S_tmp_tmp = 3 * idx + i;
      S_tmp[S_tmp_tmp] = R[3 * i + idx];
      Rdes_0[S_tmp_tmp] = (Rdes[3 * i + 1] * R[3 * idx + 1] + Rdes[3 * i] * R[3 *
                           idx]) + Rdes[3 * i + 2] * R[3 * idx + 2];
      S_tmp_0[S_tmp_tmp] = 0.0;
    }
  }

  for (idx = 0; idx < 3; idx++) {
    rtb_q_des_idx_3 = S_tmp_0[3 * idx];
    S_tmp_tmp = 3 * idx + 1;
    R_tmp_0 = S_tmp_0[S_tmp_tmp];
    S_tmp_tmp_0 = 3 * idx + 2;
    R_tmp_1 = S_tmp_0[S_tmp_tmp_0];
    for (i = 0; i < 3; i++) {
      R_tmp = Rdes[3 * idx + i];
      tmp_7 = _mm_add_pd(_mm_mul_pd(_mm_loadu_pd(&S_tmp[3 * i]), _mm_set1_pd
        (R_tmp)), _mm_set_pd(R_tmp_0, rtb_q_des_idx_3));
      _mm_storeu_pd(&tmp_5[0], tmp_7);
      rtb_q_des_idx_3 = tmp_5[0];
      R_tmp_0 = tmp_5[1];
      R_tmp_1 += S_tmp[3 * i + 2] * R_tmp;
    }

    S_tmp_0[S_tmp_tmp_0] = R_tmp_1;
    S_tmp_0[S_tmp_tmp] = R_tmp_0;
    S_tmp_0[3 * idx] = rtb_q_des_idx_3;
  }

  for (idx = 0; idx <= 6; idx += 2) {
    tmp_7 = _mm_loadu_pd(&Rdes_0[idx]);
    tmp_8 = _mm_loadu_pd(&S_tmp_0[idx]);
    _mm_storeu_pd(&R[idx], _mm_mul_pd(_mm_sub_pd(tmp_7, tmp_8), _mm_set1_pd(0.5)));
  }

  for (idx = 8; idx < 9; idx++) {
    R[idx] = (Rdes_0[idx] - S_tmp_0[idx]) * 0.5;
  }

  na = R[5];
  rtb_q_des_idx_1 = R[6];
  rtb_q_des_idx_2 = R[1];
  rtb_q_des_idx_3 = q_err_idx_1 * q_err_idx_1;
  R_tmp = q_err_idx_2 * q_err_idx_2;
  R_tmp_0 = q_err_idx_3 * q_err_idx_3;
  R_tmp_1 = rtb_q_ref_idx_3 * rtb_q_ref_idx_3;
  Rdes_0[0] = ((rtb_q_des_idx_3 + R_tmp) - R_tmp_0) - R_tmp_1;
  R_tmp_2 = q_err_idx_2 * q_err_idx_3;
  Rdes_tmp = q_err_idx_1 * rtb_q_ref_idx_3;
  Rdes_0[1] = (R_tmp_2 + Rdes_tmp) * 2.0;
  Rdes_tmp_0 = q_err_idx_2 * rtb_q_ref_idx_3;
  Rdes_tmp_1 = q_err_idx_1 * q_err_idx_3;
  Rdes_0[2] = (Rdes_tmp_0 - Rdes_tmp_1) * 2.0;
  Rdes_0[3] = (R_tmp_2 - Rdes_tmp) * 2.0;
  rtb_q_des_idx_3 -= R_tmp;
  Rdes_0[4] = (rtb_q_des_idx_3 + R_tmp_0) - R_tmp_1;
  R_tmp = q_err_idx_3 * rtb_q_ref_idx_3;
  R_tmp_2 = q_err_idx_1 * q_err_idx_2;
  Rdes_0[5] = (R_tmp + R_tmp_2) * 2.0;
  Rdes_0[6] = (Rdes_tmp_0 + Rdes_tmp_1) * 2.0;
  Rdes_0[7] = (R_tmp - R_tmp_2) * 2.0;
  Rdes_0[8] = (rtb_q_des_idx_3 - R_tmp_0) + R_tmp_1;
  for (idx = 0; idx < 3; idx++) {
    rtb_q_des_idx_3 = 0.0;
    R_tmp_0 = 0.0;
    R_tmp_1 = 0.0;
    for (i = 0; i < 3; i++) {
      R_tmp = Rdes_0[3 * idx + i];
      tmp_7 = _mm_add_pd(_mm_mul_pd(_mm_loadu_pd(&S_tmp[3 * i]), _mm_set1_pd
        (R_tmp)), _mm_set_pd(R_tmp_0, rtb_q_des_idx_3));
      _mm_storeu_pd(&tmp_5[0], tmp_7);
      rtb_q_des_idx_3 = tmp_5[0];
      R_tmp_0 = tmp_5[1];
      R_tmp_1 += S_tmp[3 * i + 2] * R_tmp;
    }

    S_tmp_0[3 * idx + 2] = R_tmp_1;
    S_tmp_0[3 * idx + 1] = R_tmp_0;
    S_tmp_0[3 * idx] = rtb_q_des_idx_3;
  }

  rtb_q_des_idx_3 = theta[1];
  q_err_idx_1 = theta[0];
  q_err_idx_2 = theta[2];
  for (idx = 0; idx <= 0; idx += 2) {
    tmp_7 = _mm_loadu_pd(&S_tmp_0[idx + 3]);
    tmp_8 = _mm_loadu_pd(&S_tmp_0[idx]);
    tmp_6 = _mm_loadu_pd(&S_tmp_0[idx + 6]);
    tmp_4 = _mm_loadu_pd(&rtb_Add[idx]);
    _mm_storeu_pd(&theta[idx], _mm_sub_pd(tmp_4, _mm_add_pd(_mm_add_pd
      (_mm_mul_pd(tmp_7, _mm_set1_pd(rtb_q_des_idx_3)), _mm_mul_pd(tmp_8,
      _mm_set1_pd(q_err_idx_1))), _mm_mul_pd(tmp_6, _mm_set1_pd(q_err_idx_2)))));
    tmp_7 = _mm_loadu_pd(&rtb_tau_ref[idx]);
    _mm_storeu_pd(&rtb_tau_ref_0[idx], _mm_sub_pd(tmp_7, _mm_add_pd(_mm_add_pd
      (_mm_mul_pd(_mm_loadu_pd(&rtCP_Constant_Value_m[idx + 3]), _mm_set1_pd
                  (rtb_q_des_idx_1)), _mm_mul_pd(_mm_loadu_pd
      (&rtCP_Constant_Value_m[idx]), _mm_set1_pd(na))), _mm_mul_pd(_mm_loadu_pd(
      &rtCP_Constant_Value_m[idx + 6]), _mm_set1_pd(rtb_q_des_idx_2)))));
    _mm_storeu_pd(&tmp_3[idx], _mm_set1_pd(0.0));
  }

  for (idx = 2; idx < 3; idx++) {
    _mm_storeu_pd(&tmp_5[0], _mm_sub_pd(_mm_set_pd(rtb_tau_ref[idx], rtb_Add[idx]),
      _mm_add_pd(_mm_add_pd(_mm_mul_pd(_mm_set_pd(rtCP_Constant_Value_m[idx + 3],
      S_tmp_0[idx + 3]), _mm_set_pd(rtb_q_des_idx_1, rtb_q_des_idx_3)),
      _mm_mul_pd(_mm_set_pd(rtCP_Constant_Value_m[idx], S_tmp_0[idx]),
                 _mm_set_pd(na, q_err_idx_1))), _mm_mul_pd(_mm_set_pd
      (rtCP_Constant_Value_m[idx + 6], S_tmp_0[idx + 6]), _mm_set_pd
      (rtb_q_des_idx_2, q_err_idx_2)))));
    theta[idx] = tmp_5[0];
    rtb_tau_ref_0[idx] = tmp_5[1];
  }

  R_tmp = 0.0;
  q_err_idx_1 = 0.0;
  q_err_idx_2 = 0.0;
  for (idx = 0; idx < 3; idx++) {
    _mm_storeu_pd(&tmp_5[0], _mm_add_pd(_mm_mul_pd(_mm_loadu_pd
      (&rtCP_Constant1_Value_o[3 * idx]), _mm_set1_pd(theta[idx])), _mm_set_pd
      (q_err_idx_1, R_tmp)));
    R_tmp = tmp_5[0];
    q_err_idx_1 = tmp_5[1];
    q_err_idx_2 += rtCP_Constant1_Value_o[3 * idx + 2] * theta[idx];
  }

  tmp_3[2] = q_err_idx_2;
  tmp_3[1] = q_err_idx_1;
  tmp_3[0] = R_tmp;
  tmp_7 = _mm_sub_pd(_mm_loadu_pd(&rtb_tau_ref_0[0]), _mm_loadu_pd(&tmp_3[0]));

  // SignalConversion generated from: '<Root>/Gain' incorporates:
  //   MATLAB Function: '<S4>/MATLAB Function'

  _mm_storeu_pd(&rtb_F_cmd_0[1], tmp_7);
  rtb_F_cmd_0[3] = rtb_tau_ref_0[2] - q_err_idx_2;

  // Gain: '<Root>/Gain'
  na = 0.0;
  rtb_q_des_idx_1 = 0.0;
  rtb_q_des_idx_2 = 0.0;
  rtb_q_des_idx_3 = 0.0;
  for (idx = 0; idx < 4; idx++) {
    i = idx << 2;
    tmp_7 = _mm_set1_pd(rtb_F_cmd_0[idx]);
    _mm_storeu_pd(&tmp_5[0], _mm_add_pd(_mm_mul_pd(_mm_loadu_pd
      (&rtCP_Gain_Gain[i]), tmp_7), _mm_set_pd(rtb_q_des_idx_1, na)));
    na = tmp_5[0];
    rtb_q_des_idx_1 = tmp_5[1];
    _mm_storeu_pd(&tmp_5[0], _mm_add_pd(_mm_mul_pd(_mm_loadu_pd
      (&rtCP_Gain_Gain[i + 2]), tmp_7), _mm_set_pd(rtb_q_des_idx_3,
      rtb_q_des_idx_2)));
    rtb_q_des_idx_2 = tmp_5[0];
    rtb_q_des_idx_3 = tmp_5[1];
  }

  // End of Gain: '<Root>/Gain'

  // Switch: '<Root>/Switch' incorporates:
  //   Abs: '<Root>/Abs'
  //   Constant: '<Root>/Constant2'
  //   MATLAB Function: '<Root>/MATLAB Function'
  //   Sqrt: '<Root>/Sqrt'

  if (mcu_DW.latched) {
    rty_rotor_cmd[0] = 0.0;
    rty_rotor_cmd[1] = 0.0;
    rty_rotor_cmd[2] = 0.0;
    rty_rotor_cmd[3] = 0.0;
  } else {
    rty_rotor_cmd[0] = std::sqrt(std::abs(na));
    rty_rotor_cmd[1] = std::sqrt(std::abs(rtb_q_des_idx_1));
    rty_rotor_cmd[2] = std::sqrt(std::abs(rtb_q_des_idx_2));
    rty_rotor_cmd[3] = std::sqrt(std::abs(rtb_q_des_idx_3));
  }

  // End of Switch: '<Root>/Switch'

  // Polyval: '<Root>/Polynomial'
  q_err_idx_1 = -2.9813898214245487E-13;
  q_err_idx_2 = -2.9813898214245487E-13;
  q_err_idx_3 = -2.9813898214245487E-13;
  rtb_q_ref_idx_3 = -2.9813898214245487E-13;
  for (idx = 0; idx < 2; idx++) {
    R_tmp = rtCP_Polynomial_Coefs[idx + 1];
    q_err_idx_1 = q_err_idx_1 * na + R_tmp;
    q_err_idx_2 = q_err_idx_2 * rtb_q_des_idx_1 + R_tmp;
    q_err_idx_3 = q_err_idx_3 * rtb_q_des_idx_2 + R_tmp;
    rtb_q_ref_idx_3 = rtb_q_ref_idx_3 * rtb_q_des_idx_3 + R_tmp;
  }

  // End of Polyval: '<Root>/Polynomial'

  // Saturate: '<Root>/Saturation'
  if (q_err_idx_1 > 100.0) {
    rty_throttle[0] = 100.0;
  } else if (q_err_idx_1 < 0.0) {
    rty_throttle[0] = 0.0;
  } else {
    rty_throttle[0] = q_err_idx_1;
  }

  if (q_err_idx_2 > 100.0) {
    rty_throttle[1] = 100.0;
  } else if (q_err_idx_2 < 0.0) {
    rty_throttle[1] = 0.0;
  } else {
    rty_throttle[1] = q_err_idx_2;
  }

  if (q_err_idx_3 > 100.0) {
    rty_throttle[2] = 100.0;
  } else if (q_err_idx_3 < 0.0) {
    rty_throttle[2] = 0.0;
  } else {
    rty_throttle[2] = q_err_idx_3;
  }

  if (rtb_q_ref_idx_3 > 100.0) {
    rty_throttle[3] = 100.0;
  } else if (rtb_q_ref_idx_3 < 0.0) {
    rty_throttle[3] = 0.0;
  } else {
    rty_throttle[3] = rtb_q_ref_idx_3;
  }

  // End of Saturate: '<Root>/Saturation'
  if (over_inst) {
    // MATLAB Function: '<Root>/volts to 12-bit counts'
    na = rt_roundd(*rtu_batt_count / 0.014652161172161169);

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
    *rty_led = mcu_DW.state;

    // End of MATLAB Function: '<Root>/MATLAB Function1'

    // Update for RateTransition: '<Root>/Rate Transition'
    mcu_DW.RateTransition_Buffer0 = mcu_DW.landed;
  }
}

time_T MCU::RT_MODEL_mcu_T::getClockTickH0() const
{
  return ( *(timingBridge->clockTickH[Timing.mdlref_GlobalTID[0]]) );
}

time_T MCU::RT_MODEL_mcu_T::getClockTick0() const
{
  return ( *((timingBridge->clockTick[Timing.mdlref_GlobalTID[0]])) );
}

time_T MCU::RT_MODEL_mcu_T::getClockTickH1() const
{
  return ( *(timingBridge->clockTickH[Timing.mdlref_GlobalTID[1]]) );
}

time_T MCU::RT_MODEL_mcu_T::getClockTick1() const
{
  return ( *((timingBridge->clockTick[Timing.mdlref_GlobalTID[1]])) );
}

time_T MCU::RT_MODEL_mcu_T::getT() const
{
  return (*(timingBridge->taskTime[0]));
}

const char_T* MCU::RT_MODEL_mcu_T::getErrorStatus() const
{
  return (*(errorStatus));
}

void MCU::RT_MODEL_mcu_T::setErrorStatus(const char_T* const aErrorStatus) const
{
  (*(errorStatus) = aErrorStatus);
}

boolean_T MCU::RT_MODEL_mcu_T::isSampleHit(int32_T sti) const
{
  return (timingBridge->taskCounter[Timing.mdlref_GlobalTID[sti]] == 0);
}

const char_T** MCU::RT_MODEL_mcu_T::getErrorStatusPointer() const
{
  return errorStatus;
}

void MCU::RT_MODEL_mcu_T::setErrorStatusPointer(const char_T
  ** aErrorStatusPointer)
{
  (errorStatus = aErrorStatusPointer);
}

// Constructor
MCU::MCU() :
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

// member function to initialize Real-Time model
void MCU::initializeRTM(const rtTimingBridge *timingBridge, int_T mdlref_TID0,
  int_T mdlref_TID1)
{
  setupGlobalTimingEngine(timingBridge, mdlref_TID0, mdlref_TID1);
}

// Private member function to set up the global timing engine
void MCU::setupGlobalTimingEngine(const rtTimingBridge *timingBridge, int_T
  mdlref_TID0, int_T mdlref_TID1)
{
  (&mcu_M)->Timing.mdlref_GlobalTID[0] = mdlref_TID0;
  (&mcu_M)->Timing.mdlref_GlobalTID[1] = mdlref_TID1;
  (&mcu_M)->timingBridge = timingBridge;
}

//
// File trailer for generated code.
//
// [EOF]
//
