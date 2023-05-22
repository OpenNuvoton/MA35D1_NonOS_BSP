/**************************************************************************//**
 * @file     main.c
 *
 * @brief    This sample uses a command-shell-like interface to demonstrate
 *           how to use USBH mass storage driver and make it working as a disk
 *           driver under FATFS file system.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "usbh_lib.h"
#include "ff.h"
#include "diskio.h"

#define BUFF_SIZE       (4*1024)

static UINT blen = BUFF_SIZE;
DWORD acc_size;                         /* Work register for fs command */
WORD acc_files, acc_dirs;
FILINFO Finfo;

char Line[256];                         /* Console input buffer */
#if _USE_LFN
char Lfname[512];
#endif

BYTE  Buff_Pool1[BUFF_SIZE] __attribute__((aligned(32)));  /* Working buffer 1 */
BYTE  Buff_Pool2[BUFF_SIZE] __attribute__((aligned(32)));  /* Working buffer 2 */
BYTE  *Buff1;
BYTE  *Buff2;

static volatile	uint64_t  _start_time =	0;
static volatile uint32_t  _mark_time;

void start_timer(void)
{
	_start_time	= EL0_GetCurrentPhysicalValue();
}

uint32_t get_ticks(void)
{
	uint64_t   t_off;
	t_off =	EL0_GetCurrentPhysicalValue() -	_start_time;
	t_off =	t_off /	12000;
	return (uint32_t)t_off;
}

/*
 *	This function is necessary for USB Host	library.
 */
void delay_us(int usec)
{
	uint64_t   t0;
	t0	= EL0_GetCurrentPhysicalValue();
	while ((EL0_GetCurrentPhysicalValue() -	t0)	< (usec	* 12));
}

static void set_time_mark()
{
	_mark_time = get_ticks();
}

static uint32_t get_elapsed_time()
{
	return (get_ticks() - _mark_time);
}

/*--------------------------------------------------------------------------*/
/* Monitor                                                                  */

/*----------------------------------------------*/
/* Get a value of the string                    */
/*----------------------------------------------*/
/*  "123 -5   0x3ff 0b1111 0377  w "
		^                           1st call returns 123 and next ptr
		   ^                        2nd call returns -5 and next ptr
				   ^                3rd call returns 1023 and next ptr
						  ^         4th call returns 15 and next ptr
							   ^    5th call returns 255 and next ptr
								  ^ 6th call fails and returns 0
*/

int xatoi (         /* 0:Failed, 1:Successful */
	TCHAR **str,    /* Pointer to pointer to the string */
	long *res       /* Pointer to a variable to store the value */
)
{
	unsigned long val;
	unsigned char r, s = 0;
	TCHAR c;


	*res = 0;
	while ((c = **str) == ' ') (*str)++;    /* Skip leading spaces */

	if (c == '-')       /* negative? */
	{
		s = 1;
		c = *(++(*str));
	}

	if (c == '0')
	{
		c = *(++(*str));
		switch (c)
		{
		case 'x':       /* hexadecimal */
			r = 16;
			c = *(++(*str));
			break;
		case 'b':       /* binary */
			r = 2;
			c = *(++(*str));
			break;
		default:
			if (c <= ' ') return 1; /* single zero */
			if (c < '0' || c > '9') return 0;   /* invalid char */
			r = 8;      /* octal */
		}
	}
	else
	{
		if (c < '0' || c > '9') return 0;   /* EOL or invalid char */
		r = 10;         /* decimal */
	}

	val = 0;
	while (c > ' ')
	{
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17)
		{
			c -= 7;
			if (c <= 9) return 0;   /* invalid char */
		}
		if (c >= r) return 0;       /* invalid char for current radix */
		val = val * r + c;
		c = *(++(*str));
	}
	if (s) val = 0 - val;           /* apply sign if needed */

	*res = val;
	return 1;
}


/*----------------------------------------------*/
/* Dump a block of byte array                   */

void put_dump (
	const unsigned char* buff,  /* Pointer to the byte array to be dumped */
	unsigned long addr,         /* Heading address value */
	int cnt                     /* Number of bytes to be dumped */
)
{
	int i;

	sysprintf("%08x ", addr);
	for (i = 0; i < cnt; i++)
		sysprintf(" %02x", buff[i]);

	sysprintf(" ");
	for (i = 0; i < cnt; i++)
		sysputchar((TCHAR)((buff[i] >= ' ' && buff[i] <= '~') ? buff[i] : '.'));
	sysprintf("\n");
}


