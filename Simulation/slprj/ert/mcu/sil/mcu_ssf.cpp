/*
 * mcu_ssf.cpp
 *
 * Automatically generated s-function with I/O interface for:
 * Component: mcu
 * Component Simulink Path: mcu
 * Simulation Mode: SIL
 *
 */

#define S_FUNCTION_NAME                mcu_ssf
#define S_FUNCTION_LEVEL               2
#if !defined(RTW_GENERATED_S_FUNCTION)
#define RTW_GENERATED_S_FUNCTION
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "simstruc.h"
#include "simtarget/slMdlrefSimTargetCoreHeaders.h"
#include "simtarget/slMdlrefSimTargetInstrumentationHeaders.h"
#include "fixedpoint.h"
#include "coder/connectivity_core/xilutils/xilutils.h"
#include "coder/simulinkcoder/xilutils_sl/xilutils_sl.h"
#include "rtiostream_utils.h"
#include "coder/connectivity/xilcomms_rtiostream/xilcomms_rtiostream.h"
#include "coder/connectivity/XILHostAppSvc/XILHostAppSvc_CInterface.h"
#include "messages/slMessagesSfcnBridge.h"
#include "mwstringutil.h"
#include "coder/connectivity/CoderAssumpHostAppSvc/CoderAssumpHostAppSvc_CInterface.h"

static real_T rtInf;
static real_T rtMinusInf;

/* Response case labels */
enum ResponseIDs {
  RESPONSE_ERROR = 0,
  RESPONSE_OUTPUT_PRE_DATA = 1,
  RESPONSE_OUTPUT_DATA = 2,
  RESPONSE_PRINTF = 3,
  RESPONSE_FOPEN = 4,
  RESPONSE_FPRINTF = 5,
  RESPONSE_SIGNAL_RAISED = 6,
  RESPONSE_EXCEPTION = 7
};

typedef struct {
  FILE ** Fd;
  mwSize size;
  int32_T fidOffset;
} targetIOFd_T;

typedef enum {
  XIL_INIT_COMMAND = 0,
  XIL_INITIALIZE_COMMAND,
  XIL_SYSTEM_INITIALIZE_COMMAND,
  XIL_OUTPUT_COMMAND,
  XIL_TERMINATE_COMMAND,
  XIL_ENABLE_COMMAND,
  XIL_DISABLE_COMMAND,
  XIL_CONST_OUTPUT_COMMAND,
  XIL_PROCESS_PARAMS_COMMAND,
  XIL_CLIENT_SERVER_COMMAND,
  XIL_SHUTDOWN_COMMAND,
  XIL_UPDATE_COMMAND,
  XIL_SYSTEM_RESET_COMMAND,
  XIL_PAUSE_COMMAND
}
  XIL_COMMAND_TYPE_ENUM;

static RegMdlInfo rtMdlInfo_mcu[1] = { "", MDL_INFO_ID_GLOBAL_RTW_CONSTRUCT, 0,
  0, NULL };

static char * getSimulinkBlockPath(SimStruct *S)
{
  char * simulinkBlockPath = NULL;
  const char * origBlockPath = ssGetPath(S);
  const char * searchString = "TmpSFcnForModelReference_";
  const char * searchPtr;
  size_t origLength, searchAndNameLength, copyAmount;
  origLength = strlen(origBlockPath);
  searchPtr = strstr(origBlockPath, searchString);
  if (searchPtr == NULL) {
    return simulinkBlockPath;
  }

  searchAndNameLength = strlen(searchPtr);
  copyAmount = origLength - searchAndNameLength - 1;
  simulinkBlockPath = (char *) mxCalloc((mwSize) (origLength + 1), sizeof(char));
  simulinkBlockPath = strncpy(simulinkBlockPath, origBlockPath, copyAmount);
  simulinkBlockPath[copyAmount] = '\0';
  return simulinkBlockPath;
}

static uint32_T computeTaskTickOffset(SimStruct *S, int_T tidIdx)
{
  uint32_T nOffsetTicks = 0;

  /* No task counter required for base rate */
  if (tidIdx>0) {
    time_T stepSize = ssGetFixedStepSize(S);
    time_T period = ssGetSampleTime(S, tidIdx);
    time_T simulationTime = ssGetT(S);
    time_T offset = period*floor(simulationTime/period);
    nOffsetTicks = round((simulationTime-offset)/stepSize);
  }                                    /* if */

  return nOffsetTicks;
}

static void callStopHookAndFreeSFcnMemory(SimStruct *S);
static void mdlTerminate(SimStruct *S);
static void xilThrowException(void* pXILUtils);

/* grow the buffer for target I/O Fd array
 * targetIOFd->Fd is NULL on failure */
static void growTargetIOFd(SimStruct *S, targetIOFd_T * IOFd, mwSize
  requiredSize)
{
  if (IOFd->size < requiredSize) {
    IOFd->Fd = (FILE**)mxRealloc(IOFd->Fd, requiredSize * sizeof(FILE*));
    if (IOFd->Fd == NULL) {
      ssSetErrorStatus( S,"growTargetIOFd: mxRealloc failed.");
    } else {
      mexMakeMemoryPersistent(IOFd->Fd);
      IOFd->size = requiredSize;
    }                                  /* if */
  }                                    /* if */
}

static void closeAndFreeTargetIOFd(SimStruct *S)
{
  int i;
  if (ssGetPWork(S) != NULL) {
    targetIOFd_T * targetIOFdPtr = (targetIOFd_T *) ssGetPWorkValue(S, 3);
    if (targetIOFdPtr != NULL) {
      if (targetIOFdPtr->Fd != NULL) {
        for (i=0; i<targetIOFdPtr->size; i++) {
          if (targetIOFdPtr->Fd[i] != NULL) {
            fclose(targetIOFdPtr->Fd[i]);
          }                            /* if */
        }                              /* for */

        mxFree(targetIOFdPtr->Fd);
      }                                /* if */

      mxFree(targetIOFdPtr);
    }                                  /* if */

    ssSetPWorkValue(S, 3, NULL);
  }                                    /* if */
}

/* receive one packet of data and dispatch to owning application */
static boolean_T recvData(SimStruct *S, void* pComms)
{
  int * pCommErrorOccurred = (int *) ssGetPWorkValue(S, 4);
  void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
  if (pCommErrorOccurred == NULL) {
    ssSetErrorStatus( S,"pCommErrorOccurred is NULL.");
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  if (pXILUtils == NULL) {
    ssSetErrorStatus( S,"pXILUtils is NULL.");
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  *pCommErrorOccurred = (xilCommsRun(pComms, pXILUtils) !=
    XILCOMMS_RTIOSTREAM_SUCCESS);
  return (*pCommErrorOccurred?XILHOSTAPPSVC_ERROR:XILHOSTAPPSVC_SUCCESS);
}

/* send data via xil comms */
static boolean_T sendData(SimStruct *S, void* pXILService, XIL_IOBuffer_T
  * IOBuffer, mwSize sendSize)
{
  int * pCommErrorOccurred = (int *) ssGetPWorkValue(S, 4);
  if (pCommErrorOccurred == NULL) {
    ssSetErrorStatus( S,"pCommErrorOccurred is NULL.");
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  *pCommErrorOccurred = (xilHostAppSvcSend(pXILService, IOBuffer->data, sendSize)
    != XILHOSTAPPSVC_SUCCESS);
  return (*pCommErrorOccurred?XILHOSTAPPSVC_ERROR:XILHOSTAPPSVC_SUCCESS);
}

/* implements command dispatch */
static boolean_T commandDispatch(SimStruct *S, XIL_IOBuffer_T* IOBuffer, mwSize
  dataOutSize)
{
  void * pXILService = (void *) ssGetPWorkValue(S, 9);
  if (pXILService == NULL) {
    ssSetErrorStatus( S,"pXILService is NULL!");
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  /* send the data */
  if (sendData(S, pXILService, IOBuffer, dataOutSize) != XILHOSTAPPSVC_SUCCESS)
  {
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  return XILHOSTAPPSVC_SUCCESS;
}

/* implements command response */
static boolean_T commandResponse(SimStruct *S, XIL_IOBuffer_T* IOBuffer,
  XILCommandResponseType* commandType)
{
  void * pXILService = (void *) ssGetPWorkValue(S, 9);
  if (pXILService == NULL) {
    ssSetErrorStatus( S,"pXILService is NULL!");
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  {
    /* receive the response data */
    uint8_T COMMAND_COMPLETE = 0;
    void * pComms = (void *) ssGetPWorkValue(S, 7);
    if (pComms == NULL) {
      ssSetErrorStatus( S,"pComms is NULL!");
      return XILHOSTAPPSVC_ERROR;
    }                                  /* if */

    while (!COMMAND_COMPLETE) {
      xilHostAppSvcSetIsResponseComplete(pXILService, 0);
      if (recvData(S, pComms) != XILHOSTAPPSVC_SUCCESS) {
        return XILHOSTAPPSVC_ERROR;
      }                                /* if */

      COMMAND_COMPLETE = xilHostAppSvcGetIsResponseComplete(pXILService);
    }                                  /* while */

    /* determine command response type */
    *commandType = (XILCommandResponseType) COMMAND_COMPLETE;
    IOBuffer->nextMemUnit = (void*)IOBuffer->data;
    IOBuffer->bufferDataSize = xilHostAppSvcGetPayloadSizeForOneStep(pXILService)*
      1;
    IOBuffer->readData = ((IOBuffer->bufferDataSize>0)?true:false);
    return XILHOSTAPPSVC_SUCCESS;
  }
}

boolean_T xilReadData(SimStruct *S, uint8_T *dst, size_t numElements,
                      mem_unit_type_T dataTypeId)
{
  XIL_IOBuffer_T* IOBufferPtr = NULL;
  void* pMemUnitTransformer = NULL;
  void* pXILService = NULL;
  IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
  pMemUnitTransformer = (void *) ssGetPWorkValue(S, 11);
  pXILService = (void *) ssGetPWorkValue(S, 9);
  return xilReadDataInternal(dst, numElements, dataTypeId, true, IOBufferPtr,
    pMemUnitTransformer, 1, pXILService);
}

boolean_T xilWriteData(SimStruct *S, const uint8_T *src, size_t numElements,
  mem_unit_type_T dataTypeId)
{
  XIL_IOBuffer_T* IOBufferPtr = NULL;
  void* pMemUnitTransformer = NULL;
  void* pXILService = NULL;
  IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
  pMemUnitTransformer = (void *) ssGetPWorkValue(S, 11);
  pXILService = (void *) ssGetPWorkValue(S, 9);
  return xilWriteDataInternal(src, numElements, dataTypeId, true, IOBufferPtr,
    pMemUnitTransformer, 1, pXILService);
}

static boolean_T xilWriteStringData(SimStruct *S, uint8_T *srcPtr, size_t
  numElements)
{
  size_t maxBytesConsumed;
  void* dstPtr = NULL;
  XIL_IOBuffer_T* IOBufferPtr = NULL;
  IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
  maxBytesConsumed = numElements * sizeof(char_T);
  dstPtr = IOBufferPtr->nextMemUnit;
  suWriteSILStringInput((char *)dstPtr, (int32_T)numElements, srcPtr);
  IOBufferPtr->nextMemUnit = (uint8_T*)IOBufferPtr->nextMemUnit +
    (maxBytesConsumed/sizeof(uint8_T));
  IOBufferPtr->bufferDataSize = (size_t) ((uint8_T*)IOBufferPtr->nextMemUnit -
    (uint8_T*)IOBufferPtr->data);
  return XILHOSTAPPSVC_SUCCESS;
}

static boolean_T xilReadStringData(SimStruct *S, uint8_T *dstPtr, size_t
  numElements)
{
  size_t maxBytesConsumed;
  void* srcPtr = NULL;
  XIL_IOBuffer_T* IOBufferPtr = NULL;
  IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
  maxBytesConsumed = numElements * sizeof(char_T);
  srcPtr = IOBufferPtr->nextMemUnit;
  suWriteSILStringOutput(dstPtr, (char *)srcPtr, (int32_T)numElements);
  IOBufferPtr->nextMemUnit = (uint8_T*)IOBufferPtr->nextMemUnit +
    (maxBytesConsumed/sizeof(uint8_T));
  IOBufferPtr->bufferDataSize -= maxBytesConsumed;
  IOBufferPtr->readData = ((IOBufferPtr->bufferDataSize>0)?true:false);
  return XILHOSTAPPSVC_SUCCESS;
}

static void callStopHookAndFreeSFcnMemory(SimStruct *S)
{
  closeAndFreeTargetIOFd(S);
  if (ssGetPWork(S) != NULL) {
    int * pCommErrorOccurred = (int *) ssGetPWorkValue(S, 4);
    int * pIsXILApplicationStarted = (int *) ssGetPWorkValue(S, 5);
    if ((pIsXILApplicationStarted != NULL) && (*pIsXILApplicationStarted == 1))
    {
      {
        void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
        if (pXILUtils) {
          mxArray *rhs[3];
          char * simulinkBlockPath = getSimulinkBlockPath(S);
          if (simulinkBlockPath == NULL) {
            ssSetErrorStatus(S,
                             "ModelBlock SIL/PIL unexpected error: getSimulinkBlockPath returned NULL pointer. Check search string was found in ssGetPath.\n");
            return;
          }

          rhs[ 0 ] = mxCreateString(
            "@coder.connectivity.SimulinkInterface.getSILPILInterface");
          rhs[ 1 ] = mxCreateDoubleScalar( 1 );
          rhs[ 2 ] = mxCreateString(simulinkBlockPath);
          xilUtilsCallMATLAB(pXILUtils, 0, NULL, 3, rhs,
                             "rtw.pil.SILPILInterface.sfunctionPILStopHook");
          mxFree((void *) simulinkBlockPath);
        }                              /* if */
      }
    }                                  /* if */

    if (pIsXILApplicationStarted != NULL) {
      *pIsXILApplicationStarted = 0;
    }                                  /* if */
  }                                    /* if */

  if (ssGetPWork(S) != NULL) {
    XIL_IOBuffer_T* IOBufferPtr;
    XIL_RtIOStreamData_T * rtIOStreamDataPtr = (XIL_RtIOStreamData_T *)
      ssGetPWorkValue(S, 0);
    SIL_DEBUGGING_DATA_T * silDebuggingDataPtr = (SIL_DEBUGGING_DATA_T *)
      ssGetPWorkValue(S, 2);
    if (rtIOStreamDataPtr != NULL) {
      {
        int errorStatus = rtIOStreamUnloadLib(&rtIOStreamDataPtr->libH);
        if (errorStatus) {
          ssSetErrorStatus( S,"rtIOStreamUnloadLib failed.");
        }                              /* if */
      }

      mxFree(rtIOStreamDataPtr->lib);
      mxDestroyArray(rtIOStreamDataPtr->MATLABObject);
      mxFree(rtIOStreamDataPtr);
      ssSetPWorkValue(S, 0, NULL);
    }                                  /* if */

    if (silDebuggingDataPtr != NULL) {
      mxFree(silDebuggingDataPtr->componentBlockPath);
      mxFree(silDebuggingDataPtr->SILPILInterfaceFcnStr);
      mxFree(silDebuggingDataPtr);
      ssSetPWorkValue(S, 2, NULL);
    }                                  /* if */

    IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr != NULL) {
      mxFree(IOBufferPtr->data);
      mxFree(IOBufferPtr);
      ssSetPWorkValue(S, 1, NULL);
    }                                  /* if */

    closeAndFreeTargetIOFd(S);
    if (ssGetPWork(S) != NULL) {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      void * pComms = (void *) ssGetPWorkValue(S, 7);
      void * pXILService = (void *) ssGetPWorkValue(S, 9);
      void * pCoderAssumptionsApp = (void *) ssGetPWorkValue(S, 10);
      if (pCoderAssumptionsApp != NULL) {
        coderAssumpHostAppSvcDestroy(pCoderAssumptionsApp);
        ssSetPWorkValue(S, 10, NULL);
      }                                /* if */

      if (pXILService != NULL) {
        xilHostAppSvcDestroy(pXILService);
        ssSetPWorkValue(S, 9, NULL);
      }                                /* if */

      if (pComms != NULL) {
        xilCommsDestroy(pComms);
        ssSetPWorkValue(S, 7, NULL);
      }                                /* if */
    }                                  /* if */
  }                                    /* if */
}

static boolean_T processResponseError(SimStruct * S)
{
  uint8_T errorId;
  if (xilReadData(S, &errorId, 1, MEM_UNIT_UINT8_TYPE) != XILHOSTAPPSVC_SUCCESS)
  {
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  if (errorId) {
    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      mxArray * rhs[ 2 ];
      rhs[0] = mxCreateString("PIL:pilverification:PILError");
      rhs[1] = mxCreateDoubleScalar(errorId);
      xilUtilsHandleError(pXILUtils, 2 , rhs );
      mxDestroyArray(rhs[0]);
      mxDestroyArray(rhs[1]);
      return XILHOSTAPPSVC_ERROR;
    }
  }                                    /* if */

  return XILHOSTAPPSVC_SUCCESS;
}

static boolean_T processResponsePrintf(SimStruct * S)
{
  const int TARGET_IO_SUCCESS = 0;
  uint8_T PRINTF_ERROR;
  uint16_T PRINTF_SIZE;

  {
    {
      uint8_T * simDataMemUnitPtr;
      simDataMemUnitPtr = (uint8_T *) &PRINTF_ERROR;

      {
        size_t num_elements = 1;

        {
          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                          MEM_UNIT_UINT8_TYPE) != XILHOSTAPPSVC_SUCCESS) {
            return XILHOSTAPPSVC_ERROR;
          }                            /* if */
        }
      }
    }
  }

  {
    {
      uint8_T * simDataMemUnitPtr;
      simDataMemUnitPtr = (uint8_T *) &PRINTF_SIZE;

      {
        size_t num_elements = 1;

        {
          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                          MEM_UNIT_UINT16_TYPE) != XILHOSTAPPSVC_SUCCESS) {
            return XILHOSTAPPSVC_ERROR;
          }                            /* if */
        }
      }
    }
  }

  if (PRINTF_ERROR != TARGET_IO_SUCCESS) {
    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      mxArray * rhs[ 2 ];
      rhs[0] = mxCreateString("PIL:pil:TargetIOError");
      rhs[1] = mxCreateDoubleScalar(PRINTF_ERROR);
      xilUtilsHandleError(pXILUtils, 2 , rhs );
      mxDestroyArray(rhs[0]);
      mxDestroyArray(rhs[1]);
      return XILHOSTAPPSVC_ERROR;
    }
  } else {
    uint8_T *pPrintBuff;
    pPrintBuff = (uint8_T *) mxCalloc(PRINTF_SIZE, sizeof(uint8_T));
    if (pPrintBuff != NULL) {
      if (xilReadData(S, &(pPrintBuff[0]), PRINTF_SIZE, MEM_UNIT_UINT8_TYPE) !=
          XILHOSTAPPSVC_SUCCESS) {
        return XILHOSTAPPSVC_ERROR;
      }                                /* if */

      if (pPrintBuff[PRINTF_SIZE-1] == '\0') {
        mexPrintf("%s", pPrintBuff);
        mexEvalString("pause(0)");
      }                                /* if */

      mxFree(pPrintBuff);
    }                                  /* if */
  }                                    /* if */

  return XILHOSTAPPSVC_SUCCESS;
}

