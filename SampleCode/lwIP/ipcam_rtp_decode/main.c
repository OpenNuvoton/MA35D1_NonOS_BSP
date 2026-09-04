/**************************************************************************//**
 * @file     main.c
 *
 * @brief    FreeRTOS + LwIP + VC8000 H.264 RTP decoding to LVDS LCD
 *
 *****************************************************************************/

/* Nuvoton includes */
#include "NuMicro.h"
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "displib.h"
/* lwIP includes */
#include "lwipopts.h"
#include "lwip/tcpip.h"
#include "netif/ethernetif.h"
//#include "udp_echoserver-netconn.h"
#if (LWIP_DHCP == 1)
#include "lwip/dhcp.h"
#endif
/* VC8K includes */
#include "vc8000_lib.h"
#include "vc8k_decode.h"
/* Ring buffer */
#include "rtp_ring.h"

#define UDP_TASK_PRIORITY        ( tskIDLE_PRIORITY + 1UL )
#define UDP_THREAD_STACKSIZE     ( 1024 )


/* FreeRTOS hook functions */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    sysprintf( "ASSERT!  MallocFailed\r\n");
    for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    taskDISABLE_INTERRUPTS();
    sysprintf( "ASSERT!  StackOverflow\r\n");
    for( ;; );
}

void vApplicationIdleHook( void )
{
    volatile size_t xFreeHeapSpace;
    xFreeHeapSpace = xPortGetFreeHeapSize();
    ( void ) xFreeHeapSpace;
}

void vApplicationTickHook( void )
{
#if( mainSELECTED_APPLICATION == 1 )
    {
        extern void vFullDemoTickHook( void );
        vFullDemoTickHook();
    }
#endif
}

/* configUSE_STATIC_ALLOCATION = 1 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* Timer task memory */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vMainAssertCalled( const char *pcFileName, uint32_t ulLineNumber )
{
    sysprintf( "ASSERT!  Line %lu of file %s\r\n", ulLineNumber, pcFileName );
    taskENTER_CRITICAL();
    for( ;; );
}

/*--------------------------Display--------------------------*/
uint8_t s_au8Primary[1280 * 800 * 4 * 3] __attribute__((aligned(128)));
extern uint32_t ImageDataBase, ImageDataLimit;

/* LCD attributes 1024x600 */
DISP_LCD_INFO LcdPanelInfo =
{
	/* Panel Resolution */
	1024,
	600,
	/* DISP_LCD_TIMING */
	{
		51000000,
		1024,
		1,
		160,
		160,
		600,
		1,
		23,
		12,
		ePolarity_Positive,
		ePolarity_Positive
	},
	/* DISP_PANEL_CONF */
	{
		eDPIFmt_D24,
		ePolarity_Positive,
		ePolarity_Positive,
		ePolarity_Positive
	},
};

