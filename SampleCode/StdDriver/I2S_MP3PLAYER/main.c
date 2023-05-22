/**************************************************************************//**
 * @file     main.c
 *
 * @brief    This sample plays MP3 files stored on USB mass storage
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "config.h"
#include "usbh_lib.h"
#include "ff.h"
#include "diskio.h"

extern void PDMA2_IRQHandler(void);
void delay_ms(int msec);

#define MAX_FILE_SIZE   0x800000

DMA_DESC_T PDMA_TXDESC[2];

uint8_t bAudioPlaying = 0;
extern signed int aPCMBuffer[2][PCM_BUFFER_SIZE];

FILINFO MyFinfo;

static volatile uint64_t  _start_time = 0;

void start_timer(void)
{
    _start_time = EL0_GetCurrentPhysicalValue();
}

uint32_t get_ticks(void)
{
    uint64_t   t_off;
    t_off = EL0_GetCurrentPhysicalValue() - _start_time;
    t_off = t_off / 12000;
    return (uint32_t)t_off;
}

/*
 *  This function is necessary for USB Host library.
 */
void delay_us(int usec)
{
    uint64_t   t0;
    t0  = EL0_GetCurrentPhysicalValue();
    while ((EL0_GetCurrentPhysicalValue() - t0) < (usec * 12));
}

void delay_ms(int msec)
{
    uint64_t   t0;
    t0  = EL0_GetCurrentPhysicalValue();
    while ((EL0_GetCurrentPhysicalValue() - t0) < (msec * 12000));
}



/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */

DWORD get_fattime (void)
{
    unsigned long tmr;

    tmr=0x00000;

    return tmr;
}


static FIL file1, file2;        /* File objects */

static void do_dir(char* path)
{
    FILINFO MyFinfo;
    FATFS   *fs;              /* Pointer to file system object */
    char   *ptr = path;
    long   p1, p2;
    UINT   s1, s2, cnt, sector_no;
    DIR    dir;            /* Directory object */
    FRESULT     res;

    res = f_opendir(&dir, ptr);
    if (res)
    {
        sysprintf("f_opendir failed!\n");
        return;
    }
    p1 = s1 = s2 = 0;
    for(;;)
    {
        res = f_readdir(&dir, &MyFinfo);
        if ((res != FR_OK) || !MyFinfo.fname[0]) break;
        if (MyFinfo.fattrib & AM_DIR)
        {
            s2++;
        }
        else
        {
            s1++;
            p1 += MyFinfo.fsize;
        }
        sysprintf("%c%c%c%c%c %d/%02d/%02d %02d:%02d    %9d  %s",
                  (MyFinfo.fattrib & AM_DIR) ? 'D' : '-',
                  (MyFinfo.fattrib & AM_RDO) ? 'R' : '-',
                  (MyFinfo.fattrib & AM_HID) ? 'H' : '-',
                  (MyFinfo.fattrib & AM_SYS) ? 'S' : '-',
                  (MyFinfo.fattrib & AM_ARC) ? 'A' : '-',
                  (MyFinfo.fdate >> 9) + 1980, (MyFinfo.fdate >> 5) & 15, MyFinfo.fdate & 31,
                  (MyFinfo.ftime >> 11), (MyFinfo.ftime >> 5) & 63, MyFinfo.fsize, MyFinfo.fname);
#if _USE_LFN
        for (p2 = strlen(MyFinfo.fname); p2 < 14; p2++)
            sysprintf(" ");
        sysprintf("%s\n", Lfname);
#else
        sysprintf("\n");
#endif
    }
    sysprintf("%4d File(s),%10d bytes total\n%4d Dir(s)", s1, p1, s2);
    if (f_getfree(ptr, (DWORD*)&p1, &fs) == FR_OK)
        sysprintf(", %10d bytes free\n", p1 * fs->csize * 512);
}