static boolean_T processResponseFopen(SimStruct * S)
{
  uint16_T FOPEN_FID;
  uint16_T FOPEN_NAME_SIZE;
  targetIOFd_T *targetIOFdPtr;

  {
    {
      uint8_T * simDataMemUnitPtr;
      simDataMemUnitPtr = (uint8_T *) &FOPEN_FID;

      {
        size_t num_elements = 1;

        {
          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                          MEM_UNIT_UINT16_TYPE) != XILHOSTAPPSVC_SUCCESS) {
            return XILHOSTAPPSVC_ERROR;
          }                            /* if */
        }
      }
    }
  }

  {
    {
      uint8_T * simDataMemUnitPtr;
      simDataMemUnitPtr = (uint8_T *) &FOPEN_NAME_SIZE;

      {
        size_t num_elements = 1;

        {
          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                          MEM_UNIT_UINT16_TYPE) != XILHOSTAPPSVC_SUCCESS) {
            return XILHOSTAPPSVC_ERROR;
          }                            /* if */
        }
      }
    }
  }

  targetIOFdPtr = (targetIOFd_T *) ssGetPWorkValue(S, 3);
  if (targetIOFdPtr != NULL) {
    /* check fid increments by 1 */
    if (targetIOFdPtr->fidOffset + 1 == FOPEN_FID) {
      targetIOFdPtr->fidOffset = FOPEN_FID;
      growTargetIOFd(S, targetIOFdPtr, targetIOFdPtr->fidOffset + 1);
      if (targetIOFdPtr->Fd != NULL) {
        uint8_T *pFopenBuff;
        targetIOFdPtr->Fd[targetIOFdPtr->fidOffset] = NULL;
        pFopenBuff = (uint8_T *) mxCalloc(FOPEN_NAME_SIZE, sizeof(uint8_T));
        if (pFopenBuff != NULL) {
          if (xilReadData(S, &(pFopenBuff[0]), FOPEN_NAME_SIZE,
                          MEM_UNIT_UINT8_TYPE) != XILHOSTAPPSVC_SUCCESS) {
            return XILHOSTAPPSVC_ERROR;
          }                            /* if */

          if (pFopenBuff[FOPEN_NAME_SIZE-1] == '\0') {
            FILE * tmpFd = NULL;
            tmpFd = fopen((char *) pFopenBuff,"w");
            if (tmpFd != NULL) {
              /* save the file descriptor */
              targetIOFdPtr->Fd[targetIOFdPtr->fidOffset] = tmpFd;
            } else {
              {
                void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                mxArray * rhs[ 2 ];
                rhs[0] = mxCreateString("PIL:pil:TargetIOFopenError");
                rhs[1] = mxCreateString((char *) pFopenBuff);
                xilUtilsHandleError(pXILUtils, 2 , rhs );
                mxDestroyArray(rhs[0]);
                mxDestroyArray(rhs[1]);
                return XILHOSTAPPSVC_ERROR;
              }
            }                          /* if */
          }                            /* if */

          mxFree(pFopenBuff);
        }                              /* if */
      }                                /* if */
    } else {
      {
        void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
        mxArray * rhs[ 2 ];
        rhs[0] = mxCreateString("PIL:pil:TargetIOFopenFidError");
        rhs[1] = mxCreateDoubleScalar(FOPEN_FID);
        xilUtilsHandleError(pXILUtils, 2 , rhs );
        mxDestroyArray(rhs[0]);
        mxDestroyArray(rhs[1]);
        return XILHOSTAPPSVC_ERROR;
      }
    }                                  /* if */
  }                                    /* if */

  return XILHOSTAPPSVC_SUCCESS;
}

static boolean_T processResponseFprintf(SimStruct * S)
{
  const int TARGET_IO_SUCCESS = 0;
  uint8_T FPRINTF_ERROR;
  uint16_T FPRINTF_FID;
  uint16_T FPRINTF_SIZE;

  {
    {
      uint8_T * simDataMemUnitPtr;
      simDataMemUnitPtr = (uint8_T *) &FPRINTF_ERROR;

      {
        size_t num_elements = 1;

        {
          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                          MEM_UNIT_UINT8_TYPE) != XILHOSTAPPSVC_SUCCESS) {
            return XILHOSTAPPSVC_ERROR;
          }                            /* if */
        }
      }
    }
  }

  {
    {
      uint8_T * simDataMemUnitPtr;
      simDataMemUnitPtr = (uint8_T *) &FPRINTF_FID;

      {
        size_t num_elements = 1;

        {
          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                          MEM_UNIT_UINT16_TYPE) != XILHOSTAPPSVC_SUCCESS) {
            return XILHOSTAPPSVC_ERROR;
          }                            /* if */
        }
      }
    }
  }

  {
    {
      uint8_T * simDataMemUnitPtr;
      simDataMemUnitPtr = (uint8_T *) &FPRINTF_SIZE;

      {
        size_t num_elements = 1;

        {
          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                          MEM_UNIT_UINT16_TYPE) != XILHOSTAPPSVC_SUCCESS) {
            return XILHOSTAPPSVC_ERROR;
          }                            /* if */
        }
      }
    }
  }

  if (FPRINTF_ERROR != TARGET_IO_SUCCESS) {
    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      mxArray * rhs[ 2 ];
      rhs[0] = mxCreateString("PIL:pil:TargetIOError");
      rhs[1] = mxCreateDoubleScalar(FPRINTF_ERROR);
      xilUtilsHandleError(pXILUtils, 2 , rhs );
      mxDestroyArray(rhs[0]);
      mxDestroyArray(rhs[1]);
      return XILHOSTAPPSVC_ERROR;
    }
  } else {
    targetIOFd_T * targetIOFdPtr = (targetIOFd_T *) ssGetPWorkValue(S, 3);
    if (targetIOFdPtr != NULL) {
      if (targetIOFdPtr->size > FPRINTF_FID) {
        if (targetIOFdPtr->Fd[FPRINTF_FID] != NULL) {
          uint8_T *pFprintfBuff;
          pFprintfBuff = (uint8_T *) mxCalloc(FPRINTF_SIZE, sizeof(uint8_T));
          if (pFprintfBuff != NULL) {
            if (xilReadData(S, &(pFprintfBuff[0]), FPRINTF_SIZE,
                            MEM_UNIT_UINT8_TYPE) != XILHOSTAPPSVC_SUCCESS) {
              return XILHOSTAPPSVC_ERROR;
            }                          /* if */

            if (pFprintfBuff[FPRINTF_SIZE-1] == '\0') {
              fprintf(targetIOFdPtr->Fd[FPRINTF_FID], "%s", pFprintfBuff);
            }                          /* if */

            mxFree(pFprintfBuff);
          }                            /* if */
        }                              /* if */
      }                                /* if */
    }                                  /* if */
  }                                    /* if */

  return XILHOSTAPPSVC_SUCCESS;
}