void DISP_Open(void)
{
    /* Set EPLL/2 as DISP Core Clock source */
    DISP_EnableDCUClk();

    /* Waiting EPLL ready */
    CLK_WaitClockReady(CLK_STATUS_STABLE_EPLL);

    /* Enable DISP Core Clock */
    CLK_EnableModuleClock(DCU_MODULE);

    /* Select DISP Core Clock source */
    CLK_SetModuleClock(DCU_MODULE, CLK_CLKSEL0_DCUSEL_EPLL_DIV2, 0);
    CLK_SetModuleClock(DCUP_MODULE, CLK_CLKSEL0_DCUSEL_EPLL_DIV2, 0);

    /* Select DISP pixel clock source to VPLL */
    DISP_GeneratePixelClk(LcdPanelInfo.sLcdTiming.u32PCF);

    /* Set multi-function pins for LCD Display Controller */
    SYS->GPG_MFPH = SYS->GPG_MFPH &
                    ~(SYS_GPG_MFPH_PG8MFP_Msk | SYS_GPG_MFPH_PG9MFP_Msk | SYS_GPG_MFPH_PG10MFP_Msk) |
                    (SYS_GPG_MFPH_PG8MFP_LCM_VSYNC |
                     SYS_GPG_MFPH_PG9MFP_LCM_HSYNC |
                     SYS_GPG_MFPH_PG10MFP_LCM_CLK);
    SYS->GPK_MFPL = SYS->GPK_MFPL &
                    ~(SYS_GPK_MFPL_PK4MFP_Msk) |
                    SYS_GPK_MFPL_PK4MFP_LCM_DEN;
    SYS->GPI_MFPH = SYS->GPI_MFPH &
                    ~(SYS_GPI_MFPH_PI8MFP_Msk | SYS_GPI_MFPH_PI9MFP_Msk |
                      SYS_GPI_MFPH_PI10MFP_Msk | SYS_GPI_MFPH_PI11MFP_Msk |
                      SYS_GPI_MFPH_PI12MFP_Msk | SYS_GPI_MFPH_PI13MFP_Msk |
                      SYS_GPI_MFPH_PI14MFP_Msk | SYS_GPI_MFPH_PI15MFP_Msk);
    SYS->GPI_MFPH |= (SYS_GPI_MFPH_PI8MFP_LCM_DATA0 |
                      SYS_GPI_MFPH_PI9MFP_LCM_DATA1 |
                      SYS_GPI_MFPH_PI10MFP_LCM_DATA2 |
                      SYS_GPI_MFPH_PI11MFP_LCM_DATA3 |
                      SYS_GPI_MFPH_PI12MFP_LCM_DATA4 |
                      SYS_GPI_MFPH_PI13MFP_LCM_DATA5 |
                      SYS_GPI_MFPH_PI14MFP_LCM_DATA6 |
                      SYS_GPI_MFPH_PI15MFP_LCM_DATA7);
    SYS->GPH_MFPL = SYS->GPH_MFPL &
                    ~(SYS_GPH_MFPL_PH0MFP_Msk | SYS_GPH_MFPL_PH1MFP_Msk |
                      SYS_GPH_MFPL_PH2MFP_Msk | SYS_GPH_MFPL_PH3MFP_Msk |
                      SYS_GPH_MFPL_PH4MFP_Msk | SYS_GPH_MFPL_PH5MFP_Msk |
                      SYS_GPH_MFPL_PH6MFP_Msk | SYS_GPH_MFPL_PH7MFP_Msk);
    SYS->GPH_MFPL |= (SYS_GPH_MFPL_PH0MFP_LCM_DATA8 |
                      SYS_GPH_MFPL_PH1MFP_LCM_DATA9 |
                      SYS_GPH_MFPL_PH2MFP_LCM_DATA10 |
                      SYS_GPH_MFPL_PH3MFP_LCM_DATA11 |
                      SYS_GPH_MFPL_PH4MFP_LCM_DATA12 |
                      SYS_GPH_MFPL_PH5MFP_LCM_DATA13 |
                      SYS_GPH_MFPL_PH6MFP_LCM_DATA14 |
                      SYS_GPH_MFPL_PH7MFP_LCM_DATA15);
    SYS->GPC_MFPH = SYS->GPC_MFPH &
                    ~(SYS_GPC_MFPH_PC12MFP_Msk | SYS_GPC_MFPH_PC13MFP_Msk |
                      SYS_GPC_MFPH_PC14MFP_Msk | SYS_GPC_MFPH_PC15MFP_Msk);
    SYS->GPC_MFPH |= (SYS_GPC_MFPH_PC12MFP_LCM_DATA16 |
                      SYS_GPC_MFPH_PC13MFP_LCM_DATA17 |
                      SYS_GPC_MFPH_PC14MFP_LCM_DATA18 |
                      SYS_GPC_MFPH_PC15MFP_LCM_DATA19);
    SYS->GPH_MFPH = SYS->GPH_MFPH &
                    ~(SYS_GPH_MFPH_PH12MFP_Msk | SYS_GPH_MFPH_PH13MFP_Msk |
                      SYS_GPH_MFPH_PH14MFP_Msk | SYS_GPH_MFPH_PH15MFP_Msk);
    SYS->GPH_MFPH |= (SYS_GPH_MFPH_PH12MFP_LCM_DATA20 |
                      SYS_GPH_MFPH_PH13MFP_LCM_DATA21 |
                      SYS_GPH_MFPH_PH14MFP_LCM_DATA22 |
                      SYS_GPH_MFPH_PH15MFP_LCM_DATA23);

    /* PK.5 backlight enable */
    GPIO_SetMode(PK, BIT5, GPIO_MODE_OUTPUT);
    PK5 = 1;
}

