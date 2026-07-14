//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: mcu_types.h
//
// Code generated for Simulink model 'mcu'.
//
// Model version                  : 1.276
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Jul 14 09:45:28 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef mcu_types_h_
#define mcu_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_Bus_IMU_
#define DEFINED_TYPEDEF_FOR_Bus_IMU_

struct Bus_IMU
{
  real_T imu_gyro[3];
  real_T imu_acc[3];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_Bus_Cmd_
#define DEFINED_TYPEDEF_FOR_Bus_Cmd_

struct Bus_Cmd
{
  real_T F_des;
  real_T q_des[4];
  real_T q_ref[4];
  real_T Omega_ref[3];
  real_T tau_ref[3];
  real_T q_ext[4];
  uint8_T estop;
  boolean_T ack;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_hwJ2RzfczGMa75g45KRiCB_
#define DEFINED_TYPEDEF_FOR_struct_hwJ2RzfczGMa75g45KRiCB_

struct struct_hwJ2RzfczGMa75g45KRiCB
{
  real_T omega_max;
  uint16_T debounce_N;
  boolean_T use_norm;
  real_T rearm_idle_frac;
  real_T F_rearm_idle;
  real_T batt_pin;
  real_T adc_bits;
  real_T batt_k;
  real_T batt_b;
  real_T batt_tau;
  real_T batt_alpha;
  real_T V_warn;
  real_T V_crit;
  real_T V_floor;
  real_T V_hyst;
  real_T hardfloor_thrust_frac;
  real_T m;
  real_T g;
};

#endif
#endif                                 // mcu_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