static boolean_T processResponseSignalRaised(SimStruct * S)
{
  const char *signalStr[6] = { "SIGFPE", "SIGILL", "SIGABRT", "SIGSEGV",
    "SIGEXIT", "Unknown Signal" };

  uint8_T errorId;
  if (xilReadData(S, &errorId, 1, MEM_UNIT_UINT8_TYPE) != XILHOSTAPPSVC_SUCCESS)
  {
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  if (strcmp(signalStr[errorId], "SIGEXIT")==0) {
    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      mxArray * rhs[ 2 ];
      rhs[0] = mxCreateString("Connectivity:target:CaughtTargetExitRequest");
      rhs[1] = mxCreateDoubleScalar(ssGetT(S));
      xilUtilsHandleError(pXILUtils, 2 , rhs );
      mxDestroyArray(rhs[0]);
      mxDestroyArray(rhs[1]);
      return XILHOSTAPPSVC_ERROR;
    }
  } else {
    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      mxArray * rhs[ 3 ];
      rhs[0] = mxCreateString("Connectivity:target:CaughtTargetSignalWithStderr");
      rhs[1] = mxCreateString(signalStr[errorId]);
      rhs[2] = mxCreateDoubleScalar(ssGetT(S));
      xilUtilsHandleError(pXILUtils, 3 , rhs );
      mxDestroyArray(rhs[0]);
      mxDestroyArray(rhs[1]);
      mxDestroyArray(rhs[2]);
      return XILHOSTAPPSVC_ERROR;
    }
  }                                    /* if */
}

static boolean_T processResponseException(SimStruct * S)
{
  uint32_T exceptionLength;
  if (xilReadData(S, (uint8_T *)&exceptionLength, 1, MEM_UNIT_UINT32_TYPE) !=
      XILHOSTAPPSVC_SUCCESS) {
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  uint8_T *exception = (uint8_T *) mxCalloc(exceptionLength+1, sizeof(uint8_T));
  if (xilReadData(S, &(exception[0]), exceptionLength, MEM_UNIT_UINT8_TYPE) !=
      XILHOSTAPPSVC_SUCCESS) {
    return XILHOSTAPPSVC_ERROR;
  }                                    /* if */

  if (exceptionLength) {
    uint32_T exceptionTypeLength;
    if (xilReadData(S, (uint8_T *)&exceptionTypeLength, 1, MEM_UNIT_UINT32_TYPE)
        != XILHOSTAPPSVC_SUCCESS) {
      return XILHOSTAPPSVC_ERROR;
    }                                  /* if */

    uint8_T *exceptionType = (uint8_T *) mxCalloc(exceptionTypeLength+1, sizeof
      (uint8_T));
    if (xilReadData(S, &(exceptionType[0]), exceptionTypeLength,
                    MEM_UNIT_UINT8_TYPE) != XILHOSTAPPSVC_SUCCESS) {
      return XILHOSTAPPSVC_ERROR;
    }                                  /* if */

    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      mxArray * rhs[ 4 ];
      rhs[0] = mxCreateString("Connectivity:target:CaughtTargetException");
      rhs[1] = mxCreateString((const char*)exceptionType);
      rhs[2] = mxCreateString((const char*)exception);
      rhs[3] = mxCreateDoubleScalar(ssGetT(S));
      xilUtilsHandleError(pXILUtils, 4 , rhs );
      mxDestroyArray(rhs[0]);
      mxDestroyArray(rhs[1]);
      mxDestroyArray(rhs[2]);
      mxDestroyArray(rhs[3]);
      mxFree(exception);
      mxFree(exceptionType);
      return XILHOSTAPPSVC_ERROR;
    }
  } else {
    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      mxArray * rhs[ 2 ];
      rhs[0] = mxCreateString("Connectivity:target:CaughtUnknownTargetException");
      rhs[1] = mxCreateDoubleScalar(ssGetT(S));
      xilUtilsHandleError(pXILUtils, 2 , rhs );
      mxDestroyArray(rhs[0]);
      mxDestroyArray(rhs[1]);
      mxFree(exception);
      return XILHOSTAPPSVC_ERROR;
    }
  }                                    /* if */
}

static boolean_T processErrorAndTargetIOResponseCases(SimStruct * S, const int
  responseId)
{
  boolean_T processResponseStatus;
  switch (responseId) {
   case RESPONSE_ERROR:
    {
      processResponseStatus = processResponseError(S );
      break;
    }

   case RESPONSE_PRINTF:
    {
      processResponseStatus = processResponsePrintf(S );
      break;
    }

   case RESPONSE_FOPEN:
    {
      processResponseStatus = processResponseFopen(S );
      break;
    }

   case RESPONSE_FPRINTF:
    {
      processResponseStatus = processResponseFprintf(S );
      break;
    }

   case RESPONSE_SIGNAL_RAISED:
    {
      processResponseStatus = processResponseSignalRaised(S );
      break;
    }

   case RESPONSE_EXCEPTION:
    {
      processResponseStatus = processResponseException(S );
      break;
    }

   default:
    {
      {
        void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
        mxArray * rhs[ 2 ];
        rhs[0] = mxCreateString("PIL:pilverification:UnknownResponseId");
        rhs[1] = mxCreateDoubleScalar(responseId);
        xilUtilsHandleError(pXILUtils, 2 , rhs );
        mxDestroyArray(rhs[0]);
        mxDestroyArray(rhs[1]);
        processResponseStatus = XILHOSTAPPSVC_ERROR;
      }
    }
  }                                    /* switch */

  if (processResponseStatus == XILHOSTAPPSVC_ERROR) {
    callStopHookAndFreeSFcnMemory(S);
  }                                    /* if */

  return processResponseStatus;
}

/* Convert Simulink Type ID to Mem Unit Type ID */
static mem_unit_type_T simulinkToMemUnitType(SimStruct * S, DTypeId
  simulinkTypeID)
{
  mem_unit_type_T memUnitType = MEM_UNIT_INVALID_TYPE;
  switch (simulinkTypeID) {
   case SS_DOUBLE:
    {
      memUnitType = MEM_UNIT_DOUBLE_TYPE;
      break;
    }

   case SS_SINGLE:
    {
      memUnitType = MEM_UNIT_SINGLE_TYPE;
      break;
    }

   case SS_INT8:
    {
      memUnitType = MEM_UNIT_INT8_TYPE;
      break;
    }

   case SS_UINT8:
    {
      memUnitType = MEM_UNIT_UINT8_TYPE;
      break;
    }

   case SS_INT16:
    {
      memUnitType = MEM_UNIT_INT16_TYPE;
      break;
    }

   case SS_UINT16:
    {
      memUnitType = MEM_UNIT_UINT16_TYPE;
      break;
    }

   case SS_INT32:
    {
      memUnitType = MEM_UNIT_INT32_TYPE;
      break;
    }

   case SS_UINT32:
    {
      memUnitType = MEM_UNIT_UINT32_TYPE;
      break;
    }

   case SS_BOOLEAN:
    {
      memUnitType = MEM_UNIT_BOOLEAN_TYPE;
      break;
    }

   default:
    {
      if (ssGetDataTypeIsSpecifiedInteger(S,simulinkTypeID,0,64)) {
        memUnitType = MEM_UNIT_UINT64_TYPE;
        break;
      }                                /* if */
      else if (ssGetDataTypeIsSpecifiedInteger(S,simulinkTypeID,1,64)) {
        memUnitType = MEM_UNIT_INT64_TYPE;
        break;
      } else if (ssGetDataTypeIsHalfPrecision(S,simulinkTypeID)) {
        memUnitType = MEM_UNIT_UINT16_TYPE;
        break;
      } else if (ssGetDataTypeIsFixedPoint(S,simulinkTypeID) &&
                 (ssGetDataTypeFxpContainWordLen(S,simulinkTypeID) == 64)) {
        memUnitType = (ssGetDataTypeFxpIsSigned(S,simulinkTypeID)?
                       MEM_UNIT_INT64_TYPE:MEM_UNIT_UINT64_TYPE);
        break;
      } else {
        {
          void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
          mxArray * rhs[ 2 ];
          rhs[0] = mxCreateString("PIL:pilverification:UnknownDataType");
          rhs[1] = mxCreateDoubleScalar(simulinkTypeID);
          xilUtilsHandleError(pXILUtils, 2 , rhs );
          mxDestroyArray(rhs[0]);
          mxDestroyArray(rhs[1]);
          return memUnitType;
        }
        break;
      }
    }
  }                                    /* switch */

  return memUnitType;
}

static size_t getBusElementDimension(SimStruct * S, DTypeId dType, int_T
  offsetIdx, const int_T ** dimsOut, int_T * numDims)
{
  int_T numDimensions = ssGetBusElementNumDimensions(S, dType, offsetIdx);
  const int_T *dims = ssGetBusElementDimensions(S, dType, offsetIdx);
  size_t numElements = 1;
  int j = 0;
  for (j = 0; j< numDimensions; j++) {
    numElements *= dims[j];
  }                                    /* for */

  *dimsOut = dims;
  *numDims = numDimensions;
  return numElements;
}

/* Process params function shared between mdlStart and mdlProcessParams */
static void processParams(SimStruct * S)
{
  if (ssGetPWork(S) != NULL) {
    int * pIsXILApplicationStarted = (int *) ssGetPWorkValue(S, 5);
    if ((pIsXILApplicationStarted!=NULL) && (*pIsXILApplicationStarted==1)) {
      /* update run time params */
      ssUpdateAllTunableParamsAsRunTimeParams(S);

      {
        XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
        if (IOBufferPtr != NULL) {
          void * pXILService = (void *) ssGetPWorkValue(S, 9);
          if (pXILService != NULL) {
            if (xilHostAppSvcGrowIOBuffer(pXILService, IOBufferPtr, 5, 0)!=
                XILHOSTAPPSVC_SUCCESS) {
              return;
            }                          /* if */

            IOBufferPtr->bufferDataSize = 0;
            if (IOBufferPtr->data != NULL) {
              /* write command id */
              {
                uint8_T commandDataArg = (uint8_T) XIL_PROCESS_PARAMS_COMMAND;
                if (xilWriteData(S, &commandDataArg, 1, MEM_UNIT_UINT8_TYPE) !=
                    XILHOSTAPPSVC_SUCCESS) {
                  return;
                }                      /* if */
              }

              {
                {
                  uint8_T * simDataMemUnitPtr;
                  uint32_T commandDataFcnid = 0;
                  simDataMemUnitPtr = (uint8_T *) &commandDataFcnid;

                  {
                    size_t num_elements = 1;

                    {
                      if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                       MEM_UNIT_UINT32_TYPE) !=
                          XILHOSTAPPSVC_SUCCESS) {
                        return;
                      }                /* if */
                    }
                  }
                }
              }
            }                          /* if */
          }                            /* if */
        }                              /* if */
      }

      {
        XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
        if (IOBufferPtr != NULL) {
          /* dispatch command to the target */
          if (commandDispatch(S, IOBufferPtr, IOBufferPtr->bufferDataSize)!=
              XILHOSTAPPSVC_SUCCESS) {
            return;
          }                            /* if */

          {
            XILCommandResponseType commandResponseType =
              XIL_COMMAND_NOT_COMPLETE;
            while (commandResponseType != XIL_STEP_COMPLETE) {
              /* receive command from the target */
              if (commandResponse(S, IOBufferPtr, &commandResponseType) !=
                  XILHOSTAPPSVC_SUCCESS) {
                return;
              }                        /* if */

              if (IOBufferPtr->readData) {
                uint8_T responseId = 0;
                while (IOBufferPtr->readData) {
                  /* read response id */
                  if (xilReadData(S, &responseId, 1, MEM_UNIT_UINT8_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */

                  switch (responseId) {
                   case RESPONSE_ERROR:
                   case RESPONSE_PRINTF:
                   case RESPONSE_FOPEN:
                   case RESPONSE_FPRINTF:
                   case RESPONSE_EXCEPTION:
                   case RESPONSE_SIGNAL_RAISED:
                    {
                      if (processErrorAndTargetIOResponseCases(S, responseId)==
                          XILHOSTAPPSVC_ERROR) {
                        return;
                      }                /* if */
                      break;
                    }

                   default:
                    {
                      {
                        void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                        mxArray * rhs[ 2 ];
                        rhs[0] = mxCreateString(
                          "PIL:pilverification:UnknownResponseId");
                        rhs[1] = mxCreateDoubleScalar(responseId);
                        xilUtilsHandleError(pXILUtils, 2 , rhs );
                        mxDestroyArray(rhs[0]);
                        mxDestroyArray(rhs[1]);
                        return;
                      }
                      break;
                    }
                  }                    /* switch */
                }                      /* while */
              }                        /* if */
            }                          /* while */
          }
        }                              /* if */
      }
    }                                  /* if */
  }                                    /* if */
}

