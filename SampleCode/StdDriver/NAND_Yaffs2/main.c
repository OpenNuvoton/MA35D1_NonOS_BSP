/******************************************************************************
 * @file     main.c
 *
 * @brief    Access a NAND flash formatted in YAFFS2 file system
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <string.h>
#include "stdio.h"

#include "NuMicro.h"
#include "yaffs_glue.h"

/*--------------------------------------------------------------------------*/
/* Delay execution for given amount of ticks */
void Delay0(uint32_t ticks)  {
	uint32_t tgtTicks = msTicks0 + ticks;             // target tick count to delay
	while (msTicks0 < tgtTicks);
}

void UART0_Init()
{
    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Set multi-function pins */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Initial UART to 115200-8n1 for print message */
	UART_Open(UART0, 115200);
}


void SYS_Init(void)
{
    /*----------------------------------------------------------------------*/
    /* System Clock Initial                                                 */
    /*----------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(NAND_MODULE);

	/*----------------------------------------------------------------------*/
    /* I/O Multi-function Initial                                           */
    /*----------------------------------------------------------------------*/
    /* NAND - PA0 ~ PA14 */
	SYS->GPA_MFPH = 0x06666666;
	SYS->GPA_MFPL = 0x66666666;

    /* Lock protected registers */
    SYS_LockReg();
}

/************************************************/
extern void nand_init(void);

/*----------------------------------------------*/
/* Global variables                             */
/*----------------------------------------------*/
static char CommandLine[256];

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/
void get_line (char *buff, int len)
{
    char c;
    int idx = 0;

    for (;;)
    {
        c = sysgetchar();
        sysputchar(c);
        if (c == '\r') break;
        if ((c == '\b') && idx) idx--;
        if ((c >= ' ') && (idx < len - 1)) buff[idx++] = c;
    }
    buff[idx] = 0;

    sysputchar('\n');
}



int main(void)
{
    char *ptr;
    char mtpoint[] = "user";
    int volatile i;
    unsigned int volatile btime, etime;

    /* Initialize UART to 115200-8n1 for print message */
    UART0_Init();

    global_timer_init();

    /* Initialize System, IP clock and multi-function I/O */
    SYS_Init();

    sysprintf("\n");
    sysprintf("==========================================\n");
    sysprintf("          MA35D1 NAND YAFFS2              \n");
    sysprintf("==========================================\n");

    nand_init();
    cmd_yaffs_devconfig(mtpoint, 0, 1000, 4000);
    cmd_yaffs_dev_ls();
    cmd_yaffs_mount(mtpoint);
    cmd_yaffs_dev_ls();
    sysprintf("\n");

    for (;;)
    {

        sysprintf(">");
        ptr = CommandLine;
        get_line(ptr, sizeof(CommandLine));
        switch (*ptr++)
        {

        case 'q' :  /* Exit program */
            cmd_yaffs_umount(mtpoint);
            sysprintf("Program terminated!\n");
            return 0;

        case 'l' :  /* ls */
            if (*ptr++ == 's')
            {
                while (*ptr == ' ') ptr++;
                cmd_yaffs_ls(ptr, 1);
            }
            break;

        case 'w' :  /* wr */
            if (*ptr++ == 'r')
            {
                while (*ptr == ' ') ptr++;
                btime = msTicks0;
                cmd_yaffs_write_file(ptr, 0x55, 1*1024);    /* write 0x55 into file 10 times */
                etime = msTicks0 - btime;
                sysprintf("write %d MB/sec\n", 1*1024*100/etime);
            }
            break;

        case 'r' :
            if (*ptr == 'd')    /* rd */
            {
                ptr++;
                while (*ptr == ' ') ptr++;
                sysprintf("Reading file %s ...\n\n", ptr);
                btime = msTicks0;
                cmd_yaffs_read_file(ptr);
                etime = msTicks0 - btime;
                sysprintf("read %d MB/sec\n", 1*1024*100/etime);
                sysprintf("\ndone.\n");
            }
            else if (*ptr == 'm')    /* rm */
            {
                ptr++;
                if (*ptr == 'd')
                {
                    while(*ptr != ' ')
                        i = *ptr++;
                    ptr++;
                    sysprintf("Remove dir %s ...\n\n", ptr);
                    cmd_yaffs_rmdir(ptr);
                }
                else
                {
                    while (*ptr == ' ') ptr++;
                    sysprintf("Remove file %s ...\n\n", ptr);
                    cmd_yaffs_rm(ptr);
                }
            }
            break;

        case 'm' :  /* mkdir */
            if (*ptr == 'k')
            {
                ptr++;
                if (*ptr == 'd')
                {
                    while(*ptr != ' ')
                        i = *ptr++;
                    ptr++;
                    cmd_yaffs_mkdir(ptr);
                }
            }
            break;

        case '?':       /* Show usage */
            sysprintf("ls    <path>     - Show a directory. ex: ls user/test ('user' is mount point).\n");
            sysprintf("rd    <file name> - Read a file. ex: rd user/test.bin ('user' is mount point).\n");
            sysprintf("wr    <file name> - Write a file. ex: wr user/test.bin ('user' is mount point).\n");
            sysprintf("rm    <file name> - Delete a file. ex: rm user/test.bin ('user' is mount point).\n");
            sysprintf("mkdir <dir name> - Create a directory. ex: mkdir user/test ('user' is mount point).\n");
            sysprintf("rmdir <dir name> - Create a directory. ex: mkdir user/test ('user' is mount point).\n");
            sysprintf("\n");
        }
    }
}



/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
