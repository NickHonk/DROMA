//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: mcu_private.h
//
// Code generated for Simulink model 'mcu'.
//
// Model version                  : 1.246
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Jul  7 17:16:45 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: Intel->x86-64 (Windows64)
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef mcu_private_h_
#define mcu_private_h_
#include "rtwtypes.h"
#include "model_reference_types.h"
#include "mcu_types.h"

extern const real_T rtCP_pooled_Be5Qtm3k0DrI[16];
extern const real_T rtCP_pooled_kuPF4hu8gRzF[9];
extern const real_T rtCP_pooled_LSj82Bnj0HBr[9];

#define rtCP_Gain_Gain                 rtCP_pooled_Be5Qtm3k0DrI  // Expression: quadcop.Gamma_inv
                                                                 //  Referenced by: '<Root>/Gain'

#define rtCP_Constant_Value_m          rtCP_pooled_kuPF4hu8gRzF  // Expression: controller.kR
                                                                 //  Referenced by: '<S4>/Constant'

#define rtCP_Constant1_Value_o         rtCP_pooled_LSj82Bnj0HBr  // Expression: controller.kOmega
                                                                 //  Referenced by: '<S4>/Constant1'

#endif                                 // mcu_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