void UART0_Init(void)
{
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    UART_Open(UART0, 115200);
}

void SYS_Init(void)
{
    SYS_UnlockReg();

    SystemCoreClockUpdate();

    UART0_Init();

    /* Enable VC8000 clock */
    CLK_EnableModuleClock(VDEC_MODULE);

    /* Configure EPLL = 500MHz */
    CLK->PLL[EPLL].CTL0 = (6 << CLK_PLLnCTL0_INDIV_Pos) |
                          (250 << CLK_PLLnCTL0_FBDIV_Pos); // M=6, N=250
    CLK->PLL[EPLL].CTL1 = 2 << CLK_PLLnCTL1_OUTDIV_Pos;    // EPLL divide by 2 and enable
    CLK_WaitClockReady(CLK_STATUS_STABLE_EPLL);

    /* DDR Init (magic values from original sample) */
    outp32(UMCTL2_BASE + 0x6a0, 0x01);

    /* HWSEM for DDR */
    CLK_EnableModuleClock(HWS_MODULE);
    SYS->IPRST0 = SYS_IPRST0_HWSEMRST_Msk;
    SYS->IPRST0 = 0;

    /* Do not lock SYS registers to make debugging easier */
    //SYS_LockReg();
}

static netif_init_fn ethernetif_init(int intf)
{
    netif_init_fn fn;

    if (intf == GMACINTF0)
        fn = ethernetif_init0;
    else
        fn = ethernetif_init1;

    return fn;
}

struct netif netif;

/* Defined externally in udp_echoserver-netconn.c */
// extern void vc8k_decode_task(void *arg);
// extern void ring_dump_task(void *arg);
extern void udp_echoserver_netconn_init(void);
/* vUdpTask: set IP, create RTP receiver, and create VC8K decode task */
static void vUdpTask( void *pvParameters )
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    ( void ) pvParameters;

#ifdef IPCAM_RTP
    IP4_ADDR(&ipaddr,   192,168,40,2);
    IP4_ADDR(&netmask,  255,255,255,0);
    IP4_ADDR(&gw,       192,168,40,1);
#else
    IP4_ADDR(&ipaddr,   192,168,1,1);
    IP4_ADDR(&netmask,  255,255,255,0);
    IP4_ADDR(&gw,       192,168,1,1);
#endif

    tcpip_init(NULL, NULL);

    netif_add(&netif, &ipaddr, &netmask, &gw, NULL,
              ethernetif_init(GMAC_INTF), tcpip_input);
    netif_set_default(&netif);
    netif_set_up(&netif);

    sysprintf("[ UDP server ] \n");
    sysprintf("IP address:      %s\n", ip4addr_ntoa(&netif.ip_addr));
    sysprintf("Subnet mask:     %s\n", ip4addr_ntoa(&netif.netmask));
    sysprintf("Default gateway: %s\n", ip4addr_ntoa(&netif.gw));

    udp_echoserver_netconn_init();   /* RTP receive thread is started inside this function */

    vTaskSuspend(NULL);
}

int main(void)
{
    SYS_Init();

    /* ------------------- Clear display framebuffer ------------------- */

    DISP_Open();
    DISPLIB_DDR_AXIPort_Priority();

    sysprintf("\n+------------------------------------------------------------------------+\n");
    sysprintf("|      This sample code shows RGB888 image by RGB to LVDS transmitter    |\n");
    sysprintf("+------------------------------------------------------------------------+\n");

        memset((void *)(nc_ptr(s_au8Primary)), 0, sizeof(s_au8Primary));

    DISPLIB_LCDInit(LcdPanelInfo);
    DISPLIB_SetFBConfig(eFBFmt_A8R8G8B8,
                        LcdPanelInfo.u32ResolutionWidth,
                        LcdPanelInfo.u32ResolutionHeight,
                        ptr_to_u32(s_au8Primary));

    DISPLIB_EnableOutput(eLayer_Video);

    /* ------------------ FreeRTOS + LwIP + VC8K ------------------ */

    global_timer_init();
    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("FreeRTOS is starting ...\n");

    xTaskCreate(vUdpTask, "UdpTask",
                UDP_THREAD_STACKSIZE,
                NULL,
                UDP_TASK_PRIORITY,
                NULL);

    vTaskStartScheduler();

    for (;;);
}
