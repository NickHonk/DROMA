//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: mcu.cpp
//
// Code generated for Simulink model 'mcu'.
//
// Model version                  : 1.265
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Jul 13 11:54:04 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "mcu.h"
#include "rtwtypes.h"
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
  real_T R_tmp;
  real_T R_tmp_0;
  real_T R_tmp_1;
  real_T R_tmp_2;
  real_T R_tmp_3;
  real_T R_tmp_4;
  real_T R_tmp_5;
  real_T R_tmp_6;
  real_T nE;
  real_T na;
  real_T q_err_idx_1;
  real_T q_err_idx_2;
  real_T q_err_idx_3;
  real_T rtb_q_des_idx_1;
  real_T rtb_q_des_idx_2;
  real_T rtb_q_ref_idx_3;
  real_T tmp;
  real_T tmp_0;
  real_T tmp_1;
  real_T tmp_2;
  int32_T S_tmp_tmp;
  int32_T S_tmp_tmp_0;
  int32_T rtb_q_des_idx_0_tmp;
  int_T idx;
  boolean_T over_inst;
  static const int8_T b_b[3]{ 0, 0, 1 };

  // Sum: '<Root>/Add' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Sum: '<Root>/Subtract'

  rtb_Add[0] = (rtu_Bus_IMU_imu_gyro[0] - 0.17453292519943295) +
    mcu_ConstB.Gain1[0];
  rtb_Add[1] = (rtu_Bus_IMU_imu_gyro[1] - -0.17453292519943295) +
    mcu_ConstB.Gain1[1];
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
    R_tmp_1 = mcu_DW.q[0] * mcu_DW.q[0];
    q_err_idx_1 = mcu_DW.q[1] * mcu_DW.q[1];
    q_err_idx_2 = mcu_DW.q[2] * mcu_DW.q[2];
    q_err_idx_3 = mcu_DW.q[3] * mcu_DW.q[3];
    S_tmp[0] = ((R_tmp_1 + q_err_idx_1) - q_err_idx_2) - q_err_idx_3;
    rtb_q_ref_idx_3 = mcu_DW.q[1] * mcu_DW.q[2];
    R_tmp_2 = mcu_DW.q[0] * mcu_DW.q[3];
    S_tmp[3] = (rtb_q_ref_idx_3 + R_tmp_2) * 2.0;
    R_tmp_3 = mcu_DW.q[1] * mcu_DW.q[3];
    R_tmp_4 = mcu_DW.q[0] * mcu_DW.q[2];
    S_tmp[6] = (R_tmp_3 - R_tmp_4) * 2.0;
    S_tmp[1] = (rtb_q_ref_idx_3 - R_tmp_2) * 2.0;
    R_tmp_1 -= q_err_idx_1;
    S_tmp[4] = (R_tmp_1 + q_err_idx_2) - q_err_idx_3;
    q_err_idx_1 = mcu_DW.q[2] * mcu_DW.q[3];
    rtb_q_ref_idx_3 = mcu_DW.q[0] * mcu_DW.q[1];
    S_tmp[7] = (q_err_idx_1 + rtb_q_ref_idx_3) * 2.0;
    S_tmp[2] = (R_tmp_3 + R_tmp_4) * 2.0;
    S_tmp[5] = (q_err_idx_1 - rtb_q_ref_idx_3) * 2.0;
    S_tmp[8] = (R_tmp_1 - q_err_idx_2) + q_err_idx_3;
    rtb_Add[0] = rtu_Bus_IMU_imu_acc[0] / na;
    rtb_Add[1] = rtu_Bus_IMU_imu_acc[1] / na;
    rtb_Add[2] = rtu_Bus_IMU_imu_acc[2] / na;
    na = 0.0;
    rtb_q_des_idx_1 = 0.0;
    rtb_q_des_idx_2 = 0.0;
    for (idx = 0; idx < 3; idx++) {
      rtb_q_des_idx_0_tmp = b_b[idx];
      na += S_tmp[3 * idx] * static_cast<real_T>(rtb_q_des_idx_0_tmp);
      rtb_q_des_idx_1 += S_tmp[3 * idx + 1] * static_cast<real_T>
        (rtb_q_des_idx_0_tmp);
      rtb_q_des_idx_2 += S_tmp[3 * idx + 2] * static_cast<real_T>
        (rtb_q_des_idx_0_tmp);
    }

    theta[0] = rtb_Add[1] * rtb_q_des_idx_2 - rtb_q_des_idx_1 * rtb_Add[2];
    theta[1] = na * rtb_Add[2] - rtb_Add[0] * rtb_q_des_idx_2;
    theta[2] = rtb_Add[0] * rtb_q_des_idx_1 - na * rtb_Add[1];
  } else {
    theta[0] = 0.0;
    theta[1] = 0.0;
    theta[2] = 0.0;
  }

  nE = norm_bQtbKOLy(rtu_Bus_Cmd_q_ext);
  if (nE > 0.5) {
    na = rtu_Bus_Cmd_q_ext[0] / nE;
    rtb_q_des_idx_1 = rtu_Bus_Cmd_q_ext[1] / nE;
    rtb_q_des_idx_2 = rtu_Bus_Cmd_q_ext[2] / nE;
    nE = rtu_Bus_Cmd_q_ext[3] / nE;
    q_err_idx_1 = ((mcu_DW.q[0] * rtb_q_des_idx_1 + na * -mcu_DW.q[1]) +
                   -mcu_DW.q[2] * nE) - rtb_q_des_idx_2 * -mcu_DW.q[3];
    q_err_idx_2 = ((mcu_DW.q[0] * rtb_q_des_idx_2 - -mcu_DW.q[1] * nE) + na *
                   -mcu_DW.q[2]) + rtb_q_des_idx_1 * -mcu_DW.q[3];
    q_err_idx_3 = ((mcu_DW.q[0] * nE + -mcu_DW.q[1] * rtb_q_des_idx_2) -
                   rtb_q_des_idx_1 * -mcu_DW.q[2]) + na * -mcu_DW.q[3];
    if (((mcu_DW.q[0] * na - -mcu_DW.q[1] * rtb_q_des_idx_1) - -mcu_DW.q[2] *
         rtb_q_des_idx_2) - -mcu_DW.q[3] * nE < 0.0) {
      q_err_idx_1 = -q_err_idx_1;
      q_err_idx_2 = -q_err_idx_2;
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
  nE = norm_XZxrXx9N(theta);
  if (nE > 1.0E-9) {
    na = nE / 2.0;
    q_err_idx_1 = std::sin(na);
    na = std::cos(na);
    rtb_q_des_idx_1 = theta[0] / nE * q_err_idx_1;
    rtb_q_des_idx_2 = theta[1] / nE * q_err_idx_1;
    nE = theta[2] / nE * q_err_idx_1;
  } else {
    na = 1.0;
    rtb_q_des_idx_1 = 0.5 * theta[0];
    rtb_q_des_idx_2 = 0.5 * theta[1];
    nE = 0.5 * theta[2];
  }

  R_tmp_1 = mcu_DW.q[0];
  q_err_idx_1 = mcu_DW.q[1];
  q_err_idx_2 = mcu_DW.q[2];
  q_err_idx_3 = mcu_DW.q[3];
  rtb_q_ref_idx_3 = mcu_DW.q[0];
  R_tmp_2 = mcu_DW.q[1];
  R_tmp_3 = mcu_DW.q[2];
  R_tmp_4 = mcu_DW.q[3];
  R_tmp = mcu_DW.q[0];
  R_tmp_0 = mcu_DW.q[1];
  R_tmp_5 = mcu_DW.q[2];
  R_tmp_6 = mcu_DW.q[3];
  tmp = mcu_DW.q[0];
  tmp_0 = mcu_DW.q[1];
  tmp_1 = mcu_DW.q[2];
  tmp_2 = mcu_DW.q[3];
  mcu_DW.q[0] = ((R_tmp_1 * na - q_err_idx_1 * rtb_q_des_idx_1) - q_err_idx_2 *
                 rtb_q_des_idx_2) - q_err_idx_3 * nE;
  mcu_DW.q[1] = ((rtb_q_ref_idx_3 * rtb_q_des_idx_1 + na * R_tmp_2) + R_tmp_3 *
                 nE) - rtb_q_des_idx_2 * R_tmp_4;
  mcu_DW.q[2] = ((R_tmp * rtb_q_des_idx_2 - R_tmp_0 * nE) + na * R_tmp_5) +
    rtb_q_des_idx_1 * R_tmp_6;
  mcu_DW.q[3] = ((tmp * nE + tmp_0 * rtb_q_des_idx_2) - rtb_q_des_idx_1 * tmp_1)
    + na * tmp_2;
  R_tmp_1 = norm_bQtbKOLy(mcu_DW.q);
  mcu_DW.q[0] /= R_tmp_1;
  mcu_DW.q[1] /= R_tmp_1;
  mcu_DW.q[2] /= R_tmp_1;
  mcu_DW.q[3] /= R_tmp_1;

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
    nE = 0.0;
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
    nE = rtu_Bus_Cmd_q_des[3];
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

  R_tmp_1 = mcu_DW.q[0] * mcu_DW.q[0];
  R_tmp_2 = mcu_DW.q[1] * mcu_DW.q[1];
  R_tmp_3 = mcu_DW.q[2] * mcu_DW.q[2];
  R_tmp_4 = mcu_DW.q[3] * mcu_DW.q[3];
  R[0] = ((R_tmp_1 + R_tmp_2) - R_tmp_3) - R_tmp_4;
  R_tmp = mcu_DW.q[1] * mcu_DW.q[2];
  R_tmp_0 = mcu_DW.q[0] * mcu_DW.q[3];
  R[1] = (R_tmp + R_tmp_0) * 2.0;
  R_tmp_5 = mcu_DW.q[1] * mcu_DW.q[3];
  R_tmp_6 = mcu_DW.q[0] * mcu_DW.q[2];
  R[2] = (R_tmp_5 - R_tmp_6) * 2.0;
  R[3] = (R_tmp - R_tmp_0) * 2.0;
  R_tmp_1 -= R_tmp_2;
  R[4] = (R_tmp_1 + R_tmp_3) - R_tmp_4;
  R_tmp_2 = mcu_DW.q[2] * mcu_DW.q[3];
  R_tmp = mcu_DW.q[0] * mcu_DW.q[1];
  R[5] = (R_tmp_2 + R_tmp) * 2.0;
  R[6] = (R_tmp_5 + R_tmp_6) * 2.0;
  R[7] = (R_tmp_2 - R_tmp) * 2.0;
  R[8] = (R_tmp_1 - R_tmp_3) + R_tmp_4;
  R_tmp_1 = na * na;
  R_tmp_2 = rtb_q_des_idx_1 * rtb_q_des_idx_1;
  R_tmp_3 = rtb_q_des_idx_2 * rtb_q_des_idx_2;
  R_tmp_4 = nE * nE;
  Rdes[0] = ((R_tmp_1 + R_tmp_2) - R_tmp_3) - R_tmp_4;
  R_tmp = rtb_q_des_idx_1 * rtb_q_des_idx_2;
  R_tmp_0 = na * nE;
  Rdes[1] = (R_tmp + R_tmp_0) * 2.0;
  R_tmp_5 = rtb_q_des_idx_1 * nE;
  R_tmp_6 = na * rtb_q_des_idx_2;
  Rdes[2] = (R_tmp_5 - R_tmp_6) * 2.0;
  Rdes[3] = (R_tmp - R_tmp_0) * 2.0;
  R_tmp_1 -= R_tmp_2;
  Rdes[4] = (R_tmp_1 + R_tmp_3) - R_tmp_4;
  R_tmp_2 = rtb_q_des_idx_2 * nE;
  R_tmp = na * rtb_q_des_idx_1;
  Rdes[5] = (R_tmp_2 + R_tmp) * 2.0;
  Rdes[6] = (R_tmp_5 + R_tmp_6) * 2.0;
  Rdes[7] = (R_tmp_2 - R_tmp) * 2.0;
  Rdes[8] = (R_tmp_1 - R_tmp_3) + R_tmp_4;
  for (idx = 0; idx < 3; idx++) {
    for (rtb_q_des_idx_0_tmp = 0; rtb_q_des_idx_0_tmp < 3; rtb_q_des_idx_0_tmp++)
    {
      S_tmp_tmp = 3 * idx + rtb_q_des_idx_0_tmp;
      S_tmp[S_tmp_tmp] = R[3 * rtb_q_des_idx_0_tmp + idx];
      Rdes_0[S_tmp_tmp] = (Rdes[3 * rtb_q_des_idx_0_tmp + 1] * R[3 * idx + 1] +
                           Rdes[3 * rtb_q_des_idx_0_tmp] * R[3 * idx]) + Rdes[3 *
        rtb_q_des_idx_0_tmp + 2] * R[3 * idx + 2];
      S_tmp_0[S_tmp_tmp] = 0.0;
    }
  }

  for (idx = 0; idx < 3; idx++) {
    nE = S_tmp_0[3 * idx];
    S_tmp_tmp = 3 * idx + 1;
    R_tmp_2 = S_tmp_0[S_tmp_tmp];
    S_tmp_tmp_0 = 3 * idx + 2;
    R_tmp_3 = S_tmp_0[S_tmp_tmp_0];
    for (rtb_q_des_idx_0_tmp = 0; rtb_q_des_idx_0_tmp < 3; rtb_q_des_idx_0_tmp++)
    {
      R_tmp_1 = Rdes[3 * idx + rtb_q_des_idx_0_tmp];
      nE += S_tmp[3 * rtb_q_des_idx_0_tmp] * R_tmp_1;
      R_tmp_2 += S_tmp[3 * rtb_q_des_idx_0_tmp + 1] * R_tmp_1;
      R_tmp_3 += S_tmp[3 * rtb_q_des_idx_0_tmp + 2] * R_tmp_1;
    }

    S_tmp_0[S_tmp_tmp_0] = R_tmp_3;
    S_tmp_0[S_tmp_tmp] = R_tmp_2;
    S_tmp_0[3 * idx] = nE;
  }

  for (idx = 0; idx < 9; idx++) {
    R[idx] = (Rdes_0[idx] - S_tmp_0[idx]) * 0.5;
  }

  na = R[5];
  rtb_q_des_idx_1 = R[6];
  rtb_q_des_idx_2 = R[1];
  nE = q_err_idx_1 * q_err_idx_1;
  R_tmp_1 = q_err_idx_2 * q_err_idx_2;
  R_tmp_2 = q_err_idx_3 * q_err_idx_3;
  R_tmp_3 = rtb_q_ref_idx_3 * rtb_q_ref_idx_3;
  Rdes_0[0] = ((nE + R_tmp_1) - R_tmp_2) - R_tmp_3;
  R_tmp_4 = q_err_idx_2 * q_err_idx_3;
  R_tmp = q_err_idx_1 * rtb_q_ref_idx_3;
  Rdes_0[1] = (R_tmp_4 + R_tmp) * 2.0;
  R_tmp_0 = q_err_idx_2 * rtb_q_ref_idx_3;
  R_tmp_5 = q_err_idx_1 * q_err_idx_3;
  Rdes_0[2] = (R_tmp_0 - R_tmp_5) * 2.0;
  Rdes_0[3] = (R_tmp_4 - R_tmp) * 2.0;
  nE -= R_tmp_1;
  Rdes_0[4] = (nE + R_tmp_2) - R_tmp_3;
  R_tmp_1 = q_err_idx_3 * rtb_q_ref_idx_3;
  R_tmp_4 = q_err_idx_1 * q_err_idx_2;
  Rdes_0[5] = (R_tmp_1 + R_tmp_4) * 2.0;
  Rdes_0[6] = (R_tmp_0 + R_tmp_5) * 2.0;
  Rdes_0[7] = (R_tmp_1 - R_tmp_4) * 2.0;
  Rdes_0[8] = (nE - R_tmp_2) + R_tmp_3;
  for (idx = 0; idx < 3; idx++) {
    nE = 0.0;
    R_tmp_2 = 0.0;
    R_tmp_3 = 0.0;
    for (rtb_q_des_idx_0_tmp = 0; rtb_q_des_idx_0_tmp < 3; rtb_q_des_idx_0_tmp++)
    {
      R_tmp_1 = Rdes_0[3 * idx + rtb_q_des_idx_0_tmp];
      nE += S_tmp[3 * rtb_q_des_idx_0_tmp] * R_tmp_1;
      R_tmp_2 += S_tmp[3 * rtb_q_des_idx_0_tmp + 1] * R_tmp_1;
      R_tmp_3 += S_tmp[3 * rtb_q_des_idx_0_tmp + 2] * R_tmp_1;
    }

    S_tmp_0[3 * idx + 2] = R_tmp_3;
    S_tmp_0[3 * idx + 1] = R_tmp_2;
    S_tmp_0[3 * idx] = nE;
  }

  nE = theta[1];
  rtb_q_ref_idx_3 = theta[0];
  R_tmp_2 = theta[2];
  R_tmp_1 = 0.0;
  q_err_idx_1 = 0.0;
  q_err_idx_2 = 0.0;
  for (idx = 0; idx < 3; idx++) {
    theta[idx] = rtb_Add[idx] - ((S_tmp_0[idx + 3] * nE + S_tmp_0[idx] *
      rtb_q_ref_idx_3) + S_tmp_0[idx + 6] * R_tmp_2);
    rtb_tau_ref_0[idx] = rtb_tau_ref[idx] - ((rtCP_Constant_Value_m[idx + 3] *
      rtb_q_des_idx_1 + rtCP_Constant_Value_m[idx] * na) +
      rtCP_Constant_Value_m[idx + 6] * rtb_q_des_idx_2);
    q_err_idx_3 = theta[idx];
    R_tmp_1 += rtCP_Constant1_Value_o[3 * idx] * q_err_idx_3;
    q_err_idx_1 += rtCP_Constant1_Value_o[3 * idx + 1] * q_err_idx_3;
    q_err_idx_2 += rtCP_Constant1_Value_o[3 * idx + 2] * q_err_idx_3;
  }

  // SignalConversion generated from: '<Root>/Gain' incorporates:
  //   MATLAB Function: '<S4>/MATLAB Function'

  rtb_F_cmd_0[1] = rtb_tau_ref_0[0] - R_tmp_1;
  rtb_F_cmd_0[2] = rtb_tau_ref_0[1] - q_err_idx_1;
  rtb_F_cmd_0[3] = rtb_tau_ref_0[2] - q_err_idx_2;

  // Gain: '<Root>/Gain'
  na = 0.0;
  rtb_q_des_idx_1 = 0.0;
  rtb_q_des_idx_2 = 0.0;
  nE = 0.0;
  for (idx = 0; idx < 4; idx++) {
    R_tmp_1 = rtb_F_cmd_0[idx];
    rtb_q_des_idx_0_tmp = idx << 2;
    na += rtCP_Gain_Gain[rtb_q_des_idx_0_tmp] * R_tmp_1;
    rtb_q_des_idx_1 += rtCP_Gain_Gain[rtb_q_des_idx_0_tmp + 1] * R_tmp_1;
    rtb_q_des_idx_2 += rtCP_Gain_Gain[rtb_q_des_idx_0_tmp + 2] * R_tmp_1;
    nE += rtCP_Gain_Gain[rtb_q_des_idx_0_tmp + 3] * R_tmp_1;
  }

  // End of Gain: '<Root>/Gain'

  // Switch: '<Root>/Switch' incorporates:
  //   Abs: '<Root>/Abs'
  //   Constant: '<Root>/Constant2'
  //   Constant: '<Root>/Constant4'
  //   MATLAB Function: '<Root>/MATLAB Function'
  //   Sqrt: '<Root>/Sqrt'
  //   Switch: '<Root>/Switch1'

  if (mcu_DW.latched) {
    rty_rotor_cmd[0] = 0.0;
    rty_throttle[0] = 0.0;
    rty_rotor_cmd[1] = 0.0;
    rty_throttle[1] = 0.0;
    rty_rotor_cmd[2] = 0.0;
    rty_throttle[2] = 0.0;
    rty_rotor_cmd[3] = 0.0;
    rty_throttle[3] = 0.0;
  } else {
    rty_rotor_cmd[0] = std::sqrt(std::abs(na));

    // Polyval: '<Root>/Polynomial' incorporates:
    //   Abs: '<Root>/Abs'
    //   Sqrt: '<Root>/Sqrt'

    q_err_idx_1 = -2.9813898214245487E-13;
    rty_rotor_cmd[1] = std::sqrt(std::abs(rtb_q_des_idx_1));

    // Polyval: '<Root>/Polynomial' incorporates:
    //   Abs: '<Root>/Abs'
    //   Sqrt: '<Root>/Sqrt'

    q_err_idx_2 = -2.9813898214245487E-13;
    rty_rotor_cmd[2] = std::sqrt(std::abs(rtb_q_des_idx_2));

    // Polyval: '<Root>/Polynomial' incorporates:
    //   Abs: '<Root>/Abs'
    //   Sqrt: '<Root>/Sqrt'

    q_err_idx_3 = -2.9813898214245487E-13;
    rty_rotor_cmd[3] = std::sqrt(std::abs(nE));

    // Polyval: '<Root>/Polynomial' incorporates:
    //   Abs: '<Root>/Abs'
    //   Sqrt: '<Root>/Sqrt'

    rtb_q_ref_idx_3 = -2.9813898214245487E-13;
    for (idx = 0; idx < 2; idx++) {
      R_tmp_1 = rtCP_Polynomial_Coefs[idx + 1];
      q_err_idx_1 = q_err_idx_1 * na + R_tmp_1;
      q_err_idx_2 = q_err_idx_2 * rtb_q_des_idx_1 + R_tmp_1;
      q_err_idx_3 = q_err_idx_3 * rtb_q_des_idx_2 + R_tmp_1;
      rtb_q_ref_idx_3 = rtb_q_ref_idx_3 * nE + R_tmp_1;
    }

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
  }

  // End of Switch: '<Root>/Switch'
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