static boolean_T startAndSetupApplication(SimStruct *S)
{
  {
    {
      mxArray *rhs[4];
      char * simulinkBlockPath = getSimulinkBlockPath(S);
      if (simulinkBlockPath == NULL) {
        ssSetErrorStatus(S,
                         "ModelBlock SIL/PIL unexpected error: getSimulinkBlockPath returned NULL pointer. Check search string was found in ssGetPath.\n");
        return false;
      }

      rhs[ 0 ] = mxCreateString(
        "@coder.connectivity.SimulinkInterface.getSILPILInterface");
      rhs[ 1 ] = mxCreateDoubleScalar( 1 );
      rhs[ 2 ] = mxCreateString(simulinkBlockPath);
      rhs[3] = mxCreateString("uint8");

      {
        void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
        if (xilUtilsCallMATLAB(pXILUtils, 0, 0, 4, rhs,
                               "rtw.pil.SILPILInterface.sfunctionPILStartHook")
            !=XIL_UTILS_SUCCESS) {
          return false;
        }                              /* if */
      }

      mxFree((void *) simulinkBlockPath);
    }

    {                                  /* record that the XIL application has started */
      int * pIsXILApplicationStarted = (int *) ssGetPWorkValue(S, 5);
      *pIsXILApplicationStarted = 1;
      ssSetPWorkValue(S, 5, pIsXILApplicationStarted);
    }
  }

  {
    mxArray *rhs[3];
    mxArray *lhs[5];
    char * simulinkBlockPath = getSimulinkBlockPath(S);
    if (simulinkBlockPath == NULL) {
      ssSetErrorStatus(S,
                       "ModelBlock SIL/PIL unexpected error: getSimulinkBlockPath returned NULL pointer. Check search string was found in ssGetPath.\n");
      return false;
    }

    rhs[ 0 ] = mxCreateString(
      "@coder.connectivity.SimulinkInterface.getSILPILInterface");
    rhs[ 1 ] = mxCreateDoubleScalar( 1 );
    rhs[ 2 ] = mxCreateString(simulinkBlockPath);

    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      if (xilUtilsCallMATLAB(pXILUtils, 5, lhs, 3, rhs,
                             "rtw.pil.SILPILInterface.sfunctionGetRtIOStreamInfoHook")
          !=XIL_UTILS_SUCCESS) {
        return false;
      }                                /* if */
    }

    mxFree((void *) simulinkBlockPath);

    {
      XIL_RtIOStreamData_T* rtIOStreamDataPtr = (XIL_RtIOStreamData_T*) mxCalloc
        (1, sizeof(XIL_RtIOStreamData_T));
      if (rtIOStreamDataPtr == NULL) {
        ssSetErrorStatus( S,"Error in allocating memory through mxCalloc.");
        return false;
      }                                /* if */

      rtIOStreamDataPtr->lib = mxArrayToString(lhs[0]);
      rtIOStreamDataPtr->MATLABObject = mxDuplicateArray(lhs[1]);
      mexMakeMemoryPersistent(rtIOStreamDataPtr);
      mexMakeMemoryPersistent(rtIOStreamDataPtr->lib);
      mexMakeArrayPersistent(rtIOStreamDataPtr->MATLABObject);
      rtIOStreamDataPtr->streamID = *mxGetPr(lhs[2]);
      rtIOStreamDataPtr->recvTimeout = *mxGetPr(lhs[3]);
      rtIOStreamDataPtr->sendTimeout = *mxGetPr(lhs[4]);
      rtIOStreamDataPtr->isRtIOStreamCCall = 1;
      rtIOStreamDataPtr->ioMxClassID = mxUINT8_CLASS;
      rtIOStreamDataPtr->ioDataSize = sizeof(uint8_T);
      rtIOStreamDataPtr->targetRecvBufferSizeBytes = 50000;
      rtIOStreamDataPtr->targetSendBufferSizeBytes = 50000 + 1;

      {
        int errorStatus = rtIOStreamLoadLib(&rtIOStreamDataPtr->libH,
          rtIOStreamDataPtr->lib);
        if (errorStatus) {
          ssSetErrorStatus( S,"rtIOStreamLoadLib failed.");
          return false;
        }                              /* if */
      }

      ssSetPWorkValue(S, 0, rtIOStreamDataPtr);
    }

    {
      int i;
      for (i=0; i<5; i++) {
        mxDestroyArray(lhs[i]);
      }                                /* for */
    }
  }

  {
    XIL_IOBuffer_T* IOBufferPtr = (XIL_IOBuffer_T *) mxCalloc(1, sizeof
      (XIL_IOBuffer_T));
    if (IOBufferPtr == NULL) {
      ssSetErrorStatus( S,"Error in allocating memory through mxCalloc.");
      return false;
    }                                  /* if */

    mexMakeMemoryPersistent(IOBufferPtr);
    ssSetPWorkValue(S, 1, IOBufferPtr);
  }

  {
    SIL_DEBUGGING_DATA_T* silDebuggingDataPtr = (SIL_DEBUGGING_DATA_T*) mxCalloc
      (1, sizeof(SIL_DEBUGGING_DATA_T));
    char * simulinkBlockPath = getSimulinkBlockPath(S);
    if (simulinkBlockPath == NULL) {
      ssSetErrorStatus(S,
                       "ModelBlock SIL/PIL unexpected error: getSimulinkBlockPath returned NULL pointer. Check search string was found in ssGetPath.\n");
      return false;
    }

    if (silDebuggingDataPtr == NULL) {
      ssSetErrorStatus( S,
                       "Error in allocating memory through mxCalloc for SIL_DEBUGGING_DATA_T.");
      return false;
    }                                  /* if */

    silDebuggingDataPtr->componentBlockPath = strcpy((char *) mxCalloc(strlen
      (simulinkBlockPath)+1, sizeof(char)), simulinkBlockPath);
    silDebuggingDataPtr->SILPILInterfaceFcnStr = strcpy((char*) mxCalloc(57,
      sizeof(char)), "@coder.connectivity.SimulinkInterface.getSILPILInterface");
    silDebuggingDataPtr->inTheLoopType = 1;
    mexMakeMemoryPersistent(silDebuggingDataPtr);
    mexMakeMemoryPersistent(silDebuggingDataPtr->componentBlockPath);
    mexMakeMemoryPersistent(silDebuggingDataPtr->SILPILInterfaceFcnStr);
    ssSetPWorkValue(S, 2, silDebuggingDataPtr);
    mxFree((void *) simulinkBlockPath);
  }

  {
    targetIOFd_T * targetIOFdPtr = (targetIOFd_T *) mxCalloc(1, sizeof
      (targetIOFd_T));
    if (targetIOFdPtr == NULL) {
      return false;
    }                                  /* if */

    mexMakeMemoryPersistent(targetIOFdPtr);
    targetIOFdPtr->size = 0;
    targetIOFdPtr->Fd = NULL;
    targetIOFdPtr->fidOffset = -1;
    ssSetPWorkValue(S, 3, targetIOFdPtr);
  }

  {
    void* pConnectionOptions = NULL;
    void* pComms = NULL;
    void* pXILService = NULL;
    void* pMemUnitTransformer = NULL;
    void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
    uint8_T memUnitSizeBytes = 1;
    uint8_T ioDataTypeSizeBytes = sizeof(uint8_T);
    XIL_RtIOStreamData_T * rtIOStreamDataPtr = (XIL_RtIOStreamData_T *)
      ssGetPWorkValue(S, 0);
    SIL_DEBUGGING_DATA_T * silDebuggingDataPtr = (SIL_DEBUGGING_DATA_T *)
      ssGetPWorkValue(S, 2);
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    void * pCoderAssumptionsApp = NULL;
    if (xilCommsCreate(&pComms, rtIOStreamDataPtr, silDebuggingDataPtr,
                       memUnitSizeBytes, pMemUnitTransformer, pXILUtils, 0) !=
        XILCOMMS_RTIOSTREAM_SUCCESS) {
      return false;
    }                                  /* if */

    if (xilHostAppSvcCreate(&pXILService, pComms, pXILUtils, IOBufferPtr,
                            memUnitSizeBytes, ioDataTypeSizeBytes, 0) !=
        XILHOSTAPPSVC_SUCCESS) {
      return false;
    }                                  /* if */

    {
      mxArray * codeGenComponent = mxCreateString("mcu");
      mxArray *rhs[3];
      char * simulinkBlockPath = getSimulinkBlockPath(S);
      if (simulinkBlockPath == NULL) {
        ssSetErrorStatus(S,
                         "ModelBlock SIL/PIL unexpected error: getSimulinkBlockPath returned NULL pointer. Check search string was found in ssGetPath.\n");
        return false;
      }

      rhs[ 0 ] = mxCreateString(
        "@coder.connectivity.SimulinkInterface.getSILPILInterface");
      rhs[ 1 ] = mxCreateDoubleScalar( 1 );
      rhs[ 2 ] = mxCreateString(simulinkBlockPath);
      if (coderAssumpHostAppSvcCreate(&pCoderAssumptionsApp, pComms, pXILUtils,
           0, 1, codeGenComponent, rhs[0], rhs[1], rhs[2], 1, 0, 1) !=
          CODERASSUMPHOSTAPPSVC_SUCCESS) {
        return false;
      }                                /* if */

      mxFree((void *) simulinkBlockPath);
    }

    xilCommsRegisterApplication(pComms, pXILService);
    xilCommsRegisterApplication(pComms, pCoderAssumptionsApp);
    ssSetPWorkValue(S, 9, pXILService);
    ssSetPWorkValue(S, 7, pComms);
    ssSetPWorkValue(S, 6, pXILUtils);
    ssSetPWorkValue(S, 10, pCoderAssumptionsApp);
  }

  {
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr != NULL) {
      void * pXILService = (void *) ssGetPWorkValue(S, 9);
      if (pXILService != NULL) {
        if (xilHostAppSvcGrowIOBuffer(pXILService, IOBufferPtr, 5, 0)!=
            XILHOSTAPPSVC_SUCCESS) {
          return false;
        }                              /* if */

        IOBufferPtr->bufferDataSize = 0;
        if (IOBufferPtr->data != NULL) {
          /* write command id */
          {
            uint8_T commandDataArg = (uint8_T) XIL_INIT_COMMAND;
            if (xilWriteData(S, &commandDataArg, 1, MEM_UNIT_UINT8_TYPE) !=
                XILHOSTAPPSVC_SUCCESS) {
              return false;
            }                          /* if */
          }

          {
            {
              uint8_T * simDataMemUnitPtr;
              uint32_T commandDataFcnid = 0;
              simDataMemUnitPtr = (uint8_T *) &commandDataFcnid;

              {
                size_t num_elements = 1;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_UINT32_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return false;
                  }                    /* if */
                }
              }
            }
          }
        }                              /* if */
      }                                /* if */
    }                                  /* if */
  }

  {
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr != NULL) {
      /* dispatch command to the target */
      if (commandDispatch(S, IOBufferPtr, IOBufferPtr->bufferDataSize)!=
          XILHOSTAPPSVC_SUCCESS) {
        return false;
      }                                /* if */

      {
        XILCommandResponseType commandResponseType = XIL_COMMAND_NOT_COMPLETE;
        while (commandResponseType != XIL_STEP_COMPLETE) {
          /* receive command from the target */
          if (commandResponse(S, IOBufferPtr, &commandResponseType) !=
              XILHOSTAPPSVC_SUCCESS) {
            return false;
          }                            /* if */

          if (IOBufferPtr->readData) {
            uint8_T responseId = 0;

#define RESPONSE_TYPE_SIZE             8

            while (IOBufferPtr->readData) {
              /* read response id */
              if (xilReadData(S, &responseId, 1, MEM_UNIT_UINT8_TYPE) !=
                  XILHOSTAPPSVC_SUCCESS) {
                return false;
              }                        /* if */

              switch (responseId) {
               case RESPONSE_ERROR:
               case RESPONSE_PRINTF:
               case RESPONSE_FOPEN:
               case RESPONSE_FPRINTF:
               case RESPONSE_EXCEPTION:
               case RESPONSE_SIGNAL_RAISED:
                {
                  if (processErrorAndTargetIOResponseCases(S, responseId)==
                      XILHOSTAPPSVC_ERROR) {
                    return false;
                  }                    /* if */
                  break;
                }

               case RESPONSE_TYPE_SIZE:
                {
                  uint8_T typeBytes;
                  uint8_T typeId;
                  if (xilReadData(S, &typeId, 1, MEM_UNIT_UINT8_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return false;
                  }                    /* if */

                  if (xilReadData(S, &typeBytes, 1, MEM_UNIT_UINT8_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return false;
                  }                    /* if */

                  switch (typeId) {
                   case SS_SINGLE:
                    {
                      if (typeBytes != 4) {
                        {
                          void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                          mxArray * rhs[ 3 ];
                          rhs[0] = mxCreateString(
                            "PIL:pilverification:SingleUnsupported");
                          rhs[1] = mxCreateDoubleScalar(4);
                          rhs[2] = mxCreateDoubleScalar(typeBytes);
                          xilUtilsHandleError(pXILUtils, 3 , rhs );
                          mxDestroyArray(rhs[0]);
                          mxDestroyArray(rhs[1]);
                          mxDestroyArray(rhs[2]);
                          return false;
                        }
                      }                /* if */
                      break;
                    }

                   case SS_DOUBLE:
                    {
                      if (typeBytes != 8) {
                        {
                          void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                          mxArray * rhs[ 3 ];
                          rhs[0] = mxCreateString(
                            "PIL:pilverification:DoubleUnsupported");
                          rhs[1] = mxCreateDoubleScalar(8);
                          rhs[2] = mxCreateDoubleScalar(typeBytes);
                          xilUtilsHandleError(pXILUtils, 3 , rhs );
                          mxDestroyArray(rhs[0]);
                          mxDestroyArray(rhs[1]);
                          mxDestroyArray(rhs[2]);
                          return false;
                        }
                      }                /* if */
                      break;
                    }

                   default:
                    {
                      {
                        void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                        mxArray * rhs[ 2 ];
                        rhs[0] = mxCreateString(
                          "PIL:pilverification:UnknownTypeId");
                        rhs[1] = mxCreateDoubleScalar(typeId);
                        xilUtilsHandleError(pXILUtils, 2 , rhs );
                        mxDestroyArray(rhs[0]);
                        mxDestroyArray(rhs[1]);
                        return false;
                      }
                      break;
                    }
                  }                    /* switch */
                  break;
                }

               default:
                {
                  {
                    void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                    mxArray * rhs[ 2 ];
                    rhs[0] = mxCreateString(
                      "PIL:pilverification:UnknownResponseId");
                    rhs[1] = mxCreateDoubleScalar(responseId);
                    xilUtilsHandleError(pXILUtils, 2 , rhs );
                    mxDestroyArray(rhs[0]);
                    mxDestroyArray(rhs[1]);
                    return false;
                  }
                  break;
                }
              }                        /* switch */
            }                          /* while */
          }                            /* if */
        }                              /* while */
      }
    }                                  /* if */
  }

  /* initialize parameters */
  processParams(S);
  return true;
}

