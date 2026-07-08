/*
 * File: xil_interface.cpp
 *
 * SIL generated interface for code: "mcu"
 *
 */

#include "mcu.h"
#include "mcu_private.h"
#include "xil_interface.h"

/* Functions with a C call interface */
#ifdef __cplusplus

extern "C"
{

#endif

#include "xil_data_stream.h"
#ifdef __cplusplus

}

#endif

#include <string.h>

/* interface data */
real_T i_Bus_IMU_imu_gyro[3];

/* interface data */
real_T i_Bus_IMU_imu_acc[3];

/* interface data */
real_T i_Bus_Cmd_F_des;

/* interface data */
real_T i_Bus_Cmd_q_des[4];

/* interface data */
real_T i_Bus_Cmd_q_ref[4];

/* interface data */
real_T i_Bus_Cmd_Omega_ref[3];

/* interface data */
real_T i_Bus_Cmd_tau_ref[3];

/* interface data */
real_T i_Bus_Cmd_q_ext[4];

/* interface data */
uint8_T i_Bus_Cmd_estop;

/* interface data */
boolean_T i_Bus_Cmd_ack;

/* interface data */
real_T i_batt_count;

/* interface data */
real_T o_rotor_cmd[4];

/* interface data */
uint8_T o_led;

#define mdlref_TID0                    0
#define mdlref_TID1                    1

/* initialize error status as a null pointer */
static const char_T * rt_errorStatus_tgt_var = ((const char_T *) 0);
static const char_T ** rt_errorStatus = &rt_errorStatus_tgt_var;

/* define timingBridge */
static rtTimingBridge timingBridge_tgt_var;
static const rtTimingBridge * timingBridge = &timingBridge_tgt_var;

/* define Timing structure */
static struct {
  struct {
    uint32_T TID[2];
  } TaskCounters;
} XILTimingData;

static uint32_T InitialTaskCounterOffset[2];

/* XILFirstInitCond will be updated by xilInitialize and xilSystemInitialize */
static boolean_T XILFirstInitCond = 0;

/* Map model object name to dereferenced pointer to model object */
#define ModelMDLOBJ                    (*get_ModelMDLOBJ_ptr())

/* Accessor function to control initialization of model object */
MCU * get_ModelMDLOBJ_ptr(void)
{
  static MCU* xil_ModelMDLOBJ_ptr = new MCU();
  return xil_ModelMDLOBJ_ptr;
}

static XILIOData xil_fcnid0_task1_output_u[12];
static XILIOData xil_fcnid0_task1_y[3];
static XILIOData xil_fcnid0_init_u[3];
static XILIOData xil_fcnid0_init_y[3];

/* In-the-Loop Interface functions - see xil_interface.h */
XIL_INTERFACE_ERROR_CODE xilProcessParams(uint32_T xilFcnId)
{
  /* Single In-the-Loop Component */
  if (xilFcnId != 0) {
    return XIL_INTERFACE_UNKNOWN_FCNID;
  }

  return XIL_INTERFACE_SUCCESS;
}

XIL_INTERFACE_ERROR_CODE xilGetDataTypeInfo(void)
{
  {
    /* send response id code */
    MemUnit_T memUnitData = XIL_RESPONSE_TYPE_SIZE;
    if (xilWriteData(&memUnitData, sizeof(memUnitData)) !=
        XIL_DATA_STREAM_SUCCESS) {
      return XIL_INTERFACE_COMMS_FAILURE;
    }

    /* send type id */
    memUnitData = 0;
    if (xilWriteData(&memUnitData, sizeof(memUnitData)) !=
        XIL_DATA_STREAM_SUCCESS) {
      return XIL_INTERFACE_COMMS_FAILURE;
    }

    /* PIL_DOUBLE_SIZE should only be already defined for MathWorks testing */
#ifndef PIL_DOUBLE_SIZE
#define PIL_DOUBLE_SIZE                sizeof(double)
#endif

    /* send size in bytes */
    memUnitData = (MemUnit_T) PIL_DOUBLE_SIZE;

#ifndef HOST_WORD_ADDRESSABLE_TESTING

    /* convert MemUnits to bytes */
    memUnitData *= MEM_UNIT_BYTES;

#endif

    if (xilWriteData(&memUnitData, sizeof(memUnitData)) !=
        XIL_DATA_STREAM_SUCCESS) {
      return XIL_INTERFACE_COMMS_FAILURE;
    }
  }

  return XIL_INTERFACE_SUCCESS;
}