/*--------------------------------------------------------------------------*/
/* Monitor                                                                  */
/*--------------------------------------------------------------------------*/

static
FRESULT scan_files (
	char* path      /* Pointer to the path name working buffer */
)
{
	DIR dirs;
	FRESULT res;
	BYTE i;
	char *fn;

	if ((res = f_opendir(&dirs, path)) == FR_OK)
	{
		i = strlen(path);
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0])
		{
			if (FF_FS_RPATH && Finfo.fname[0] == '.') continue;
#if _USE_LFN
			fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR)
			{
				acc_dirs++;
				*(path+i) = '/';
				strcpy(path+i+1, fn);
				res = scan_files(path);
				*(path+i) = '\0';
				if (res != FR_OK) break;
			}
			else
			{
				/*              sysprintf("%s/%s\n", path, fn); */
				acc_files++;
				acc_size += Finfo.fsize;
			}
		}
	}
	return res;
}

void put_rc (FRESULT rc)
{
	const TCHAR *p =
		_T("OK\0DISK_ERR\0INT_ERR\0NOT_READY\0NO_FILE\0NO_PATH\0INVALID_NAME\0")
		_T("DENIED\0EXIST\0INVALID_OBJECT\0WRITE_PROTECTED\0INVALID_DRIVE\0")
		_T("NOT_ENABLED\0NO_FILE_SYSTEM\0MKFS_ABORTED\0TIMEOUT\0LOCKED\0")
		_T("NOT_ENOUGH_CORE\0TOO_MANY_OPEN_FILES\0");
	//FRESULT i;
	uint32_t i;
	for (i = 0; (i != (UINT)rc) && *p; i++)
	{
		while(*p++) ;
	}
	sysprintf(_T("rc=%d FR_%s\n"), (UINT)rc, p);
}

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/

