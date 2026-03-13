/**************************************************************************//**
 * @file     main.h
 * @version  V1.00
 * @brief    Main header file.
 * @copyright Copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef MAIN_H
#define MAIN_H

/* Includes -----------------------------------------------------------------------------------------------*/
#include "NuMicro.h"


#define printf    sysprintf

/* Settings -----------------------------------------------------------------------------------------------*/
#define TEST_W25N01KW            (1)

#define CHECK_4BYTE_ADDRESS_MODE  (0)  // Confirm Status Register-3 ADS bit (option)

/* Exported constants -------------------------------------------------------------------------------------*/
#if (TEST_W25N01KW == 1)
#define QSPI_FLASH_PORT       QSPI0
#define QSPI_CLOCK            30000000

#define FLASH_JEDEC_ID        (0xEFBE21)
#endif

/* Exported macro -----------------------------------------------------------------------------------------*/
#define QSPI_ENABLE_DTR_MODE(qspi)    ( (qspi)->CTL |= QSPI_CTL_DTREN_Msk )
#define QSPI_DISABLE_DTR_MODE(qspi)   ( (qspi)->CTL &= ~QSPI_CTL_DTREN_Msk )

#define QSPI_IS_DTR_ENABLED(qspi)     ( ((qspi)->CTL & QSPI_CTL_DTREN_Msk)    >> QSPI_CTL_DTREN_Pos )
#define QSPI_IS_DUAL_ENABLED(qspi)    ( ((qspi)->CTL & QSPI_CTL_DUALIOEN_Msk) >> QSPI_CTL_DUALIOEN_Pos )
#define QSPI_IS_QUAD_ENABLED(qspi)    ( ((qspi)->CTL & QSPI_CTL_QUADIOEN_Msk) >> QSPI_CTL_QUADIOEN_Pos )
#define QSPI_IS_DIR_INPUT_MODE(qspi)  ( ((qspi)->CTL & QSPI_CTL_DATDIR_Msk) == 0 ? 1 : 0 )
#define QSPI_IS_DIR_OUTPUT_MODE(qspi) ( ((qspi)->CTL & QSPI_CTL_DATDIR_Msk) ? 1 : 0 )

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
// WB_APP.c
extern void SPI_NAND_APP_MainRoutine();

// main.c
extern void Timer0_Delay_us(uint32_t ticks_us);
extern void Timer1_Delay_ms(uint32_t ticks_ms);


#endif  /* __MAIN_H__ */