XIL_INTERFACE_ERROR_CODE xilInitialize(uint32_T xilFcnId)
{
  XIL_INTERFACE_ERROR_CODE errorCode = XIL_INTERFACE_SUCCESS;

  /* initialize XILTimingData */
  (void) memset((void *) &XILTimingData, 0, sizeof(XILTimingData));

  {
    static uint32_T *taskCounterPtrs;
    memcpy(XILTimingData.TaskCounters.TID, InitialTaskCounterOffset, sizeof
           (InitialTaskCounterOffset));
    taskCounterPtrs = &(XILTimingData.TaskCounters.TID[0]);
    timingBridge_tgt_var.taskCounter = taskCounterPtrs;
    timingBridge_tgt_var.clockTick = (uint32_T **) 0;
    timingBridge_tgt_var.clockTickH = (uint32_T **) 0;
    timingBridge_tgt_var.nTasks = 2;
  }

  timingBridge_tgt_var.firstInitCond = &XILFirstInitCond;

  /* firstInitCond should be true until after the first InitCond call */
  XILFirstInitCond = 1;

  {
  }

  /* initialize output storage owned by In-the-Loop */
  {
    {
      uint32_T elementIdx_0;
      for (elementIdx_0 = 0; elementIdx_0 < 4; elementIdx_0++) {
        o_rotor_cmd[elementIdx_0] = 0;
      }                                /* for */
    }
  }

  {
    o_led = 0;
  }

  /* Single In-the-Loop Component */
  if (xilFcnId == 0) {
    /* Call all Set class methods */
    ModelMDLOBJ.getRTM()->setErrorStatusPointer(rt_errorStatus);
    ModelMDLOBJ.initializeRTM(timingBridge, mdlref_TID0, mdlref_TID1);

    /* Call all Get class methods */
  } else {
    errorCode = XIL_INTERFACE_UNKNOWN_FCNID;
  }

  return errorCode;
}

XIL_INTERFACE_ERROR_CODE xilPause(uint32_T xilFcnId)
{
  XIL_INTERFACE_ERROR_CODE errorCode = XIL_INTERFACE_SUCCESS;
  if (xilFcnId == 0) {
    /* Nothing to do */
  } else {
    errorCode = XIL_INTERFACE_UNKNOWN_FCNID;
  }                                    /* if */

  return errorCode;
}

XIL_INTERFACE_ERROR_CODE xilSystemInitialize(uint32_T xilFcnId)
{
  XIL_INTERFACE_ERROR_CODE errorCode = XIL_INTERFACE_SUCCESS;

  /* Single In-the-Loop Component */
  if (xilFcnId == 0) {
    /* No Function to Call */
  } else {
    errorCode = XIL_INTERFACE_UNKNOWN_FCNID;
  }

  /* firstInitCond should be false after the first InitCond call */
  XILFirstInitCond = 0;
  return errorCode;
}

XIL_INTERFACE_ERROR_CODE xilSystemReset(uint32_T xilFcnId)
{
  XIL_INTERFACE_ERROR_CODE errorCode = XIL_INTERFACE_SUCCESS;

  /* Single In-the-Loop Component */
  if (xilFcnId == 0) {
    /* No Function to Call */
  } else {
    errorCode = XIL_INTERFACE_UNKNOWN_FCNID;
  }

  return errorCode;
}

