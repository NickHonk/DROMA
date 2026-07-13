//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: mcu.h
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
#ifndef mcu_h_
#define mcu_h_
#include "rtwtypes.h"
#include "mcu_types.h"
#include "model_reference_types.h"
#include <cstring>

// Class declaration for model mcu
class MCU final
{
  // public data and function members
 public:
  // Block signals for model 'mcu'
  struct B_mcu_T {
    boolean_T RateTransition;          // '<Root>/Rate Transition'
  };

  // Block states (default storage) for model 'mcu'
  struct DW_mcu_T {
    real_T q[4];                       // '<S3>/MATLAB Function'
    real_T Vf;                         // '<Root>/MATLAB Function1'
    uint16_T cnt;                      // '<Root>/MATLAB Function'
    uint8_T state;                     // '<Root>/MATLAB Function1'
    uint8_T src;                       // '<Root>/MATLAB Function'
    boolean_T RateTransition_Buffer0;  // '<Root>/Rate Transition'
    boolean_T q_not_empty;             // '<S3>/MATLAB Function'
    boolean_T Vf_not_empty;            // '<Root>/MATLAB Function1'
    boolean_T landed;                  // '<Root>/MATLAB Function1'
    boolean_T latched;                 // '<Root>/MATLAB Function'
    boolean_T ack_prev;                // '<Root>/MATLAB Function'
  };

  // Invariant block signals for model 'mcu'
  struct ConstB_mcu_h_T {
    real_T Gain1[3];                   // '<Root>/Gain1'
  };

  // Real-time Model Data Structure
  struct RT_MODEL_mcu_T {
    const char_T **errorStatus;
    const rtTimingBridge *timingBridge;

    //
    //  Timing:
    //  The following substructure contains information regarding
    //  the timing information for the model.

    struct {
      int_T mdlref_GlobalTID[2];
    } Timing;

    time_T getClockTickH0() const;
    time_T getClockTick0() const;
    time_T getClockTickH1() const;
    time_T getClockTick1() const;
    time_T getT() const;
    const char_T* getErrorStatus() const;
    void setErrorStatus(const char_T* const aErrorStatus) const;
    boolean_T isSampleHit(int32_T sti) const;
    const char_T** getErrorStatusPointer() const;
    void setErrorStatusPointer(const char_T** aErrorStatusPointer);
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

  // member function to initialize Real-Time model
  void initializeRTM(const rtTimingBridge *timingBridge, int_T mdlref_TID0,
                     int_T mdlref_TID1);

  // model step function
  void step(const real_T rtu_Bus_IMU_imu_gyro[3], const real_T
            rtu_Bus_IMU_imu_acc[3], const real_T *rtu_Bus_Cmd_F_des, const
            real_T rtu_Bus_Cmd_q_des[4], const real_T rtu_Bus_Cmd_q_ref[4],
            const real_T rtu_Bus_Cmd_Omega_ref[3], const real_T
            rtu_Bus_Cmd_tau_ref[3], const real_T rtu_Bus_Cmd_q_ext[4], const
            uint8_T *rtu_Bus_Cmd_estop, const boolean_T *rtu_Bus_Cmd_ack, const
            real_T *rtu_batt_count, real_T rty_rotor_cmd[4], uint8_T *rty_led,
            real_T rty_throttle[4]);

  // Constructor
  MCU();

  // Destructor
  ~MCU();

  // private data and function members
 private:
  // Block signals
  B_mcu_T mcu_B;

  // Block states
  DW_mcu_T mcu_DW;

  // Private member function to set up the global timing engine
  void setupGlobalTimingEngine(const rtTimingBridge *timingBridge, int_T
    mdlref_TID0, int_T mdlref_TID1);

  // Real-Time Model
  RT_MODEL_mcu_T mcu_M;
};

// Invariant block signals (default storage)
extern const MCU::ConstB_mcu_h_T mcu_ConstB;

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
//  '<S6>'   : 'mcu/volts to 12-bit counts'
//  '<S7>'   : 'mcu/Mahony filter/MATLAB Function'
//  '<S8>'   : 'mcu/geometrical attitude controller/MATLAB Function'

#endif                                 // mcu_h_

//
// File trailer for generated code.
//
// [EOF]
//
