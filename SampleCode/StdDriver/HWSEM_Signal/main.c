/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate hardware semaphore (HWSEM) signal function with
 *           interrupt between A35 core and M4 core.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#define HWSEM_CH_A35     0
#define HWSEM_CH_CM4     4
#define A35_KEY          0xAA

extern uint32_t rtp_file_data, rtp_file_end;

void UART0_Init()
{
    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Set multi-function pins */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Init UART to 115200-8n1 for print message */
	UART_Open(UART0, 115200);
}

static uint32_t flag = 0;
void HWSEM_IRQHandler(void)
{
	uint32_t reg;

	reg = HWSEM0->INTSTSA35;

	flag = HWSEM_GET_INT_FLAG(HWSEM0, HWSEM_CH_CM4);

	// Clear flags
	HWSEM0->INTSTSA35 = reg;
}

void HWSEM_Init()
{
	// Enable clock
	CLK_EnableModuleClock(HWS_MODULE);

	// Reset HWSEM
    SYS->IPRST0 = SYS_IPRST0_HWSEMRST_Msk;
    SYS->IPRST0 = 0;

	HWSEM_ENABLE_INT(HWSEM0, HWSEM_CH_CM4);

	IRQ_SetHandler(HWSEM0_IRQn, HWSEM_IRQHandler);
	IRQ_Enable(HWSEM0_IRQn);
}

/* Signal to M4 and check status */
void Signal_Task(uint32_t ch, uint8_t key)
{
	/* Try lock */
	HWSEM_Spin_Lock(HWSEM0, ch, key);

	/* Critical section */

	/* Unlock to issue INT */
	HWSEM_UNLOCK(HWSEM0, ch, key);

	// Check lock & INT status, M4 should clear this
	if(HWSEM_CHK_INT_FLAG(HWSEM0, ch))
	{
		sysprintf("---Signal to M4---\n");
	}
}

/* main function */
int main(void)
{
	uint32_t u32ImageSize = 0;

	SYS_UnlockReg();

	HWSEM_Init();

	UART0_Init();

	/* Enable RTP M4 clock */
	CLK_EnableModuleClock(RTPST_MODULE);

	/* Assign UART16 to RTP M4 */
	SSPCC->PSSET8  |= SSPCC_PSSET8_UART16_Msk;
	SSPCC->IOKSSET |= (SSPCC_IOxSSET_PIN2_Msk | SSPCC_IOxSSET_PIN3_Msk);

	/* Assign GPK multi-function pins for UART RXD and TXD */
	SYS->GPK_MFPL &= ~(SYS_GPK_MFPL_PK2MFP_Msk | SYS_GPK_MFPL_PK3MFP_Msk);
	SYS->GPK_MFPL |= (SYS_GPK_MFPL_PK2MFP_UART16_RXD | SYS_GPK_MFPL_PK3MFP_UART16_TXD);

    /* Load executable image to RTP M4 SRAM and disable RTP M4 core reset */
    u32ImageSize = ptr_to_u32(&rtp_file_end) - ptr_to_u32(&rtp_file_data);
    dcache_clean_invalidate_by_mva(&rtp_file_data, u32ImageSize);
#if 0
    memcpy((void *) 0x24000000, &rtp_file_data, u32ImageSize);
#else
    for(int i = 0; i < u32ImageSize; i++)
    {
    	*((uint8_t *)0x24000000 + i) = *((uint8_t *)&rtp_file_data + i);
    }
#endif
    SYS->IPRST0 &= ~SYS_IPRST0_CM4RST_Msk;  /* Note: This bit is write protected. */

	global_timer_init();

	sysprintf("Run A35 & M4 first, press any key to start demo...\n");
	sysgetchar();

	// Issue a signal
	Signal_Task(HWSEM_CH_A35, A35_KEY);

	while(1)
	{
		if(flag)
		{
			flag = 0;
			sysprintf("---Get signal from M4---\n");
		}
	}
}