void SYS_Init(void)
{
    /* Set APLL to 250 MHz */
    CLK_SetPLLClockFreq(APLL, PLL_OPMODE_INTEGER, FREQ_PLLSRC, 250000000);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable HSUSBH0/1 module clock */
    CLK_EnableModuleClock(HUSBH0_MODULE);
    CLK_EnableModuleClock(HUSBH1_MODULE);
    CLK_EnableModuleClock(USBD_MODULE);
    /* Enable I2S0 module clock */
    CLK_EnableModuleClock(I2S0_MODULE);

    /* Enable I2C2 module clock */
    CLK_EnableModuleClock(I2C2_MODULE);

    /* Enable PDMA module clock */
    CLK_EnableModuleClock(PDMA2_MODULE);

    /* Enable GPIOD module clock */
    CLK_EnableModuleClock(GPD_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* set UHOVRCURH(SYS_MISCFCR0[12]) 1 => USBH Host over-current detect is high-active */
    /*                                 0 => USBH Host over-current detect is low-active  */
    // SYS->MISCFCR0 |= SYS_MISCFCR0_UHOVRCURH_Msk;  /* OVCR is active-high */
    SYS->MISCFCR0 &= ~SYS_MISCFCR0_UHOVRCURH_Msk;    /* OVCR is active-low  */

    /* set PL.12 as MFP9 for HSUSBH_PWREN */
    SYS->GPL_MFPH &= ~SYS_GPL_MFPH_PL12MFP_Msk;
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL12MFP_HSUSBH_PWREN;

    /* set PL.13 as MFP9 for HSUSBH_OVC */
    SYS->GPL_MFPH &= ~SYS_GPL_MFPH_PL13MFP_Msk;
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL13MFP_HSUSBH_OVC;

    /* PB8: I2C2_SDA; PB9: I2C2_SCL */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~(SYS_GPB_MFPH_PB8MFP_Msk|SYS_GPB_MFPH_PB9MFP_Msk))) | SYS_GPB_MFPH_PB8MFP_I2C2_SDA | SYS_GPB_MFPH_PB9MFP_I2C2_SCL;

    /* PK.12(I2S0_LRCK),PK.13(I2S0_BCLK),PK.14(I2S0_DI),PK.15(I2S0_DO), PN.15(I2S0_MCLK) */
    SYS->GPK_MFPH &= ~(SYS_GPK_MFPH_PK12MFP_Msk | SYS_GPK_MFPH_PK13MFP_Msk | SYS_GPK_MFPH_PK14MFP_Msk | SYS_GPK_MFPH_PK15MFP_Msk);
    SYS->GPK_MFPH |= (SYS_GPK_MFPH_PK12MFP_I2S0_LRCK | SYS_GPK_MFPH_PK13MFP_I2S0_BCLK | SYS_GPK_MFPH_PK14MFP_I2S0_DI | SYS_GPK_MFPH_PK15MFP_I2S0_DO);
    /* PN.15(I2S0_MCLK) */
    SYS->GPN_MFPH = (SYS->GPN_MFPH & (~SYS_GPN_MFPH_PN15MFP_Msk)) | SYS_GPN_MFPH_PN15MFP_I2S0_MCLK;
}

int USB_PHY_Init(void)
{
    uint64_t t0;

    /* reset USB PHY */
    SYS->USBPMISCR = (SYS->USBPMISCR & ~0x70007) | SYS_USBPMISCR_PHY0POR_Msk |
                     SYS_USBPMISCR_PHY0SUSPEND_Msk |
                     SYS_USBPMISCR_PHY1POR_Msk | SYS_USBPMISCR_PHY1SUSPEND_Msk;

    delay_us(20000);  /* delay 20 ms */

    /* enter operation mode */
    SYS->USBPMISCR = (SYS->USBPMISCR & ~0x70007) |
                     SYS_USBPMISCR_PHY0SUSPEND_Msk | SYS_USBPMISCR_PHY1SUSPEND_Msk;
    t0 = get_ticks();
    while (1)
    {
        delay_us(10);
        if ((SYS->USBPMISCR & SYS_USBPMISCR_PHY0HSTCKSTB_Msk) &&
                (SYS->USBPMISCR & SYS_USBPMISCR_PHY1HSTCKSTB_Msk))
            break;   /* both USB PHY0 and PHY1 clock 60MHz UTMI clock stable */

        if (get_ticks() - t0 > 500)
        {
            sysprintf("USB PHY reset failed. USBPMISCR = 0x%x\n", SYS->USBPMISCR);
            return -1;
        }
    }
    sysprintf("USBPMISCR = 0x%x\n", SYS->USBPMISCR);
    return 0;
}

void UART0_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

static int is_mp3_file(char *fname)
{
    int  slen = strlen(fname);

    if ((strcasecmp(fname + slen - 4, ".mp3") == 0) ||
            (strcasecmp(fname + slen - 4, ".MP3") == 0))
        return 1;
    return 0;
}