static void sendInitializeCommand(SimStruct *S)
{
  {
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr != NULL) {
      void * pXILService = (void *) ssGetPWorkValue(S, 9);
      if (pXILService != NULL) {
        if (xilHostAppSvcGrowIOBuffer(pXILService, IOBufferPtr, 13, 0)!=
            XILHOSTAPPSVC_SUCCESS) {
          return;
        }                              /* if */

        IOBufferPtr->bufferDataSize = 0;
        if (IOBufferPtr->data != NULL) {
          /* write command id */
          {
            uint8_T commandDataArg = (uint8_T) XIL_INITIALIZE_COMMAND;
            if (xilWriteData(S, &commandDataArg, 1, MEM_UNIT_UINT8_TYPE) !=
                XILHOSTAPPSVC_SUCCESS) {
              return;
            }                          /* if */
          }

          {
            {
              uint8_T * simDataMemUnitPtr;
              uint32_T commandDataFcnid = 0;
              simDataMemUnitPtr = (uint8_T *) &commandDataFcnid;

              {
                size_t num_elements = 1;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_UINT32_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            {
              uint8_T * simDataMemUnitPtr;
              int_T tidIdx = 0;
              uint32_T taskCounterOffset = computeTaskTickOffset(S, tidIdx);
              simDataMemUnitPtr = (uint8_T *) &taskCounterOffset;

              {
                size_t num_elements = 1;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_UINT32_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            {
              uint8_T * simDataMemUnitPtr;
              int_T tidIdx = 1;
              uint32_T taskCounterOffset = computeTaskTickOffset(S, tidIdx);
              simDataMemUnitPtr = (uint8_T *) &taskCounterOffset;

              {
                size_t num_elements = 1;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_UINT32_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }
        }                              /* if */
      }                                /* if */
    }                                  /* if */
  }

  {
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr != NULL) {
      /* dispatch command to the target */
      if (commandDispatch(S, IOBufferPtr, IOBufferPtr->bufferDataSize)!=
          XILHOSTAPPSVC_SUCCESS) {
        return;
      }                                /* if */

      {
        XILCommandResponseType commandResponseType = XIL_COMMAND_NOT_COMPLETE;
        while (commandResponseType != XIL_STEP_COMPLETE) {
          /* receive command from the target */
          if (commandResponse(S, IOBufferPtr, &commandResponseType) !=
              XILHOSTAPPSVC_SUCCESS) {
            return;
          }                            /* if */

          if (IOBufferPtr->readData) {
            uint8_T responseId = 0;
            while (IOBufferPtr->readData) {
              /* read response id */
              if (xilReadData(S, &responseId, 1, MEM_UNIT_UINT8_TYPE) !=
                  XILHOSTAPPSVC_SUCCESS) {
                return;
              }                        /* if */

              switch (responseId) {
               case RESPONSE_ERROR:
               case RESPONSE_PRINTF:
               case RESPONSE_FOPEN:
               case RESPONSE_FPRINTF:
               case RESPONSE_EXCEPTION:
               case RESPONSE_SIGNAL_RAISED:
                {
                  if (processErrorAndTargetIOResponseCases(S, responseId)==
                      XILHOSTAPPSVC_ERROR) {
                    return;
                  }                    /* if */
                  break;
                }

               case RESPONSE_OUTPUT_PRE_DATA:
                {
                  break;
                }

               case RESPONSE_OUTPUT_DATA:
                {
                  {
                    /* DataInterface: output, 1 */
                    void * dataInterfacePtr = (void *) ssGetOutputPortSignal(S,
                      0);

                    {
                      uint8_T * simDataMemUnitPtr;

                      /* DataInterface: output, 1 */
                      simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

                      {
                        size_t num_elements = 4;

                        {
                          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                                          MEM_UNIT_DOUBLE_TYPE) !=
                              XILHOSTAPPSVC_SUCCESS) {
                            return;
                          }            /* if */
                        }
                      }
                    }
                  }

                  {
                    /* DataInterface: output, 2 */
                    void * dataInterfacePtr = (void *) ssGetOutputPortSignal(S,
                      1);

                    {
                      uint8_T * simDataMemUnitPtr;

                      /* DataInterface: output, 2 */
                      simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

                      {
                        size_t num_elements = 1;

                        {
                          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                                          MEM_UNIT_UINT8_TYPE) !=
                              XILHOSTAPPSVC_SUCCESS) {
                            return;
                          }            /* if */
                        }
                      }
                    }
                  }
                  break;
                }

               default:
                {
                  {
                    void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                    mxArray * rhs[ 2 ];
                    rhs[0] = mxCreateString(
                      "PIL:pilverification:UnknownResponseId");
                    rhs[1] = mxCreateDoubleScalar(responseId);
                    xilUtilsHandleError(pXILUtils, 2 , rhs );
                    mxDestroyArray(rhs[0]);
                    mxDestroyArray(rhs[1]);
                    return;
                  }
                  break;
                }
              }                        /* switch */
            }                          /* while */
          }                            /* if */
        }                              /* while */
      }
    }                                  /* if */
  }
}

static void initializeInfAndMinusInf(void)
{
  typedef struct {
    struct {
      uint32_T wordL;
      uint32_T wordH;
    } words;
  } LittleEndianIEEEDouble;

  rtInf = 0.0;
  rtMinusInf = 0.0;

  {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal1;

    tmpVal1.bitVal.words.wordH = 0x7FF00000U;
    tmpVal1.bitVal.words.wordL = 0x00000000U;
    rtInf = tmpVal1.fltVal;
  }

  {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal2;

    tmpVal2.bitVal.words.wordH = 0xFFF00000U;
    tmpVal2.bitVal.words.wordL = 0x00000000U;
    rtMinusInf = tmpVal2.fltVal;
  }
}

static void mdlSystemInitialize(SimStruct *S)
{
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S)) {
    return;
  }                                    /* if */

  {
    int * pIsXILApplicationStartedLocal = (int *) ssGetPWorkValue(S, 5);
    if (pIsXILApplicationStartedLocal == NULL || (*pIsXILApplicationStartedLocal)
        == 0) {
      boolean_T applicationStarted = startAndSetupApplication(S);
      if (!applicationStarted) {
        return;
      }                                /* if */
    }                                  /* if */

    sendInitializeCommand(S);
  }

  {
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr != NULL) {
      void * pXILService = (void *) ssGetPWorkValue(S, 9);
      if (pXILService != NULL) {
        if (xilHostAppSvcGrowIOBuffer(pXILService, IOBufferPtr, 5, 0)!=
            XILHOSTAPPSVC_SUCCESS) {
          return;
        }                              /* if */

        IOBufferPtr->bufferDataSize = 0;
        if (IOBufferPtr->data != NULL) {
          /* write command id */
          {
            uint8_T commandDataArg = (uint8_T) XIL_SYSTEM_INITIALIZE_COMMAND;
            if (xilWriteData(S, &commandDataArg, 1, MEM_UNIT_UINT8_TYPE) !=
                XILHOSTAPPSVC_SUCCESS) {
              return;
            }                          /* if */
          }

          {
            {
              uint8_T * simDataMemUnitPtr;
              uint32_T commandDataFcnid = 0;
              simDataMemUnitPtr = (uint8_T *) &commandDataFcnid;

              {
                size_t num_elements = 1;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_UINT32_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }
        }                              /* if */
      }                                /* if */
    }                                  /* if */
  }

  {
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr != NULL) {
      /* dispatch command to the target */
      if (commandDispatch(S, IOBufferPtr, IOBufferPtr->bufferDataSize)!=
          XILHOSTAPPSVC_SUCCESS) {
        return;
      }                                /* if */

      {
        XILCommandResponseType commandResponseType = XIL_COMMAND_NOT_COMPLETE;
        while (commandResponseType != XIL_STEP_COMPLETE) {
          /* receive command from the target */
          if (commandResponse(S, IOBufferPtr, &commandResponseType) !=
              XILHOSTAPPSVC_SUCCESS) {
            return;
          }                            /* if */

          if (IOBufferPtr->readData) {
            uint8_T responseId = 0;
            while (IOBufferPtr->readData) {
              /* read response id */
              if (xilReadData(S, &responseId, 1, MEM_UNIT_UINT8_TYPE) !=
                  XILHOSTAPPSVC_SUCCESS) {
                return;
              }                        /* if */

              switch (responseId) {
               case RESPONSE_ERROR:
               case RESPONSE_PRINTF:
               case RESPONSE_FOPEN:
               case RESPONSE_FPRINTF:
               case RESPONSE_EXCEPTION:
               case RESPONSE_SIGNAL_RAISED:
                {
                  if (processErrorAndTargetIOResponseCases(S, responseId)==
                      XILHOSTAPPSVC_ERROR) {
                    return;
                  }                    /* if */
                  break;
                }

               default:
                {
                  {
                    void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                    mxArray * rhs[ 2 ];
                    rhs[0] = mxCreateString(
                      "PIL:pilverification:UnknownResponseId");
                    rhs[1] = mxCreateDoubleScalar(responseId);
                    xilUtilsHandleError(pXILUtils, 2 , rhs );
                    mxDestroyArray(rhs[0]);
                    mxDestroyArray(rhs[1]);
                    return;
                  }
                  break;
                }
              }                        /* switch */
            }                          /* while */
          }                            /* if */
        }                              /* while */
      }
    }                                  /* if */
  }
}

static void mdlSystemReset(SimStruct *S)
{
  {
    void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
    mxArray * prhs[ 1 ];
    prhs[0] = mxCreateString("PIL:pil:InitCondOptimCallbackError");
    if (xilUtilsCallMATLAB(pXILUtils, 0, NULL, 1, prhs, "DAStudio.error") !=
        XIL_UTILS_SUCCESS) {
      return;
    }                                  /* if */
  }
}

static void mdlPeriodicOutputUpdate(SimStruct *S, int_T tid);

/* This function checks the attributes of tunable parameters. */
#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)

static void mdlCheckParameters(SimStruct *S)
{
}

#endif                                 /* MDL_CHECK_PARAMETERS */

