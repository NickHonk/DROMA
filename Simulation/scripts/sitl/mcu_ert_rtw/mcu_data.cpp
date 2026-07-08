//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: mcu_data.cpp
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
  // Expression: quadcop.Gamma_inv
  //  Referenced by: '<Root>/Gain'

  { 220.45855379188708, 220.45855379188717, 220.45855379188714,
    220.45855379188708, -2268.6058512015852, 2268.6058512015852,
    2268.6058512015843, -2268.6058512015843, -2862.2682383911938,
    -2862.2682383911938, 2862.2682383911938, 2862.2682383911938,
    -33792.917004595831, 33792.917004595831, -33792.917004595831,
    33792.917004595831 },

  // Expression: controller.kR
  //  Referenced by: '<S4>/Constant'

  { 1.290268, 0.0, 0.0, 0.0, 1.0045, 0.0, 0.0, 0.0, 2.1638400000000004 },

  // Expression: controller.kOmega
  //  Referenced by: '<S4>/Constant1'

  { 0.130317068, 0.0, 0.0, 0.0, 0.1014545, 0.0, 0.0, 0.0, 0.21854784000000002 }
};

//
// File trailer for generated code.
//
// [EOF]
//
