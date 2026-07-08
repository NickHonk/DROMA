/*
 * File: coder_assumptions.h
 *
 * Abstract: Coder assumptions header file
 */

#ifndef CODER_ASSUMPTIONS_H
#define CODER_ASSUMPTIONS_H

/* include model specific checks */
#include "mcu_ca.h"

/* global results variable mapping for static code */
#define CA_Expected                    CA_mcu_Exp
#define CA_Actual                      CA_mcu_Act
#define CA_TestResults                 CA_mcu_Res
#define CA_PortableWordSizes_Results   CA_mcu_PWSRes

/* entry point function mapping for static code */
#define CA_Run_Tests                   mcu_caRunTests
#define CA_MODEL_SPECIFIC_CHECKS_ENABLED 0
#endif                                 /* CODER_ASSUMPTIONS_H */