static void mdlInitializeSizes(SimStruct *S)
{
  ssSetNumSFcnParams(S, 0);            /* Number of expected parameters */
  if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {

#if defined(MDL_CHECK_PARAMETERS)

    mdlCheckParameters(S);

#endif

    if (ssGetErrorStatus(S) != (NULL))
      return;
  } else {
    /* Parameter mismatch will be reported by Simulink */
    return;
  }

  ssSetNumContStates(S, 0);
  ssSetNumDiscStates(S, 0);

  /* no support for SimState */
  ssSetSimStateCompliance(S, DISALLOW_SIM_STATE);

  /* Allow signal dimensions greater than 2 */
  ssAllowSignalsWithMoreThan2D(S);

  /* Allow fixed-point data types with 33 or more bits */
  ssFxpSetU32BitRegionCompliant(S,1);
  initializeInfAndMinusInf();
  ssSetRTWGeneratedSFcn(S, 2);
  if ((S->mdlInfo->genericFcn != (NULL)) && (!(S->mdlInfo->genericFcn)(S,
        GEN_FCN_CHK_MODELREF_SFUN_HAS_MODEL_BLOCK, -1, (NULL)))) {
    return;
  }

  slmrInitializeIOPortDataVectors(S, 11, 2);
  if (!ssSetNumInputPorts(S, 11))
    return;
  if (!ssSetInputPortVectorDimension(S, 0, 3))
    return;
  ssSetInputPortDimensionsMode(S, 0, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 0, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 0, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 0, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 0, 1);
  ssSetInputPortRequiredContiguous(S, 0, 1);
  ssSetInputPortOverWritable(S, 0, false);
  ssSetInputPortSampleTime(S, 0, 0.001);
  ssSetInputPortOffsetTime(S, 0, 0.0);
  if (!ssSetInputPortVectorDimension(S, 1, 3))
    return;
  ssSetInputPortDimensionsMode(S, 1, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 1, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 1, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 1, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 1, 1);
  ssSetInputPortRequiredContiguous(S, 1, 1);
  ssSetInputPortOverWritable(S, 1, false);
  ssSetInputPortSampleTime(S, 1, 0.001);
  ssSetInputPortOffsetTime(S, 1, 0.0);
  if (!ssSetInputPortVectorDimension(S, 2, 1))
    return;
  ssSetInputPortDimensionsMode(S, 2, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 2, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 2, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 2, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 2, 1);
  ssSetInputPortRequiredContiguous(S, 2, 1);
  ssSetInputPortOverWritable(S, 2, false);
  ssSetInputPortSampleTime(S, 2, 0.001);
  ssSetInputPortOffsetTime(S, 2, 0.0);
  if (!ssSetInputPortVectorDimension(S, 3, 4))
    return;
  ssSetInputPortDimensionsMode(S, 3, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 3, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 3, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 3, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 3, 1);
  ssSetInputPortRequiredContiguous(S, 3, 1);
  ssSetInputPortOverWritable(S, 3, false);
  ssSetInputPortSampleTime(S, 3, 0.001);
  ssSetInputPortOffsetTime(S, 3, 0.0);
  if (!ssSetInputPortVectorDimension(S, 4, 4))
    return;
  ssSetInputPortDimensionsMode(S, 4, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 4, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 4, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 4, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 4, 1);
  ssSetInputPortRequiredContiguous(S, 4, 1);
  ssSetInputPortOverWritable(S, 4, false);
  ssSetInputPortSampleTime(S, 4, 0.001);
  ssSetInputPortOffsetTime(S, 4, 0.0);
  if (!ssSetInputPortVectorDimension(S, 5, 3))
    return;
  ssSetInputPortDimensionsMode(S, 5, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 5, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 5, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 5, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 5, 1);
  ssSetInputPortRequiredContiguous(S, 5, 1);
  ssSetInputPortOverWritable(S, 5, false);
  ssSetInputPortSampleTime(S, 5, 0.001);
  ssSetInputPortOffsetTime(S, 5, 0.0);
  if (!ssSetInputPortVectorDimension(S, 6, 3))
    return;
  ssSetInputPortDimensionsMode(S, 6, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 6, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 6, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 6, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 6, 1);
  ssSetInputPortRequiredContiguous(S, 6, 1);
  ssSetInputPortOverWritable(S, 6, false);
  ssSetInputPortSampleTime(S, 6, 0.001);
  ssSetInputPortOffsetTime(S, 6, 0.0);
  if (!ssSetInputPortVectorDimension(S, 7, 4))
    return;
  ssSetInputPortDimensionsMode(S, 7, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 7, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 7, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 7, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 7, 1);
  ssSetInputPortRequiredContiguous(S, 7, 1);
  ssSetInputPortOverWritable(S, 7, false);
  ssSetInputPortSampleTime(S, 7, 0.001);
  ssSetInputPortOffsetTime(S, 7, 0.0);
  if (!ssSetInputPortVectorDimension(S, 8, 1))
    return;
  ssSetInputPortDimensionsMode(S, 8, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 8, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 8, SS_UINT8);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 8, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 8, 1);
  ssSetInputPortRequiredContiguous(S, 8, 1);
  ssSetInputPortOverWritable(S, 8, false);
  ssSetInputPortSampleTime(S, 8, 0.001);
  ssSetInputPortOffsetTime(S, 8, 0.0);
  if (!ssSetInputPortVectorDimension(S, 9, 1))
    return;
  ssSetInputPortDimensionsMode(S, 9, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 9, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 9, SS_BOOLEAN);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 9, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 9, 1);
  ssSetInputPortRequiredContiguous(S, 9, 1);
  ssSetInputPortOverWritable(S, 9, false);
  ssSetInputPortSampleTime(S, 9, 0.001);
  ssSetInputPortOffsetTime(S, 9, 0.0);
  if (!ssSetInputPortVectorDimension(S, 10, 1))
    return;
  ssSetInputPortDimensionsMode(S, 10, FIXED_DIMS_MODE);
  ssSetInputPortFrameData(S, 10, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetInputPortDataType(S, 10, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetInputPortUnit(S, 10, unitIdReg);

#endif

  }

  ssSetInputPortDirectFeedThrough(S, 10, 1);
  ssSetInputPortRequiredContiguous(S, 10, 1);
  ssSetInputPortOverWritable(S, 10, false);
  ssSetInputPortSampleTime(S, 10, 1.0);
  ssSetInputPortOffsetTime(S, 10, 0.0);
  if (!ssSetNumOutputPorts(S, 2))
    return;
  if (!ssSetOutputPortVectorDimension(S, 0, 4))
    return;
  ssSetOutputPortDimensionsMode(S, 0, FIXED_DIMS_MODE);
  ssSetOutputPortFrameData(S, 0, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetOutputPortDataType(S, 0, SS_DOUBLE);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetOutputPortUnit(S, 0, unitIdReg);

#endif

  }

  ssSetOutputPortSampleTime(S, 0, 0.001);
  ssSetOutputPortOffsetTime(S, 0, 0.0);
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S) ) {
    ssSetOutputPortOkToMerge(S, 0, SS_OK_TO_MERGE_CONDITIONAL);
  }                                    /* if */

  ssSetOutputPortICAttributes(S, 0, false, false, false);
  if (!ssSetOutputPortVectorDimension(S, 1, 1))
    return;
  ssSetOutputPortDimensionsMode(S, 1, FIXED_DIMS_MODE);
  ssSetOutputPortFrameData(S, 1, FRAME_NO);
  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssSetOutputPortDataType(S, 1, SS_UINT8);
  }

  if (ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {

#if defined (MATLAB_MEX_FILE)

    UnitId unitIdReg;
    ssRegisterUnitFromExpr(
      S,
      "",
      &unitIdReg);
    if (unitIdReg == INVALID_UNIT_ID)
      return;
    ssSetOutputPortUnit(S, 1, unitIdReg);

#endif

  }

  ssSetOutputPortSampleTime(S, 1, 1.0);
  ssSetOutputPortOffsetTime(S, 1, 0.0);
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S) ) {
    ssSetOutputPortOkToMerge(S, 1, SS_OK_TO_MERGE);
  }                                    /* if */

  ssSetOutputPortICAttributes(S, 1, false, false, false);

  {
    int_T zcsIdx = 0;
  }

  ssSetOutputPortIsNonContinuous(S, 0, 0);
  ssSetOutputPortIsFedByBlockWithModesNoZCs(S, 0, 0);
  ssSetOutputPortIsNonContinuous(S, 1, 0);
  ssSetOutputPortIsFedByBlockWithModesNoZCs(S, 1, 0);
  ssSetInputPortIsNotDerivPort(S, 0, 1);
  ssSetInputPortIsNotDerivPort(S, 1, 1);
  ssSetInputPortIsNotDerivPort(S, 2, 1);
  ssSetInputPortIsNotDerivPort(S, 3, 1);
  ssSetInputPortIsNotDerivPort(S, 4, 1);
  ssSetInputPortIsNotDerivPort(S, 5, 1);
  ssSetInputPortIsNotDerivPort(S, 6, 1);
  ssSetInputPortIsNotDerivPort(S, 7, 1);
  ssSetInputPortIsNotDerivPort(S, 8, 1);
  ssSetInputPortIsNotDerivPort(S, 9, 1);
  ssSetInputPortIsNotDerivPort(S, 10, 1);
  ssSetModelReferenceSampleTimeInheritanceRule(S,
    DISALLOW_SAMPLE_TIME_INHERITANCE);
  ssSupportsMultipleExecInstances(S, true);
  ssSetAcceptsFcnCallInputs(S);        /* All sample times are available through ports. Use port based sample times. */
  ssSetNumSampleTimes(S, PORT_BASED_SAMPLE_TIMES);
  ssSetParameterTuningCompliance(S, true);
  ssSetModelReferenceSampleTimeInheritanceRule(S,
    DISALLOW_SAMPLE_TIME_INHERITANCE);
  ssSetOptions(S, SS_OPTION_SUPPORTS_ALIAS_DATA_TYPES |
               SS_OPTION_DISALLOW_CONSTANT_SAMPLE_TIME |
               SS_OPTION_CALL_TERMINATE_ON_EXIT);
  slmrRegisterSystemInitializeMethod(S, mdlSystemInitialize);
  slmrRegisterSystemResetMethod(S, mdlSystemReset);
  slmrRegisterPeriodicOutputUpdateMethod(S, mdlPeriodicOutputUpdate);
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S) ) {
    ssSetNumPWork(S, 0);
  } else {
    ssSetNumPWork(S, 12);
  }                                    /* if */

  ssSetNumRWork(S, 0);
  ssSetNumIWork(S, 0);
  ssSetNumModes(S, 0);
  ssSetNumNonsampledZCs(S, 0);
  ssSetModelReferenceNormalModeSupport(S, MDL_START_AND_MDL_PROCESS_PARAMS_OK);

#if SS_SFCN_FOR_SIM

  if (S->mdlInfo->genericFcn != (NULL) &&
      ssGetSimMode(S) != SS_SIMMODE_SIZES_CALL_ONLY) {
    ssRegModelRefMdlInfo(S, (char *)"mcu", rtMdlInfo_mcu, 1);
  }

#endif

  /* DWork */
  if (!ssRTWGenIsAccelerator(S)) {

#if SS_SFCN_FOR_SIM

    if (ssSetNumDWork(S, 1)) {
      ssSetDWorkWidth(S, 0, 1);
    }

#else

    if (!ssSetNumDWork(S, 1)) {
      return;
    }

#endif

  }                                    /* if */
}

#define MDL_SET_INPUT_PORT_SAMPLE_TIME                           /* Change to #undef to remove function */
#if defined(MDL_SET_INPUT_PORT_SAMPLE_TIME) && defined(MATLAB_MEX_FILE)

static void mdlSetInputPortSampleTime(SimStruct *S, int_T portIdx, real_T
  sampleTime, real_T offsetTime)
{
  /* sample times are fully specified, not inherited */
}

#endif                                 /* MDL_SET_INPUT_PORT_SAMPLE_TIME */

#define MDL_SET_OUTPUT_PORT_SAMPLE_TIME                          /* Change to #undef to remove function */
#if defined(MDL_SET_OUTPUT_PORT_SAMPLE_TIME) && defined(MATLAB_MEX_FILE)

static void mdlSetOutputPortSampleTime(SimStruct *S, int_T portIdx, real_T
  sampleTime, real_T offsetTime)
{
  /* sample times are fully specified, not inherited */
}

#endif                                 /* MDL_SET_OUTPUT_PORT_SAMPLE_TIME */

static void mdlInitializeSampleTimes(SimStruct *S)
{
  /* This block has port based sample time. */
  return;
}

#define MDL_SETUP_RUNTIME_RESOURCES                              /* Change to #undef to remove function */
#if defined(MDL_SETUP_RUNTIME_RESOURCES)