void get_line (char *buff, int len)
{
	TCHAR c;
	int idx = 0;
//  DWORD dw;

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
	FILINFO Finfo;
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
	    res = f_readdir(&dir, &Finfo);
	    if ((res != FR_OK) || !Finfo.fname[0]) break;
	    if (Finfo.fattrib & AM_DIR)
	    {
	        s2++;
	    }
	    else
	    {
	        s1++;
	        p1 += Finfo.fsize;
	    }
	    sysprintf("%c%c%c%c%c %d/%02d/%02d %02d:%02d    %9d  %s",
	           (Finfo.fattrib & AM_DIR) ? 'D' : '-',
	           (Finfo.fattrib & AM_RDO) ? 'R' : '-',
	           (Finfo.fattrib & AM_HID) ? 'H' : '-',
	           (Finfo.fattrib & AM_SYS) ? 'S' : '-',
	           (Finfo.fattrib & AM_ARC) ? 'A' : '-',
	           (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
	           (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, Finfo.fsize, Finfo.fname);
#if _USE_LFN
		for (p2 = strlen(Finfo.fname); p2 < 14; p2++)
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
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable HSUSBH0/1 module clock */
    CLK_EnableModuleClock(HUSBH0_MODULE);
    CLK_EnableModuleClock(HUSBH1_MODULE);
    CLK_EnableModuleClock(USBD_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

	/* set UHOVRCURH(SYS_MISCFCR0[12]) 1 => USBH Host over-current detect is high-active */
	/*                                 0 => USBH Host over-current detect is low-active  */
	// SYS->MISCFCR0 |= SYS_MISCFCR0_UHOVRCURH_Msk;  /* OVCR is active-high */
	SYS->MISCFCR0 &= ~SYS_MISCFCR0_UHOVRCURH_Msk;    /* OVCR is active-low  */

	/* set PL.12 as	MFP9 for HSUSBH_PWREN */
	SYS->GPL_MFPH &= ~SYS_GPL_MFPH_PL12MFP_Msk;
	SYS->GPL_MFPH |= SYS_GPL_MFPH_PL12MFP_HSUSBH_PWREN;

	/* set PL.13 as	MFP9 for HSUSBH_OVC	*/
	SYS->GPL_MFPH &= ~SYS_GPL_MFPH_PL13MFP_Msk;
	SYS->GPL_MFPH |= SYS_GPL_MFPH_PL13MFP_HSUSBH_OVC;
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
	while (1) {
		delay_us(10);
		if ((SYS->USBPMISCR & SYS_USBPMISCR_PHY0HSTCKSTB_Msk) &&
		    (SYS->USBPMISCR & SYS_USBPMISCR_PHY1HSTCKSTB_Msk))
		    break;   /* both USB PHY0 and PHY1 clock 60MHz UTMI clock stable */

		if (get_ticks() - t0 > 500) {
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

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for sysprintf */
    UART0_Init();

	start_timer();

	if (USB_PHY_Init() != 0) {
		sysprintf("USB PHY Init failed!\n");
		while (1);
	}

	sysprintf("\n\n");
	sysprintf("+-----------------------------------------------+\n");
	sysprintf("|                                               |\n");
	sysprintf("|     USB Host Mass Storage sample program      |\n");
	sysprintf("|                                               |\n");
	sysprintf("+-----------------------------------------------+\n");

	Buff1 = nc_ptr(Buff_Pool1);
	Buff2 = nc_ptr(Buff_Pool2);

	usbh_core_init();
	usbh_umas_init();
	usbh_pooling_hubs();

	f_chdrive(usb_path);               /* set default path                                */

	for (;;)
	{
		usbh_pooling_hubs();

		usbh_memory_used();            /* print out UsbHostLib memory usage information   */

		sysprintf(_T(">"));
		ptr = Line;

		get_line(ptr, sizeof(Line));

		switch (*ptr++) {

		case 'd' :
			switch (*ptr++)
			{
			case 'd' :  /* dd [<lba>] - Dump sector */
				if (!xatoi(&ptr, &p2)) p2 = sect;
				res = (FRESULT)disk_read(3, Buff1, p2, 1);
				if (res)
				{
					sysprintf("rc=%d\n", (WORD)res);
					break;
				}
				sect = p2 + 1;
				sysprintf("Sector:%d\n", p2);
				for (buf=(unsigned char*)Buff1, ofs = 0; ofs < 0x200; buf+=16, ofs+=16)
					put_dump(buf, ofs, 16);
				break;

			case 't' :  /* dt - raw sector read/write performance test */
				sysprintf("Raw sector read performance test...\n");
				set_time_mark();
				for (s1 = 0; s1 < (0x800000/BUFF_SIZE); s1++)
				{
					s2 = BUFF_SIZE/512;     /* sector count for each read  */
					res = (FRESULT)disk_read(3, Buff1, 10000 + s1 * s2, s2);
					if (res)
					{
						sysprintf("read failed at %d, rc=%d\n", 10000 + s1 * s2, (WORD)res);
						put_rc(res);
						break;
					}
					if (s1 % 256 == 0)
						sysprintf("%d KB\n", (s1*BUFF_SIZE)/1024);
				}
				p1 = get_elapsed_time();
				sysprintf("time = %d.%02d\n", p1/100, p1 % 100);
				sysprintf("Raw read speed: %d KB/s\n", ((0x800000 * 100) / p1)/1024);

				sysprintf("Raw sector write performance test...\n");
				set_time_mark();
				for (s1 = 0; s1 < (0x800000/BUFF_SIZE); s1++)
				{
					s2 = BUFF_SIZE/512;     /* sector count for each read  */
					res = (FRESULT)disk_write(3, Buff1, 20000 + s1 * s2, s2);
					if (res)
					{
						sysprintf("write failed at %d, rc=%d\n", 20000 + s1 * s2, (WORD)res);
						put_rc(res);
						break;
					}
					if (s1 % 256 == 0)
						sysprintf("%d KB\n", (s1*BUFF_SIZE)/1024);
				}
				p1 = get_elapsed_time();
				sysprintf("time = %d.%02d\n", p1/100, p1 % 100);
				sysprintf("Raw write speed: %d KB/s\n", ((0x800000 * 100) / p1)/1024);
				break;

			case 'z' :  /* dz - file read/write performance test */
				sysprintf("File write performance test...\n");
				res = f_open(&file1, "0:\\tfile", FA_CREATE_ALWAYS | FA_WRITE);
				if (res)
				{
					put_rc(res);
					break;
				}
				set_time_mark();
				for (s1 = 0; s1 < (0x800000/BUFF_SIZE); s1++)
				{
					res = f_write(&file1, Buff1, BUFF_SIZE, &cnt);
					if (res || (cnt != BUFF_SIZE))
					{
						put_rc(res);
						break;
					}
					if (s1 % 128 == 0)
						sysprintf("%d KB\n", (s1*BUFF_SIZE)/1024);
				}
				p1 = get_elapsed_time();
				f_close(&file1);
				sysprintf("time = %d.%02d\n", p1/100, p1 % 100);
				sysprintf("File write speed: %d KB/s\n", ((0x800000 * 100) / p1)/1024);

				sysprintf("File read performance test...\n");
				res = f_open(&file1, "0:\\tfile", FA_READ);
				if (res)
				{
					put_rc(res);
					break;
				}
				set_time_mark();
				for (s1 = 0; s1 < (0x800000/BUFF_SIZE); s1++)
				{
					res = f_read(&file1, Buff1, BUFF_SIZE, &cnt);
					if (res || (cnt != BUFF_SIZE))
					{
						put_rc(res);
						break;
					}
					if (s1 % 128 == 0)
						sysprintf("%d KB\n", (s1*BUFF_SIZE)/1024);
				}
				p1 = get_elapsed_time();
				f_close(&file1);
				sysprintf("time = %d.%02d\n", p1/100, p1 % 100);
				sysprintf("File read speed: %d KB/s\n", ((0x800000 * 100) / p1)/1024);
				break;
			}
			break;

		case 'b' :
			switch (*ptr++)
			{
			case 'd' :  /* bd <addr> - Dump R/W buffer */
				if (!xatoi(&ptr, &p1)) break;
				for (ptr=(char*)&Buff1[p1], ofs = p1, cnt = 32; cnt; cnt--, ptr+=16, ofs+=16)
					put_dump((BYTE*)ptr, ofs, 16);
				break;

			case 'e' :  /* be <addr> [<data>] ... - Edit R/W buffer */
				if (!xatoi(&ptr, &p1)) break;
				if (xatoi(&ptr, &p2))
				{
					do
					{
						Buff1[p1++] = (BYTE)p2;
					}
					while (xatoi(&ptr, &p2));
					break;
				}
				for (;;)
				{
					sysprintf("%04X %02X-", (WORD)p1, Buff1[p1]);
					get_line(Line, sizeof(Line));
					ptr = Line;
					if (*ptr == '.') break;
					if (*ptr < ' ')
					{
						p1++;
						continue;
					}
					if (xatoi(&ptr, &p2))
						Buff1[p1++] = (BYTE)p2;
					else
						sysprintf("???\n");
				}
				break;

			case 'r' :  /* br <sector> [<n>] - Read disk into R/W buffer */
				if (!xatoi(&ptr, &p2)) break;
				if (!xatoi(&ptr, &p3)) p3 = 1;
				sysprintf("rc=%d\n", disk_read(3, Buff1, p2, p3));
				break;

			case 'w' :  /* bw <sector> [<n>] - Write R/W buffer into disk */
				if (!xatoi(&ptr, &p2)) break;
				if (!xatoi(&ptr, &p3)) p3 = 1;
				sysprintf("rc=%d\n", disk_write(3, Buff1, p2, p3));
				break;

			case 'f' :  /* bf <n> - Fill working buffer */
				if (!xatoi(&ptr, &p1)) break;
				memset(Buff1, (int)p1, BUFF_SIZE);
				break;

			}
			break;



		case 'f' :
			switch (*ptr++)
			{

			case 's' :  /* fs - Show logical drive status */
				res = f_getfree("", (DWORD*)&p2, &fs);
				if (res)
				{
					put_rc(res);
					break;
				}
				sysprintf("FAT type = FAT%d\nBytes/Cluster = %d\nNumber of FATs = %d\n"
					   "Root DIR entries = %d\nSectors/FAT = %d\nNumber of clusters = %d\n"
					   "FAT start (lba) = %d\nDIR start (lba,clustor) = %d\nData start (lba) = %d\n\n...",
					   ft[fs->fs_type & 3], fs->csize * 512UL, fs->n_fats,
					   fs->n_rootdir, fs->fsize, fs->n_fatent - 2,
					   fs->fatbase, fs->dirbase, fs->database
					  );
				acc_size = acc_files = acc_dirs = 0;
#if _USE_LFN
				Finfo.lfname = Lfname;
				Finfo.lfsize = sizeof(Lfname);
#endif
				res = scan_files(ptr);
				if (res)
				{
					put_rc(res);
					break;
				}
				sysprintf("\r%d files, %d bytes.\n%d folders.\n"
					   "%d KB total disk space.\n%d KB available.\n",
					   acc_files, acc_size, acc_dirs,
					   (fs->n_fatent - 2) * (fs->csize / 2), p2 * (fs->csize / 2)
					  );
				break;
			case 'l' :  /* fl [<path>] - Directory listing */
				while (*ptr == ' ') ptr++;
				res = f_opendir(&dir, ptr);
				if (res)
				{
					put_rc(res);
					break;
				}
				p1 = s1 = s2 = 0;
				for(;;)
				{
					res = f_readdir(&dir, &Finfo);
					if ((res != FR_OK) || !Finfo.fname[0]) break;
					if (Finfo.fattrib & AM_DIR)
					{
						s2++;
					}
					else
					{
						s1++;
						p1 += Finfo.fsize;
					}
					sysprintf("%c%c%c%c%c %d/%02d/%02d %02d:%02d    %9d  %s",
						   (Finfo.fattrib & AM_DIR) ? 'D' : '-',
						   (Finfo.fattrib & AM_RDO) ? 'R' : '-',
						   (Finfo.fattrib & AM_HID) ? 'H' : '-',
						   (Finfo.fattrib & AM_SYS) ? 'S' : '-',
						   (Finfo.fattrib & AM_ARC) ? 'A' : '-',
						   (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
						   (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, Finfo.fsize, Finfo.fname);
#if _USE_LFN
					for (p2 = strlen(Finfo.fname); p2 < 14; p2++)
						sysprintf(" ");
					sysprintf("%s\n", Lfname);
#else
					sysprintf("\n");
#endif
				}
				sysprintf("%4d File(s),%10d bytes total\n%4d Dir(s)", s1, p1, s2);
				if (f_getfree(ptr, (DWORD*)&p1, &fs) == FR_OK)
					sysprintf(", %10d bytes free\n", p1 * fs->csize * 512);
				break;


			case 'o' :  /* fo <mode> <file> - Open a file */
				if (!xatoi(&ptr, &p1)) break;
				while (*ptr == ' ') ptr++;
				put_rc(f_open(&file1, ptr, (BYTE)p1));
				break;

			case 'c' :  /* fc - Close a file */
				put_rc(f_close(&file1));
				break;

			case 'e' :  /* fe - Seek file pointer */
				if (!xatoi(&ptr, &p1)) break;
				res = f_lseek(&file1, p1);
				put_rc(res);
				if (res == FR_OK)
					sysprintf("fptr=%d(0x%lX)\n", file1.fptr, file1.fptr);
				break;

			case 'd' :  /* fd <len> - read and dump file from current fp */
				if (!xatoi(&ptr, &p1)) break;
				ofs = file1.fptr;
				while (p1)
				{
					if ((UINT)p1 >= 16)
					{
						cnt = 16;
						p1 -= 16;
					}
					else
					{
						cnt = p1;
						p1 = 0;
					}
					res = f_read(&file1, Buff1, cnt, &cnt);
					if (res != FR_OK)
					{
						put_rc(res);
						break;
					}
					if (!cnt) break;
					put_dump(Buff1, ofs, cnt);
					ofs += 16;
				}
				break;

			case 'r' :  /* fr <len> - read file */
				if (!xatoi(&ptr, &p1)) break;
				p2 = 0;
				set_time_mark();
				while (p1)
				{
					if ((UINT)p1 >= blen)
					{
						cnt = blen;
						p1 -= blen;
					}
					else
					{
						cnt = p1;
						p1 = 0;
					}
					res = f_read(&file1, Buff1, cnt, &s2);
					if (res != FR_OK)
					{
						put_rc(res);
						break;
					}
					p2 += s2;
					if (cnt != s2) break;
				}
				p1 = get_elapsed_time();
				if (p1)
					sysprintf("%d bytes read with %d kB/sec.\n", p2, ((p2 * 100) / p1)/1024);
				break;

			case 'w' :  /* fw <len> <val> - write file */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
				memset(Buff1, (BYTE)p2, blen);
				p2 = 0;
				set_time_mark();
				while (p1)
				{
					if ((UINT)p1 >= blen)
					{
						cnt = blen;
						p1 -= blen;
					}
					else
					{
						cnt = p1;
						p1 = 0;
					}
					res = f_write(&file1, Buff1, cnt, &s2);
					if (res != FR_OK)
					{
						put_rc(res);
						break;
					}
					p2 += s2;
					if (cnt != s2) break;
				}
				p1 = get_elapsed_time();
				if (p1)
					sysprintf("%d bytes written with %d kB/sec.\n", p2, ((p2 * 100) / p1)/1024);
				break;

			case 'n' :  /* fn <old_name> <new_name> - Change file/dir name */
				while (*ptr == ' ') ptr++;
				ptr2 = strchr(ptr, ' ');
				if (!ptr2) break;
				*ptr2++ = 0;
				while (*ptr2 == ' ') ptr2++;
				put_rc(f_rename(ptr, ptr2));
				break;

			case 'u' :  /* fu <name> - Unlink a file or dir */
				while (*ptr == ' ') ptr++;
				put_rc(f_unlink(ptr));
				break;

			case 'v' :  /* fv - Truncate file */
				put_rc(f_truncate(&file1));
				break;

			case 'k' :  /* fk <name> - Create a directory */
				while (*ptr == ' ') ptr++;
				put_rc(f_mkdir(ptr));
				break;

			case 'a' :  /* fa <atrr> <mask> <name> - Change file/dir attribute */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
				while (*ptr == ' ') ptr++;
				put_rc(f_chmod(ptr, p1, p2));
				break;

			case 't' :  /* ft <year> <month> <day> <hour> <min> <sec> <name> - Change timestamp */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
				Finfo.fdate = (WORD)(((p1 - 1980) << 9) | ((p2 & 15) << 5) | (p3 & 31));
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
				Finfo.ftime = (WORD)(((p1 & 31) << 11) | ((p1 & 63) << 5) | ((p1 >> 1) & 31));
				put_rc(f_utime(ptr, &Finfo));
				break;

			case 'x' : /* fx <src_name> <dst_name> - Copy file */
				while (*ptr == ' ') ptr++;
				ptr2 = strchr(ptr, ' ');
				if (!ptr2) break;
				*ptr2++ = 0;
				while (*ptr2 == ' ') ptr2++;
				sysprintf("Opening \"%s\"", ptr);
				res = f_open(&file1, ptr, FA_OPEN_EXISTING | FA_READ);
				sysprintf("\n");
				if (res)
				{
					put_rc(res);
					break;
				}
				sysprintf("Creating \"%s\"", ptr2);
				res = f_open(&file2, ptr2, FA_CREATE_ALWAYS | FA_WRITE);
				sysputchar('\n');
				if (res)
				{
					put_rc(res);
					f_close(&file1);
					break;
				}
				sysprintf("Copying...");
				p1 = 0;
				for (;;)
				{
					res = f_read(&file1, Buff1, BUFF_SIZE, &s1);
					if (res || s1 == 0) break;   /* error or eof */
					res = f_write(&file2, Buff1, s1, &s2);
					p1 += s2;
					if (res || s2 < s1) break;   /* error or disk full */

					if ((p1 % 0x10000) == 0)
						sysprintf("\n%d KB copyed.", p1/1024);
					sysprintf(".");
				}
				sysprintf("\n%d bytes copied.\n", p1);
				f_close(&file1);
				f_close(&file2);
				break;

			case 'y' : /* fz <src_name> <dst_name> - Compare file */
				while (*ptr == ' ') ptr++;
				ptr2 = strchr(ptr, ' ');
				if (!ptr2) break;
				*ptr2++ = 0;
				while (*ptr2 == ' ') ptr2++;
				sysprintf("Opening \"%s\"", ptr);
				res = f_open(&file1, ptr, FA_OPEN_EXISTING | FA_READ);
				sysprintf("\n");
				if (res)
				{
					put_rc(res);
					break;
				}
				sysprintf("Opening \"%s\"", ptr2);
				res = f_open(&file2, ptr2, FA_OPEN_EXISTING | FA_READ);
				sysputchar('\n');
				if (res)
				{
					put_rc(res);
					f_close(&file1);
					break;
				}
				sysprintf("Compare...");
				p1 = 0;
				for (;;)
				{
					res = f_read(&file1, Buff1, BUFF_SIZE, &s1);
					if (res || s1 == 0)
					{
						sysprintf("\nRead file %s terminated. (%d)\n", ptr, res);
						break;     /* error or eof */
					}

					res = f_read(&file2, Buff2, BUFF_SIZE, &s2);
					if (res || s2 == 0)
					{
						sysprintf("\nRead file %s terminated. (%d)\n", ptr2, res);
						break;     /* error or eof */
					}

					p1 += s2;
					if (res || s2 < s1) break;   /* error or disk full */

					sysprintf("C %d...", s1);

					if (memcmp(Buff1, Buff2, s1) != 0)
					{
						sysprintf("Compre failed!! %d %d\n", s1,s2);
						break;
					}
					sysprintf("[OK]\n");

					if ((p1 % 0x10000) == 0)
						sysprintf("\n%d KB compared.", p1/1024);
					sysprintf(".");
				}
				if (s1 == 0)
					sysprintf("\nPASS. \n ");
				f_close(&file1);
				f_close(&file2);
				break;

#if _FS_RPATH
			case 'g' :  /* fg <path> - Change current directory */
				while (*ptr == ' ') ptr++;
				put_rc(f_chdir(ptr));
				break;

			case 'j' :  /* fj <drive#> - Change current drive */
				while (*ptr == ' ') ptr++;
				put_dump((void *)&p1, ptr_to_u32(p1), 16);
				put_rc(f_chdrive((TCHAR *)ptr));
				break;
#endif
#if _USE_MKFS
			case 'm' :  /* fm <partition rule> <sect/clust> - Create file system */
				if (!xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
				sysprintf("The memory card will be formatted. Are you sure? (Y/n)=");
				get_line(ptr, sizeof(Line));
				if (*ptr == 'Y')
					put_rc(f_mkfs(0, (BYTE)p2, (WORD)p3));
				break;
#endif
			case 'z' :  /* fz [<rw size>] - Change R/W length for fr/fw/fx command */
				if (xatoi(&ptr, &p1) && p1 >= 1 && (size_t)p1 <= BUFF_SIZE)
					blen = p1;
				sysprintf("blen=%d\n", blen);
				break;
			}
			break;
		case '?':       /* Show usage */
			sysprintf(
				_T("n: - Change default drive (USB drive is 3~7)\n")
				_T("dd [<lba>] - Dump sector\n")
				//_T("ds <pd#> - Show disk status\n")
				_T("\n")
				_T("bd <ofs> - Dump working buffer\n")
				_T("be <ofs> [<data>] ... - Edit working buffer\n")
				_T("br <pd#> <sect> [<num>] - Read disk into working buffer\n")
				_T("bw <pd#> <sect> [<num>] - Write working buffer into disk\n")
				_T("bf <val> - Fill working buffer\n")
				_T("\n")
				_T("fs - Show volume status\n")
				_T("fl [<path>] - Show a directory\n")
				_T("fo <mode> <file> - Open a file\n")
				_T("fc - Close the file\n")
				_T("fe <ofs> - Move fp in normal seek\n")
				//_T("fE <ofs> - Move fp in fast seek or Create link table\n")
				_T("fd <len> - Read and dump the file\n")
				_T("fr <len> - Read the file\n")
				_T("fw <len> <val> - Write to the file\n")
				_T("fn <object name> <new name> - Rename an object\n")
				_T("fu <object name> - Unlink an object\n")
				_T("fv - Truncate the file at current fp\n")
				_T("fk <dir name> - Create a directory\n")
				_T("fa <atrr> <mask> <object name> - Change object attribute\n")
				_T("ft <year> <month> <day> <hour> <min> <sec> <object name> - Change timestamp of an object\n")
				_T("fx <src file> <dst file> - Copy a file\n")
				_T("fg <path> - Change current directory\n")
				_T("fj <ld#> - Change current drive. For example: <fj 4:>\n")
				_T("fm <ld#> <rule> <cluster size> - Create file system\n")
				_T("\n")
			);
			break;
		}
	}
	//while(1);
}