/* Configure PDMA to Scatter Gather mode */
void PDMA_Init(void)
{
    DMA_DESC_T *DMA_TXDESC = nc_ptr(PDMA_TXDESC);

    /* Tx description */
    DMA_TXDESC[0].ctl = ((PCM_BUFFER_SIZE-1)<<PDMA_DSCT_CTL_TXCNT_Pos)|PDMA_WIDTH_32|PDMA_SAR_INC|PDMA_DAR_FIX|PDMA_REQ_SINGLE|PDMA_OP_SCATTER;
    DMA_TXDESC[0].src = ptr_to_u32(&aPCMBuffer[0][0]);
    DMA_TXDESC[0].dest = ptr_to_u32(&I2S0->TXFIFO);
    DMA_TXDESC[0].offset = ptr_to_u32(&DMA_TXDESC[1]);

    DMA_TXDESC[1].ctl = ((PCM_BUFFER_SIZE-1)<<PDMA_DSCT_CTL_TXCNT_Pos)|PDMA_WIDTH_32|PDMA_SAR_INC|PDMA_DAR_FIX|PDMA_REQ_SINGLE|PDMA_OP_SCATTER;
    DMA_TXDESC[1].src = ptr_to_u32(&aPCMBuffer[1][0]);
    DMA_TXDESC[1].dest = ptr_to_u32(&I2S0->TXFIFO);
    DMA_TXDESC[1].offset = ptr_to_u32(&DMA_TXDESC[0]);   //link to first description

    PDMA_Open(PDMA2, 1 << 2);
    PDMA_SetTransferMode(PDMA2, 2, PDMA_I2S0_TX, 1, ptr_to_u32(&DMA_TXDESC[0]));

    PDMA_EnableInt(PDMA2, 2, 0);

    IRQ_SetHandler((IRQn_ID_t)PDMA2_IRQn, PDMA2_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)PDMA2_IRQn);
}

/* Init I2C interface */
void I2C2_Init(void)
{
    /* Open I2C2 and set clock to 100k */
    I2C_Open(I2C2, 100000);

    /* Get I2C2 Bus Clock */
    sysprintf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C2));
}

int32_t main(void)
{
    char     *ptr, *ptr2;
    long     p1, p2, p3;
    BYTE     *buf;
    FATFS    *fs;              /* Pointer to file system object */
    TCHAR    usb_path[] = { '0', ':', 0 };    /* USB drive started from 3 */
    FRESULT  res;

    DIR      dir;     /* Directory object */
    UINT     s1, s2, cnt, sector_no;
    static const BYTE ft[] = {0, 12, 16, 32};
    DWORD ofs = 0, sect = 0;
    FIL   hfile, *pFile;
    uint32_t fsize;
    int   handle, count, ret;
    char      fname[256];

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for sysprintf */
    UART0_Init();

    /* Init I2C2 to access NAU8822 */
    I2C2_Init();

    start_timer();

    if (USB_PHY_Init() != 0)
    {
        sysprintf("USB PHY Init failed!\n");
        while (1);
    }

    sysprintf("\n\n");
    sysprintf("+-----------------------------------------------+\n");
    sysprintf("|                                               |\n");
    sysprintf("|     MP3 Player Sample with audio codec        |\n");
    sysprintf("|                                               |\n");
    sysprintf("+-----------------------------------------------+\n");
    sysprintf(" Please put an MP3 file on USB mass storage \n");

    usbh_core_init();
    usbh_umas_init();
    usbh_pooling_hubs();

    f_chdrive(usb_path);               /* set default path                                */

    for (;;)
    {
        usbh_pooling_hubs();

        usbh_memory_used();            /* print out UsbHostLib memory usage information   */

        sysprintf("Detecting USB disk...\n");
        while (1)
        {
            if (f_opendir(&dir, usb_path) == 0)
            {
                //f_closedir(&dir);
                break;
            }
            delay_ms(200);
            usbh_pooling_hubs();
        }

        p1 = s1 = s2 = 0;
        for(;;)
        {
            res = f_readdir(&dir, &MyFinfo);
            if ((res != FR_OK) || !MyFinfo.fname[0]) break;
            if (MyFinfo.fattrib & AM_DIR)
            {
                s2++;
            }
            else
            {
                s1++;
                p1 += MyFinfo.fsize;
            }
            sysprintf("%c%c%c%c%c %d/%02d/%02d %02d:%02d    %9d  %s",
                      (MyFinfo.fattrib & AM_DIR) ? 'D' : '-',
                      (MyFinfo.fattrib & AM_RDO) ? 'R' : '-',
                      (MyFinfo.fattrib & AM_HID) ? 'H' : '-',
                      (MyFinfo.fattrib & AM_SYS) ? 'S' : '-',
                      (MyFinfo.fattrib & AM_ARC) ? 'A' : '-',
                      (MyFinfo.fdate >> 9) + 1980, (MyFinfo.fdate >> 5) & 15, MyFinfo.fdate & 31,
                      (MyFinfo.ftime >> 11), (MyFinfo.ftime >> 5) & 63, MyFinfo.fsize, MyFinfo.fname);
#if _USE_LFN
            for (p2 = strlen(MyFinfo.fname); p2 < 14; p2++)
                sysprintf(" ");
            sysprintf("%s\n", Lfname);
#else
            sysprintf("\n");
#endif


            if (is_mp3_file(MyFinfo.fname) /*&& (MyFinfo.fattrib & AM_ARC)*/)
            {
                if (MyFinfo.fsize <= MAX_FILE_SIZE)
                {
                    strcpy(fname, usb_path);
                    strcat(fname, "/");
                    strcat(fname, MyFinfo.fname);

                    MP3Player(fname);
                }
            }


        }
        while(1);
    }
}