static void mdlSetupRuntimeResources(SimStruct *S)
{
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S)) {
    return;
  }                                    /* if */

  {
    mxArray * error = NULL;
    error = mexCallMATLABWithTrap( 0, NULL, 0, NULL,
      "rtw.pil.checkEmbeddedCoderInstalled");
    if (error != NULL) {
      mexCallMATLAB( 0, NULL, 1, &error, "throw");
    }                                  /* if */
  }

  {
    mxArray * lhs[1];
    mxArray * error = NULL;
    char * installVersion;
    error = mexCallMATLABWithTrap(1, lhs, 0, NULL, "rtw.pil.getPILVersion");
    if (error != NULL) {
      mxDestroyArray(error);
      ssSetErrorStatus( S,
                       "Failed to determine the installed In-the-Loop version for comparison against the In-the-Loop s-function version (release 25.2 (R2025b)_15). To avoid this error, remove the In-the-Loop s-function from your MATLAB path (e.g. delete it or move to a clean working directory).");
      return;
    }                                  /* if */

    if (mxIsEmpty(lhs[0])) {
      ssSetErrorStatus( S,"rtw.pil.getPILVersion returned empty!");
      return;
    }                                  /* if */

    installVersion = mxArrayToString(lhs[0]);
    mxDestroyArray(lhs[0]);
    if (installVersion == NULL) {
      ssSetErrorStatus( S,"Failed to determine installed In-the-Loop version.");
      return;
    }                                  /* if */

    if (strcmp(installVersion, "25.2 (R2025b)_15") != 0) {
      ssSetErrorStatus( S,
                       "The In-the-Loop s-function is incompatible with the installed In-the-Loop version (see ver('matlab')); it was generated for release 25.2 (R2025b)_15. To avoid this error, remove the In-the-Loop s-function from your MATLAB path (e.g. delete it or move to a clean working directory)");
      return;
    }                                  /* if */

    mxFree(installVersion);
  }

  {
    int retValXILUtils = XIL_UTILS_SUCCESS;
    void* pXILUtils = NULL;
    retValXILUtils = xilSimulinkUtilsCreate(&pXILUtils, S);
    if (retValXILUtils!=XIL_UTILS_SUCCESS) {
      ssSetErrorStatus( S,"Error instantiating XIL Utils!");
      return;
    }                                  /* if */

    ssSetPWorkValue(S, 6, pXILUtils);
  }

  {
    ssSetPWorkValue(S, 11, NULL);
  }

  {
    int * pCommErrorOccurred = (int *) mxCalloc(1, sizeof(int));
    if (pCommErrorOccurred == NULL) {
      ssSetErrorStatus( S,
                       "Error in allocating memory for pCommErrorOccurred through mxCalloc.");
      return;
    }                                  /* if */

    *pCommErrorOccurred = 0;
    mexMakeMemoryPersistent(pCommErrorOccurred);
    ssSetPWorkValue(S, 4, pCommErrorOccurred);
  }

  {
    int * pIsXILApplicationStarted = (int *) mxCalloc(1, sizeof(int));
    if (pIsXILApplicationStarted == NULL) {
      ssSetErrorStatus( S,
                       "Error in allocating memory for pIsXILApplicationStarted through mxCalloc.");
      return;
    }                                  /* if */

    *pIsXILApplicationStarted = 0;
    mexMakeMemoryPersistent(pIsXILApplicationStarted);
    ssSetPWorkValue(S, 5, pIsXILApplicationStarted);
  }

  {
    mxArray *rhs[4];
    mxArray *lhs[2];
    char * simulinkBlockPath = getSimulinkBlockPath(S);
    if (simulinkBlockPath == NULL) {
      ssSetErrorStatus(S,
                       "ModelBlock SIL/PIL unexpected error: getSimulinkBlockPath returned NULL pointer. Check search string was found in ssGetPath.\n");
      return;
    }

    rhs[ 0 ] = mxCreateString(
      "@coder.connectivity.SimulinkInterface.getSILPILInterface");
    rhs[ 1 ] = mxCreateDoubleScalar( 1 );
    rhs[ 2 ] = mxCreateString(simulinkBlockPath);
    rhs[3] = mxCreateString(ssGetPath(_ssGetRootSS(S)));

    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      if (xilUtilsCallMATLAB(pXILUtils, 2, lhs, 4, rhs,
                             "rtw.pil.SILPILInterface.sfunctionInitializeHook")
          !=XIL_UTILS_SUCCESS) {
        return;
      }                                /* if */
    }

    {
      mxFree((void *) simulinkBlockPath);
    }

    mxDestroyArray(lhs[0]);
    mxDestroyArray(lhs[1]);
  }
}

#endif                                 /* MDL_SETUP_RUNTIME_RESOURCES */

#define MDL_SIM_STATUS_CHANGE                                    /* Change to #undef to remove function */
#if defined(MDL_SIM_STATUS_CHANGE)

static void mdlSimStatusChange(SimStruct *S, ssSimStatusChangeType simStatus)
{
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S)) {
    return;
  }                                    /* if */

  if (simStatus == SIM_PAUSE) {
    {
      XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
      if (IOBufferPtr != NULL) {
        void * pXILService = (void *) ssGetPWorkValue(S, 9);
        if (pXILService != NULL) {
          if (xilHostAppSvcGrowIOBuffer(pXILService, IOBufferPtr, 5, 0)!=
              XILHOSTAPPSVC_SUCCESS) {
            return ;
          }                            /* if */

          IOBufferPtr->bufferDataSize = 0;
          if (IOBufferPtr->data != NULL) {
            /* write command id */
            {
              uint8_T commandDataArg = (uint8_T) XIL_PAUSE_COMMAND;
              if (xilWriteData(S, &commandDataArg, 1, MEM_UNIT_UINT8_TYPE) !=
                  XILHOSTAPPSVC_SUCCESS) {
                return;
              }                        /* if */
            }

            {
              {
                uint8_T * simDataMemUnitPtr;
                uint32_T commandDataFcnid = 0;
                simDataMemUnitPtr = (uint8_T *) &commandDataFcnid;

                {
                  size_t num_elements = 1;

                  {
                    if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                     MEM_UNIT_UINT32_TYPE) !=
                        XILHOSTAPPSVC_SUCCESS) {
                      return;
                    }                  /* if */
                  }
                }
              }
            }
          }                            /* if */
        }                              /* if */
      }                                /* if */
    }

    {
      XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
      if (IOBufferPtr != NULL) {
        /* dispatch command to the target */
        if (commandDispatch(S, IOBufferPtr, IOBufferPtr->bufferDataSize)!=
            XILHOSTAPPSVC_SUCCESS) {
          return ;
        }                              /* if */

        {
          XILCommandResponseType commandResponseType = XIL_COMMAND_NOT_COMPLETE;
          while (commandResponseType != XIL_STEP_COMPLETE) {
            /* receive command from the target */
            if (commandResponse(S, IOBufferPtr, &commandResponseType) !=
                XILHOSTAPPSVC_SUCCESS) {
              return ;
            }                          /* if */

            if (IOBufferPtr->readData) {
              uint8_T responseId = 0;
              while (IOBufferPtr->readData) {
                /* read response id */
                if (xilReadData(S, &responseId, 1, MEM_UNIT_UINT8_TYPE) !=
                    XILHOSTAPPSVC_SUCCESS) {
                  return;
                }                      /* if */

                switch (responseId) {
                 case RESPONSE_ERROR:
                 case RESPONSE_PRINTF:
                 case RESPONSE_FOPEN:
                 case RESPONSE_FPRINTF:
                 case RESPONSE_EXCEPTION:
                 case RESPONSE_SIGNAL_RAISED:
                  {
                    if (processErrorAndTargetIOResponseCases(S, responseId)==
                        XILHOSTAPPSVC_ERROR) {
                      return ;
                    }                  /* if */
                    break;
                  }

                 default:
                  {
                    {
                      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                      mxArray * rhs[ 2 ];
                      rhs[0] = mxCreateString(
                        "PIL:pilverification:UnknownResponseId");
                      rhs[1] = mxCreateDoubleScalar(responseId);
                      xilUtilsHandleError(pXILUtils, 2 , rhs );
                      mxDestroyArray(rhs[0]);
                      mxDestroyArray(rhs[1]);
                      return ;
                    }
                    break;
                  }
                }                      /* switch */
              }                        /* while */
            }                          /* if */
          }                            /* while */
        }
      }                                /* if */
    }

    {
      void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
      if (pXILUtils) {
        mxArray *rhs[3];
        char * simulinkBlockPath = getSimulinkBlockPath(S);
        if (simulinkBlockPath == NULL) {
          ssSetErrorStatus(S,
                           "ModelBlock SIL/PIL unexpected error: getSimulinkBlockPath returned NULL pointer. Check search string was found in ssGetPath.\n");
          return;
        }

        rhs[ 0 ] = mxCreateString(
          "@coder.connectivity.SimulinkInterface.getSILPILInterface");
        rhs[ 1 ] = mxCreateDoubleScalar( 1 );
        rhs[ 2 ] = mxCreateString(simulinkBlockPath);
        if (xilUtilsCallMATLAB(pXILUtils, 0, NULL, 3, rhs,
                               "rtw.pil.SILPILInterface.sfunctionPILPauseHook")
            != XIL_UTILS_SUCCESS) {
          return ;
        }                              /* if */

        mxFree((void *) simulinkBlockPath);
      }                                /* if */
    }
  }                                    /* if */
}

#endif                                 /* MDL_SIM_STATUS_CHANGE */

static void XILoutputTID01(SimStruct *S, int tid)
{
  {
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr != NULL) {
      void * pXILService = (void *) ssGetPWorkValue(S, 9);
      if (pXILService != NULL) {
        if (xilHostAppSvcGrowIOBuffer(pXILService, IOBufferPtr, 219, 0)!=
            XILHOSTAPPSVC_SUCCESS) {
          return;
        }                              /* if */

        IOBufferPtr->bufferDataSize = 0;
        if (IOBufferPtr->data != NULL) {
          /* write command id */
          {
            uint8_T commandDataArg = (uint8_T) XIL_OUTPUT_COMMAND;
            if (xilWriteData(S, &commandDataArg, 1, MEM_UNIT_UINT8_TYPE) !=
                XILHOSTAPPSVC_SUCCESS) {
              return;
            }                          /* if */
          }

          {
            {
              uint8_T * simDataMemUnitPtr;
              uint32_T commandDataFcnidTID[2] = { 0, 1 };

              simDataMemUnitPtr = (uint8_T *) &commandDataFcnidTID[0];

              {
                size_t num_elements = 2;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_UINT32_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 1 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 0);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 1 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 3;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_DOUBLE_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 2 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 1);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 2 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 3;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_DOUBLE_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 3 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 2);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 3 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 1;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_DOUBLE_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 4 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 3);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 4 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 4;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_DOUBLE_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 5 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 4);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 5 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 4;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_DOUBLE_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 6 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 5);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 6 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 3;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_DOUBLE_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 7 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 6);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 7 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 3;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_DOUBLE_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 8 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 7);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 8 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 4;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_DOUBLE_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 9 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 8);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 9 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 1;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_UINT8_TYPE) != XILHOSTAPPSVC_SUCCESS)
                  {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 10 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 9);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 10 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 1;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_BOOLEAN_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }

          {
            /* DataInterface: input, 11 */
            void * dataInterfacePtr = (void *) ssGetInputPortSignal(S, 10);

            {
              uint8_T * simDataMemUnitPtr;

              /* DataInterface: input, 11 */
              simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

              {
                size_t num_elements = 1;

                {
                  if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                   MEM_UNIT_DOUBLE_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */
                }
              }
            }
          }
        }                              /* if */
      }                                /* if */
    }                                  /* if */
  }

  {
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr != NULL) {
      /* dispatch command to the target */
      if (commandDispatch(S, IOBufferPtr, IOBufferPtr->bufferDataSize)!=
          XILHOSTAPPSVC_SUCCESS) {
        return;
      }                                /* if */

      {
        XILCommandResponseType commandResponseType = XIL_COMMAND_NOT_COMPLETE;
        while (commandResponseType != XIL_STEP_COMPLETE) {
          /* receive command from the target */
          if (commandResponse(S, IOBufferPtr, &commandResponseType) !=
              XILHOSTAPPSVC_SUCCESS) {
            return;
          }                            /* if */

          if (IOBufferPtr->readData) {
            uint8_T responseId = 0;
            while (IOBufferPtr->readData) {
              /* read response id */
              if (xilReadData(S, &responseId, 1, MEM_UNIT_UINT8_TYPE) !=
                  XILHOSTAPPSVC_SUCCESS) {
                return;
              }                        /* if */

              switch (responseId) {
               case RESPONSE_ERROR:
               case RESPONSE_PRINTF:
               case RESPONSE_FOPEN:
               case RESPONSE_FPRINTF:
               case RESPONSE_EXCEPTION:
               case RESPONSE_SIGNAL_RAISED:
                {
                  if (processErrorAndTargetIOResponseCases(S, responseId)==
                      XILHOSTAPPSVC_ERROR) {
                    return;
                  }                    /* if */
                  break;
                }

               case RESPONSE_OUTPUT_PRE_DATA:
                {
                  break;
                }

               case RESPONSE_OUTPUT_DATA:
                {
                  {
                    /* DataInterface: output, 1 */
                    void * dataInterfacePtr = (void *) ssGetOutputPortSignal(S,
                      0);

                    {
                      uint8_T * simDataMemUnitPtr;

                      /* DataInterface: output, 1 */
                      simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

                      {
                        size_t num_elements = 4;

                        {
                          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                                          MEM_UNIT_DOUBLE_TYPE) !=
                              XILHOSTAPPSVC_SUCCESS) {
                            return;
                          }            /* if */
                        }
                      }
                    }
                  }

                  {
                    /* DataInterface: output, 2 */
                    void * dataInterfacePtr = (void *) ssGetOutputPortSignal(S,
                      1);

                    {
                      uint8_T * simDataMemUnitPtr;

                      /* DataInterface: output, 2 */
                      simDataMemUnitPtr = ( uint8_T *) dataInterfacePtr;

                      {
                        size_t num_elements = 1;

                        {
                          if (xilReadData(S, simDataMemUnitPtr, num_elements,
                                          MEM_UNIT_UINT8_TYPE) !=
                              XILHOSTAPPSVC_SUCCESS) {
                            return;
                          }            /* if */
                        }
                      }
                    }
                  }
                  break;
                }

               default:
                {
                  {
                    void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                    mxArray * rhs[ 2 ];
                    rhs[0] = mxCreateString(
                      "PIL:pilverification:UnknownResponseId");
                    rhs[1] = mxCreateDoubleScalar(responseId);
                    xilUtilsHandleError(pXILUtils, 2 , rhs );
                    mxDestroyArray(rhs[0]);
                    mxDestroyArray(rhs[1]);
                    return;
                  }
                  break;
                }
              }                        /* switch */
            }                          /* while */
          }                            /* if */
        }                              /* while */
      }
    }                                  /* if */
  }

  {
    XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
    if (IOBufferPtr->readData) {
      uint8_T responseId = 0;
      while (IOBufferPtr->readData) {
        /* read response id */
        if (xilReadData(S, &responseId, 1, MEM_UNIT_UINT8_TYPE) !=
            XILHOSTAPPSVC_SUCCESS) {
          return;
        }                              /* if */

        switch (responseId) {
         case RESPONSE_ERROR:
         case RESPONSE_PRINTF:
         case RESPONSE_FOPEN:
         case RESPONSE_FPRINTF:
         case RESPONSE_EXCEPTION:
         case RESPONSE_SIGNAL_RAISED:
          {
            if (processErrorAndTargetIOResponseCases(S, responseId)==
                XILHOSTAPPSVC_ERROR) {
              return;
            }                          /* if */
            break;
          }

         default:
          {
            {
              void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
              mxArray * rhs[ 2 ];
              rhs[0] = mxCreateString("PIL:pilverification:UnknownResponseId");
              rhs[1] = mxCreateDoubleScalar(responseId);
              xilUtilsHandleError(pXILUtils, 2 , rhs );
              mxDestroyArray(rhs[0]);
              mxDestroyArray(rhs[1]);
              return;
            }
            break;
          }
        }                              /* switch */
      }                                /* while */
    }                                  /* if */
  }
}