XIL_INTERFACE_ERROR_CODE xilGetHostToTargetData(uint32_T xilFcnId,
  XIL_COMMAND_TYPE_ENUM xilCommandType, uint32_T xilCommandIdx, XILIOData
  ** xilIOData)
{
  XIL_INTERFACE_ERROR_CODE errorCode = XIL_INTERFACE_SUCCESS;
  *xilIOData = 0;

  /* Single In-the-Loop Component */
  if (xilFcnId != 0) {
    errorCode = XIL_INTERFACE_UNKNOWN_FCNID;
    return errorCode;
  }

  switch (xilCommandType) {
   case XIL_INITIALIZE_COMMAND:
    {
      static int initComplete = 0;
      if (!initComplete) {
        uint32_T tableIdx = 0;

        {
          void * dataAddress = (void *) &(InitialTaskCounterOffset[0]);
          xil_fcnid0_init_u[tableIdx].memUnitLength = 1 * sizeof(uint32_T);
          xil_fcnid0_init_u[tableIdx++].address = (MemUnit_T *) dataAddress;
        }

        {
          void * dataAddress = (void *) &(InitialTaskCounterOffset[1]);
          xil_fcnid0_init_u[tableIdx].memUnitLength = 1 * sizeof(uint32_T);
          xil_fcnid0_init_u[tableIdx++].address = (MemUnit_T *) dataAddress;
        }

        xil_fcnid0_init_u[tableIdx].memUnitLength = 0;
        xil_fcnid0_init_u[tableIdx++].address = (MemUnit_T *) 0;
        initComplete = 1;
      }                                /* if */

      *xilIOData = &xil_fcnid0_init_u[0];
      break;
    }

   case XIL_OUTPUT_COMMAND:
    {
      static int initComplete = 0;
      if (!initComplete) {
        uint32_T tableIdx = 0;

        {
          void * dataAddress = (void *) &(i_Bus_IMU_imu_gyro[0]);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 3 * sizeof(real_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_Bus_IMU_imu_acc[0]);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 3 * sizeof(real_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_Bus_Cmd_F_des);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 1 * sizeof(real_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_Bus_Cmd_q_des[0]);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 4 * sizeof(real_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_Bus_Cmd_q_ref[0]);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 4 * sizeof(real_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_Bus_Cmd_Omega_ref[0]);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 3 * sizeof(real_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_Bus_Cmd_tau_ref[0]);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 3 * sizeof(real_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_Bus_Cmd_q_ext[0]);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 4 * sizeof(real_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_Bus_Cmd_estop);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 1 * sizeof(uint8_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_Bus_Cmd_ack);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 1 * sizeof
            (boolean_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        {
          void * dataAddress = (void *) &(i_batt_count);
          xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 1 * sizeof(real_T);
          xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *)
            dataAddress;
        }

        xil_fcnid0_task1_output_u[tableIdx].memUnitLength = 0;
        xil_fcnid0_task1_output_u[tableIdx++].address = (MemUnit_T *) 0;
        initComplete = 1;
      }                                /* if */

      *xilIOData = &xil_fcnid0_task1_output_u[0];
      break;
    }

   default:
    errorCode = XIL_INTERFACE_UNKNOWN_TID;
    break;
  }

  UNUSED_PARAMETER(xilCommandIdx);
  return errorCode;
}

XIL_INTERFACE_ERROR_CODE xilGetTargetToHostPreData(uint32_T xilFcnId,
  XIL_COMMAND_TYPE_ENUM xilCommandType, uint32_T xilCommandIdx, XILIOData
  ** xilIOData, MemUnit_T responseId, uint32_T serverFcnId)
{
  XIL_INTERFACE_ERROR_CODE errorCode = XIL_INTERFACE_SUCCESS;
  *xilIOData = 0;
  if (xilFcnId != 0) {
    errorCode = XIL_INTERFACE_UNKNOWN_FCNID;
    return errorCode;
  }                                    /* if */

  errorCode = XIL_INTERFACE_UNKNOWN_TID;
  UNUSED_PARAMETER(xilCommandType);
  UNUSED_PARAMETER(xilCommandIdx);
  UNUSED_PARAMETER(responseId);
  UNUSED_PARAMETER(serverFcnId);
  return errorCode;
}

static void rate_scheduler(void)
{
  /* Compute which subrates run during the next base time step.  Subrates
   * are an integer multiple of the base rate counter.  Therefore, the subtask
   * counter is reset when it reaches its limit (zero means run).
   */

  /* sample time: [1s, 0s] */
  if (++XILTimingData.TaskCounters.TID[1] == 1000) {
    XILTimingData.TaskCounters.TID[1] = 0;
  }
}

XIL_INTERFACE_ERROR_CODE xilOutput(uint32_T xilFcnId, uint32_T xilTID)
{
  /* Single In-the-Loop Component */
  if (xilFcnId != 0) {
    return XIL_INTERFACE_UNKNOWN_FCNID;
  }

  switch (xilTID) {
   case 1:
    /* Call all Set class methods */
    ModelMDLOBJ.step(i_Bus_IMU_imu_gyro, i_Bus_IMU_imu_acc, &(i_Bus_Cmd_F_des),
                     i_Bus_Cmd_q_des, i_Bus_Cmd_q_ref, i_Bus_Cmd_Omega_ref,
                     i_Bus_Cmd_tau_ref, i_Bus_Cmd_q_ext, &(i_Bus_Cmd_estop),
                     &(i_Bus_Cmd_ack), &(i_batt_count), o_rotor_cmd, &(o_led));

    /* call scheduler */
    rate_scheduler();

    /* Call all Get class methods */
    break;

   default:
    return XIL_INTERFACE_UNKNOWN_TID;
  }

  return XIL_INTERFACE_SUCCESS;
}

XIL_INTERFACE_ERROR_CODE xilUpdate(uint32_T xilFcnId, uint32_T xilTID)
{
  /* Single In-the-Loop Component */
  if (xilFcnId != 0) {
    return XIL_INTERFACE_UNKNOWN_FCNID;
  }

  /* No Update Function */
  UNUSED_PARAMETER(xilTID);
  return XIL_INTERFACE_SUCCESS;
}

XIL_INTERFACE_ERROR_CODE xilGetTargetToHostData(uint32_T xilFcnId,
  XIL_COMMAND_TYPE_ENUM xilCommandType, uint32_T xilCommandIdx, XILIOData
  ** xilIOData, MemUnit_T responseId, uint32_T serverFcnId)
{
  XIL_INTERFACE_ERROR_CODE errorCode = XIL_INTERFACE_SUCCESS;

  /* Single In-the-Loop Component */
  *xilIOData = 0;
  if (xilFcnId != 0) {
    errorCode = XIL_INTERFACE_UNKNOWN_FCNID;
    return errorCode;
  }

  switch (xilCommandType) {
   case XIL_INITIALIZE_COMMAND:
    {
      static int initComplete = 0;
      if (!initComplete) {
        uint32_T tableIdx = 0;

        {
          void * dataAddress = (void *) &(o_rotor_cmd[0]);
          xil_fcnid0_init_y[tableIdx].memUnitLength = 4 * sizeof(real_T);
          xil_fcnid0_init_y[tableIdx++].address = (MemUnit_T *) dataAddress;
        }

        {
          void * dataAddress = (void *) &(o_led);
          xil_fcnid0_init_y[tableIdx].memUnitLength = 1 * sizeof(uint8_T);
          xil_fcnid0_init_y[tableIdx++].address = (MemUnit_T *) dataAddress;
        }

        xil_fcnid0_init_y[tableIdx].memUnitLength = 0;
        xil_fcnid0_init_y[tableIdx++].address = (MemUnit_T *) 0;
        initComplete = 1;
      }                                /* if */

      {
        if (xilWriteData(&responseId, sizeof(responseId)) !=
            XIL_DATA_STREAM_SUCCESS) {
          return XIL_INTERFACE_COMMS_FAILURE;
        }                              /* if */

        if (responseId == XIL_RESPONSE_CS_REQUEST_SERVICE) {
          if (xilWriteData((MemUnit_T *) &serverFcnId, sizeof(serverFcnId)) !=
              XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
          }                            /* if */
        }                              /* if */
      }

      *xilIOData = &xil_fcnid0_init_y[0];
      break;
    }

   case XIL_OUTPUT_COMMAND:
    {
      static int initComplete = 0;
      if (!initComplete) {
        uint32_T tableIdx = 0;

        {
          void * dataAddress = (void *) &(o_rotor_cmd[0]);
          xil_fcnid0_task1_y[tableIdx].memUnitLength = 4 * sizeof(real_T);
          xil_fcnid0_task1_y[tableIdx++].address = (MemUnit_T *) dataAddress;
        }

        {
          void * dataAddress = (void *) &(o_led);
          xil_fcnid0_task1_y[tableIdx].memUnitLength = 1 * sizeof(uint8_T);
          xil_fcnid0_task1_y[tableIdx++].address = (MemUnit_T *) dataAddress;
        }

        xil_fcnid0_task1_y[tableIdx].memUnitLength = 0;
        xil_fcnid0_task1_y[tableIdx++].address = (MemUnit_T *) 0;
        initComplete = 1;
      }                                /* if */

      {
        if (xilWriteData(&responseId, sizeof(responseId)) !=
            XIL_DATA_STREAM_SUCCESS) {
          return XIL_INTERFACE_COMMS_FAILURE;
        }                              /* if */

        if (responseId == XIL_RESPONSE_CS_REQUEST_SERVICE) {
          if (xilWriteData((MemUnit_T *) &serverFcnId, sizeof(serverFcnId)) !=
              XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
          }                            /* if */
        }                              /* if */
      }

      *xilIOData = &xil_fcnid0_task1_y[0];
      break;
    }

   default:
    errorCode = XIL_INTERFACE_UNKNOWN_TID;
    break;
  }

  UNUSED_PARAMETER(xilCommandIdx);
  UNUSED_PARAMETER(responseId);
  UNUSED_PARAMETER(serverFcnId);
  return errorCode;
}

XIL_INTERFACE_ERROR_CODE xilTerminate(uint32_T xilFcnId)
{
  if (xilFcnId != 0) {
    return XIL_INTERFACE_UNKNOWN_FCNID;
  }                                    /* if */

  /* Invoke any terminate Function */
  /* No Function to Call */

  /* Free Opaque Region Based Data */
  delete get_ModelMDLOBJ_ptr();
  return XIL_INTERFACE_SUCCESS;
}

XIL_INTERFACE_ERROR_CODE xilInitTargetData(void)
{
  return XIL_INTERFACE_SUCCESS;
}

XIL_INTERFACE_ERROR_CODE xilEnable(uint32_T xilFcnId, uint32_T xilTID)
{
  /* Single In-the-Loop Component */
  if (xilFcnId != 0) {
    return XIL_INTERFACE_UNKNOWN_FCNID;
  }

  UNUSED_PARAMETER(xilTID);

  /* No Enable Function - this function should never be called */
  return XIL_INTERFACE_UNKNOWN_TID;
}

XIL_INTERFACE_ERROR_CODE xilDisable(uint32_T xilFcnId, uint32_T xilTID)
{
  /* Single In-the-Loop Component */
  if (xilFcnId != 0) {
    return XIL_INTERFACE_UNKNOWN_FCNID;
  }

  UNUSED_PARAMETER(xilTID);

  /* No Disable Function - this function should never be called */
  return XIL_INTERFACE_UNKNOWN_TID;
}
