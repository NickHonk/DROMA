/*
 * File: mcu_ca.h
 *
 * Abstract: Tests assumptions in the generated code.
 */

#ifndef MCU_CA_H
#define MCU_CA_H

/* preprocessor validation checks */
#include "mcu_ca_preproc.h"
#include "coder_assumptions_hwimpl.h"

/* variables holding test results */
extern CA_ChecksTestResults CA_mcu_Res;
extern CA_PWS_TestResults CA_mcu_PWSRes;

/* variables holding "expected" and "actual" hardware implementation */
extern const CA_Checks CA_mcu_Exp;
extern CA_Checks CA_mcu_Act;
extern const int numberOfImportedTypes;

/* entry point function to run tests */
void mcu_caRunTests(void);

#endif                                 /* MCU_CA_H */
