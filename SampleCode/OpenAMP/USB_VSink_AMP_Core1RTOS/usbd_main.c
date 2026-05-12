/**************************************************************************//**
 * @file     usbd_main.c
 *
 * @brief
 *           This project demonstrates a real-time H.264 video decoding pipeline
 *           on MA35D1 Core1 (A35) running FreeRTOS. It receives H.264 bitstreams
 *           from a PC via USB and outputs to MA35D1 display.
 *
 *           Key Features:
 *           - USB Video Sink: Receives raw H.264 bitstream from PC Host.
 *
 * @note     - VC8000 VPU is controlled by Core1 for deterministic latency.
 *             the USB stack, Decoder, and Display Unit.
 *
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "NuMicro.h"
#include "hsusbd.h"
#include "usbd_video_sink.h"
#include "FreeRTOS.h"
#include "task.h"

uint8_t _rx_mem_poll[VSINK_BUFF_SIZE] __attribute__((aligned(32)));

uint32_t q_head = 0, q_tail = 0;

#define GET_QUEUE_DATA_COUNT()  ((q_tail - q_head + VSINK_BUFF_SIZE) % VSINK_BUFF_SIZE)
#define GET_QUEUE_FREE_COUNT()  (VSINK_BUFF_SIZE - GET_QUEUE_DATA_COUNT() - 1)

volatile uint32_t g_tmr2_ticks;

void TMR2_IRQHandler(void)
{
    g_tmr2_ticks++;
    TIMER_ClearIntFlag(TIMER2);
}

void Delay0(uint32_t ticks)
{
    uint32_t tgtTicks = g_tmr2_ticks + ticks;

    while (g_tmr2_ticks < tgtTicks);
}

/**
 * @brief  Reads a fixed amount of data from the global video sink circular buffer.
 *
 * @param[out] pdest     Pointer to the destination buffer to store the data.
 * @param[in]  len       The exact number of bytes required to be read.
 *
 * @return uint32_t      The number of bytes read (returns 'len' on success, 0 on failure).
 */
uint32_t vsink_rxq_read(uint8_t *pdest, uint32_t len)
{
    uint8_t *rx_buff = nc_ptr(_rx_mem_poll);
    uint32_t first_part;

    if (GET_QUEUE_DATA_COUNT() < len)
        return 0;

    if (q_head + len <= VSINK_BUFF_SIZE)
    {
        memcpy(pdest, &rx_buff[q_head], len);
    }
    else
    {
        first_part = VSINK_BUFF_SIZE - q_head;
        memcpy(pdest, &rx_buff[q_head], first_part);
        memcpy(&pdest[first_part], &rx_buff[0], len - first_part);
    }

    q_head = (q_head + len) % VSINK_BUFF_SIZE;
    return len;
}

void vUSBD_MainTask(void *pvParameters)
{
    uint8_t *rx_buff;
    uint32_t t0;

    rx_buff = nc_ptr(_rx_mem_poll);
    q_head = q_tail = 0;

    sysprintf("\n\nUSBD VideoSink Task\n");

    while (1)
    {
        /*
         *  Wait for USB device connected and configured
         */
        while(1)
        {
            if (HSUSBD_IS_ATTACHED())
            {
                HSUSBD_Start();

                t0 = g_tmr2_ticks;

                while (g_tmr2_ticks - t0 < 1000)
                {
                    if (g_hsusbd_Configured)
                        break;
                }

                if (HSUSBD_IS_ATTACHED() && g_hsusbd_Configured)
                    break;
            }
        }

        /*
         *  Wait for USB device connected and configured
         */
        while (HSUSBD_IS_ATTACHED() && g_hsusbd_Configured)
        {
            if (usbd_vsink_bulk_rx(&rx_buff[q_tail]) == 0)
                q_tail = (q_tail +  VSINK_CHUNK_SIZE) % VSINK_BUFF_SIZE;

            while (GET_QUEUE_FREE_COUNT() <  VSINK_CHUNK_SIZE * 2)
            {
                // sysprintf("RXQ Full.\n");
                vTaskDelay(100);
            }
        }
    }
}

int usbd_init(void)
{
    int ret;

    SYS_UnlockReg();

    sysprintf("usbd_init\n");
    fflush(stdout);

    /*
     *  Init TIMER 2
     */
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, TMR2_TICKS_PER_SEC);

    g_tmr2_ticks = 0;
    TIMER_EnableInt(TIMER2);
    IRQ_SetHandler((IRQn_ID_t)TMR2_IRQn, TMR2_IRQHandler);
    IRQ_Enable((IRQn_ID_t)TMR2_IRQn);

    TIMER_Start(TIMER2);

    /*
     *  Init USB Device
     */
    CLK_EnableModuleClock(USBD_MODULE);

    /* HSUSBD VBUS detect pin - PF15 */
    SYS->GPF_MFPH &= ~SYS_GPE_MFPH_PE15MFP_Msk;
    SYS->GPF_MFPH |= SYS_GPF_MFPH_PF15MFP_HSUSB0_VBUSVLD;

    SYS_ResetModule(HSUSBD_RST);

    HSUSBD_Open(&gsHSInfo, usbd_vsink_class_request, NULL);

    /* Endpoint configuration */
    usbd_vsink_config();

    /* Enable USBD interrupt */
    IRQ_SetHandler((IRQn_ID_t)HSUSBD_IRQn, HSUSBD_IRQHandler);
    IRQ_Enable((IRQn_ID_t)HSUSBD_IRQn);

    xTaskCreate(vUSBD_MainTask, "USBDMAIN", configMINIMAL_STACK_SIZE, NULL,
                tskIDLE_PRIORITY + 1, NULL);

    return 0;
}
