/**************************************************************************//**
 * @file     ma35d1_touch.h
 *
 * @brief    Touch calibration sample header file
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "fbutils.h"
#include "testutils.h"
#include "ts_calibrate.h"
#include "tslib.h"
#include "displib.h"

// touch
#define MAX_SAMPLES 128
#define Z_TH    50

// display
#define DDR_ADR_FRAMEBUFFER   0x82000000UL
#define DISP_RESOLUTION_X     1024
#define DISP_RESOLUTION_Y     600

extern int ts_mode;

void ts_calibrate();
void ts_test();
