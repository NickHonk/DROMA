//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: mcu_data.cpp
//
// Code generated for Simulink model 'mcu'.
//
// Model version                  : 1.268
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Jul 13 12:30:04 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "mcu.h"

// Invariant block signals (default storage)
const MCU::ConstB_mcu_T mcu_ConstB{
  {
    0.0,
    0.0,
    0.0
  }
  // '<Root>/Gain1'
};

// Constant parameters (default storage)
const MCU::ConstP_mcu_T mcu_ConstP{
  // Expression: quadcop.p_from_omega_sq
  //  Referenced by: '<Root>/Polynomial'

  { -2.9813898214245487E-13, 1.2315874872894876E-5, 8.4040477510594869 },

  // Expression: controller.kR
  //  Referenced by: '<S4>/Constant'

  { 1.290268, 0.0, 0.0, 0.0, 1.0045, 0.0, 0.0, 0.0, 2.1638400000000004 },

  // Expression: controller.kOmega
  //  Referenced by: '<S4>/Constant1'

  { 0.130317068, 0.0, 0.0, 0.0, 0.1014545, 0.0, 0.0, 0.0, 0.21854784000000002 },

  // Expression: quadcop.Gamma_inv
  //  Referenced by: '<Root>/Gain'

  { 132830.71758977618, 132830.71758977615, 132830.71758977615,
    132830.71758977612, -1.3668807036987864E+6, 1.3668807036987867E+6,
    1.3668807036987862E+6, -1.3668807036987862E+6, -1.7245742453651072E+6,
    -1.7245742453651079E+6, 1.7245742453651079E+6, 1.7245742453651074E+6,
    -9.9593013489803467E+6, 9.9593013489803486E+6, -9.9593013489803486E+6,
    9.9593013489803486E+6 }
};

//
// File trailer for generated code.
//
// [EOF]
//