#define MDL_PROCESS_PARAMETERS
#if defined(MDL_PROCESS_PARAMETERS)

static void mdlProcessParameters(SimStruct *S)
{
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S)) {
    return;
  }                                    /* if */

  processParams(S);
}

#endif                                 /* MDL_PROCESS_PARAMETERS */

#define MDL_SET_WORK_WIDTHS                                      /* Change to #undef to remove function */
#if defined(MDL_SET_WORK_WIDTHS)

static void mdlSetWorkWidths(SimStruct *S)
{
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S)) {
    if (S->mdlInfo->genericFcn != (NULL)) {
      _GenericFcn fcn = S->mdlInfo->genericFcn;
      ssSetSignalSizesComputeType(S, SS_VARIABLE_SIZE_FROM_INPUT_VALUE_AND_SIZE);
    }
  }                                    /* if */
}

#endif                                 /* MDL_SET_WORK_WIDTHS */

static void mdlPeriodicOutputUpdate(SimStruct *S, int_T tid)
{
  /* Periodic Output Update sample time matching */
  /* check for sample time hit associated with task 1 */
  if (tid == 0) {
    XILoutputTID01(S, tid);
  }                                    /* if */
}

static void mdlOutputs(SimStruct *S, int_T tid)
{
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S)) {
    return;
  }                                    /* if */

  /* Multirate, singletasking scheduling */
  /* check for sample time hit associated with task 1 */
  if (ssIsSampleHit(S, 0, tid)) {
    XILoutputTID01(S, tid);
  }                                    /* if */
}

#define MDL_UPDATE
#if defined(MDL_UPDATE)

static void mdlUpdate(SimStruct *S, int_T tid)
{
}

#endif

static void mdlTerminate(SimStruct *S)
{
  int commErrorOccurred = 0;
  int isXILApplicationStarted = 0;

  {
    if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S)) {
      return;
    }                                  /* if */

    if (ssGetPWork(S) != NULL) {
      int * pCommErrorOccurred = (int *) ssGetPWorkValue(S, 4);
      int * pIsXILApplicationStarted = (int *) ssGetPWorkValue(S, 5);
      if (pCommErrorOccurred != NULL) {
        commErrorOccurred = *pCommErrorOccurred;
      }                                /* if */

      if (pIsXILApplicationStarted != NULL) {
        isXILApplicationStarted = *pIsXILApplicationStarted;
      }                                /* if */
    }                                  /* if */
  }

  if (isXILApplicationStarted) {
    if (!commErrorOccurred) {
      {
        XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
        if (IOBufferPtr != NULL) {
          void * pXILService = (void *) ssGetPWorkValue(S, 9);
          if (pXILService != NULL) {
            if (xilHostAppSvcGrowIOBuffer(pXILService, IOBufferPtr, 5, 0)!=
                XILHOSTAPPSVC_SUCCESS) {
              callStopHookAndFreeSFcnMemory(S);
              return;
            }                          /* if */

            IOBufferPtr->bufferDataSize = 0;
            if (IOBufferPtr->data != NULL) {
              /* write command id */
              {
                uint8_T commandDataArg = (uint8_T) XIL_TERMINATE_COMMAND;
                if (xilWriteData(S, &commandDataArg, 1, MEM_UNIT_UINT8_TYPE) !=
                    XILHOSTAPPSVC_SUCCESS) {
                  return;
                }                      /* if */
              }

              {
                {
                  uint8_T * simDataMemUnitPtr;
                  uint32_T commandDataFcnid = 0;
                  simDataMemUnitPtr = (uint8_T *) &commandDataFcnid;

                  {
                    size_t num_elements = 1;

                    {
                      if (xilWriteData(S, simDataMemUnitPtr, num_elements,
                                       MEM_UNIT_UINT32_TYPE) !=
                          XILHOSTAPPSVC_SUCCESS) {
                        return;
                      }                /* if */
                    }
                  }
                }
              }
            }                          /* if */
          }                            /* if */
        }                              /* if */
      }

      {
        XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
        if (IOBufferPtr != NULL) {
          /* dispatch command to the target */
          if (commandDispatch(S, IOBufferPtr, IOBufferPtr->bufferDataSize)!=
              XILHOSTAPPSVC_SUCCESS) {
            callStopHookAndFreeSFcnMemory(S);
            return;
          }                            /* if */

          {
            XILCommandResponseType commandResponseType =
              XIL_COMMAND_NOT_COMPLETE;
            while (commandResponseType != XIL_STEP_COMPLETE) {
              /* receive command from the target */
              if (commandResponse(S, IOBufferPtr, &commandResponseType) !=
                  XILHOSTAPPSVC_SUCCESS) {
                callStopHookAndFreeSFcnMemory(S);
                return;
              }                        /* if */

              if (IOBufferPtr->readData) {
                uint8_T responseId = 0;
                while (IOBufferPtr->readData) {
                  /* read response id */
                  if (xilReadData(S, &responseId, 1, MEM_UNIT_UINT8_TYPE) !=
                      XILHOSTAPPSVC_SUCCESS) {
                    return;
                  }                    /* if */

                  switch (responseId) {
                   case RESPONSE_ERROR:
                   case RESPONSE_PRINTF:
                   case RESPONSE_FOPEN:
                   case RESPONSE_FPRINTF:
                   case RESPONSE_EXCEPTION:
                   case RESPONSE_SIGNAL_RAISED:
                    {
                      if (processErrorAndTargetIOResponseCases(S, responseId)==
                          XILHOSTAPPSVC_ERROR) {
                        callStopHookAndFreeSFcnMemory(S);
                        return;
                      }                /* if */
                      break;
                    }

                   default:
                    {
                      {
                        void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
                        mxArray * rhs[ 2 ];
                        rhs[0] = mxCreateString(
                          "PIL:pilverification:UnknownResponseId");
                        rhs[1] = mxCreateDoubleScalar(responseId);
                        xilUtilsHandleError(pXILUtils, 2 , rhs );
                        mxDestroyArray(rhs[0]);
                        mxDestroyArray(rhs[1]);
                        callStopHookAndFreeSFcnMemory(S);
                        return;
                      }
                      break;
                    }
                  }                    /* switch */
                }                      /* while */
              }                        /* if */
            }                          /* while */
          }
        }                              /* if */
      }

      {
        XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
        if (IOBufferPtr != NULL) {
          void * pXILService = (void *) ssGetPWorkValue(S, 9);
          if (pXILService != NULL) {
            if (xilHostAppSvcGrowIOBuffer(pXILService, IOBufferPtr, 1, 0)!=
                XILHOSTAPPSVC_SUCCESS) {
              callStopHookAndFreeSFcnMemory(S);
              return;
            }                          /* if */

            IOBufferPtr->bufferDataSize = 0;
            if (IOBufferPtr->data != NULL) {
              /* write command id */
              {
                uint8_T commandDataArg = (uint8_T) XIL_SHUTDOWN_COMMAND;
                if (xilWriteData(S, &commandDataArg, 1, MEM_UNIT_UINT8_TYPE) !=
                    XILHOSTAPPSVC_SUCCESS) {
                  return;
                }                      /* if */
              }
            }                          /* if */
          }                            /* if */
        }                              /* if */
      }

      {
        XIL_IOBuffer_T * IOBufferPtr = (XIL_IOBuffer_T *) ssGetPWorkValue(S, 1);
        if (IOBufferPtr != NULL) {
          /* dispatch command to the target */
          if (commandDispatch(S, IOBufferPtr, IOBufferPtr->bufferDataSize)!=
              XILHOSTAPPSVC_SUCCESS) {
            callStopHookAndFreeSFcnMemory(S);
            return;
          }                            /* if */
        }                              /* if */
      }
    }                                  /* if */
  }                                    /* if */

  callStopHookAndFreeSFcnMemory(S);
}

#define MDL_CLEANUP_RUNTIME_RESOURCES                            /* Change to #undef to remove function */
#if defined(MDL_CLEANUP_RUNTIME_RESOURCES)

static void mdlCleanupRuntimeResources(SimStruct *S)
{
  mdlTerminate(S);
  if (ssRTWGenIsCodeGen(S) || ssIsExternalSim(S)) {
    return;
  }                                    /* if */

  if (ssGetPWork(S) != NULL) {
    int * pIsXILApplicationStarted = (int *) ssGetPWorkValue(S, 5);
    int * pCommErrorOccurred = (int *) ssGetPWorkValue(S, 4);
    if (pIsXILApplicationStarted != NULL) {
      mxFree(pIsXILApplicationStarted);
      ssSetPWorkValue(S, 5, NULL);
    }                                  /* if */

    if (pCommErrorOccurred != NULL) {
      mxFree(pCommErrorOccurred);
      ssSetPWorkValue(S, 4, NULL);
    }                                  /* if */
  }                                    /* if */

  if (ssGetPWork(S) != NULL) {
    void * pXILUtils = (void *) ssGetPWorkValue(S, 6);
    if (pXILUtils != NULL) {
      xilUtilsDestroy(pXILUtils);
      ssSetPWorkValue(S, 6, NULL);
    }                                  /* if */
  }                                    /* if */
}

#endif                                 /* MDL_CLEANUP_RUNTIME_RESOURCES */

#define MDL_ENABLE
#if defined(MDL_ENABLE)

static void mdlEnable(SimStruct *S)
{
}

#endif                                 /* MDL_ENABLE */

#define MDL_DISABLE
#if defined(MDL_DISABLE)

static void mdlDisable(SimStruct *S)
{
}

#endif                                 /* MDL_DISABLE */

/* Required S-function trailer */
#ifdef MATLAB_MEX_FILE                 /* Is this file being compiled as a MEX-file? */
#include "simulink.c"                  /* MEX-file interface mechanism */
#include "fixedpoint.c"
#else
#error Assertion failed: file must be compiled as a MEX-file
#endif
