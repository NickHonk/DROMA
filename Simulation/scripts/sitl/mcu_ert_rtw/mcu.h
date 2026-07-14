//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: mcu.h
//
// Code generated for Simulink model 'mcu'.
//
// Model version                  : 1.281
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Jul 14 12:04:56 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: Intel->x86-64 (Windows64)
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef mcu_h_
#define mcu_h_
#include "rtwtypes.h"
#include "mcu_types.h"

// Class declaration for model mcu
class MCU final
{
  // public data and function members
 public:
  // Block signals (default storage)
  struct B_mcu_T {
    boolean_T RateTransition;          // '<Root>/Rate Transition'
  };

  // Block states (default storage) for system '<Root>'
  struct DW_mcu_T {
    real_T q[4];                       // '<S3>/MATLAB Function'
    real_T Vf;                         // '<Root>/MATLAB Function1'
    uint16_T cnt;                      // '<Root>/MATLAB Function'
    uint8_T state;                     // '<Root>/MATLAB Function1'
    boolean_T RateTransition_Buffer0;  // '<Root>/Rate Transition'
    boolean_T q_not_empty;             // '<S3>/MATLAB Function'
    boolean_T Vf_not_empty;            // '<Root>/MATLAB Function1'
    boolean_T landed;                  // '<Root>/MATLAB Function1'
    boolean_T latched;                 // '<Root>/MATLAB Function'
    boolean_T ack_prev;                // '<Root>/MATLAB Function'
  };

  // Constant parameters (default storage)
  struct ConstP_mcu_T {
    // Expression: quadcop.p_from_omega_sq
    //  Referenced by: '<Root>/Polynomial'

    real_T Polynomial_Coefs[3];

    // Expression: controller.kR
    //  Referenced by: '<S4>/Constant'

    real_T Constant_Value_m[9];

    // Expression: controller.kOmega
    //  Referenced by: '<S4>/Constant1'

    real_T Constant1_Value_o[9];

    // Expression: quadcop.Gamma_inv
    //  Referenced by: '<Root>/Gain'

    real_T Gain_Gain[16];
  };

  // External inputs (root inport signals with default storage)
  struct ExtU_mcu_T {
    Bus_IMU Bus_IMU_k;                 // '<Root>/Bus_IMU'
    Bus_Cmd Bus_Cmd_l;                 // '<Root>/Bus_Cmd'
    real_T batt_count;                 // '<Root>/batt_count'
    boolean_T btn_ack;                 // '<Root>/btn_ack'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY_mcu_T {
    real_T rotor_cmd[4];               // '<Root>/rotor_cmd'
    uint8_T led;                       // '<Root>/led'
    real_T throttle[4];                // '<Root>/throttle'
  };

  // Real-time Model Data Structure
  struct RT_MODEL_mcu_T {
    const char_T * volatile errorStatus;

    //
    //  Timing:
    //  The following substructure contains information regarding
    //  the timing information for the model.

    struct {
      struct {
        uint16_T TID[2];
      } TaskCounters;
    } Timing;

    const char_T* getErrorStatus() const;
    void setErrorStatus(const char_T* const volatile aErrorStatus);
  };

  // Copy Constructor
  MCU(MCU const&) = delete;

  // Assignment Operator
  MCU& operator= (MCU const&) & = delete;

  // Move Constructor
  MCU(MCU &&) = delete;

  // Move Assignment Operator
  MCU& operator= (MCU &&) = delete;

  // Real-Time Model get method
  MCU::RT_MODEL_mcu_T * getRTM();

  // Root inports set method
  void setExternalInputs(const ExtU_mcu_T *pExtU_mcu_T)
  {
    mcu_U = *pExtU_mcu_T;
  }

  // Root outports get method
  const ExtY_mcu_T &getExternalOutputs() const
  {
    return mcu_Y;
  }

  // model initialize function
  static void initialize();

  // model step function
  void step();

  // model terminate function
  static void terminate();

  // Constructor
  MCU();

  // Destructor
  ~MCU();

  // private data and function members
 private:
  // External inputs
  ExtU_mcu_T mcu_U;

  // External outputs
  ExtY_mcu_T mcu_Y;

  // Block signals
  B_mcu_T mcu_B;

  // Block states
  DW_mcu_T mcu_DW;

  // private member function(s) for subsystem '<Root>'
  real_T mcu_norm(const real_T x[3]);
  real_T mcu_norm_j(const real_T x[4]);

  // Real-Time Model
  RT_MODEL_mcu_T mcu_M;
};

// Constant parameters (default storage)
extern const MCU::ConstP_mcu_T mcu_ConstP;

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'mcu'
//  '<S1>'   : 'mcu/MATLAB Function'
//  '<S2>'   : 'mcu/MATLAB Function1'
//  '<S3>'   : 'mcu/Mahony filter'
//  '<S4>'   : 'mcu/geometrical attitude controller'
//  '<S5>'   : 'mcu/handing the hard descent in case of low battery'
//  '<S6>'   : 'mcu/Mahony filter/MATLAB Function'
//  '<S7>'   : 'mcu/geometrical attitude controller/MATLAB Function'

#endif                                 // mcu_h_

//
// File trailer for generated code.
//
// [EOF]
//
