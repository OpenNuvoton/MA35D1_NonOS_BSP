/**************************************************************************//**
 * @file     retarget.c
 * @brief    Debug1 Port Setting Source File
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

//#define DEUBG_SHOW_CORE_INFO
//#define DEUBG_PORT_ONE_ONLY

/* Debug port for Core0 and Core1 */
UART_T *DEBUG_PORT[2] = {UART16, UART16};

void _PutChar_f(unsigned char ch)
{
#ifdef DEUBG_PORT_ONE_ONLY
	int id = 0;
#else
	int id = cpuid();
#endif
    while (DEBUG_PORT[id]->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
    DEBUG_PORT[id]->DAT = ch;
    if(ch == '\n') {
    	while (DEBUG_PORT[id]->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
    	DEBUG_PORT[id]->DAT = '\r';
    }
}

void sysPutString(unsigned char *string)
{
    while (*string != '\0') {
        _PutChar_f(*string);
        string++;
    }
}

static void sysPutRepChar(char c, int count)
{
    while (count--)
        _PutChar_f(c);
}

static void sysPutStringReverse(char *s, int index)
{
    while ((index--) > 0)
        _PutChar_f(s[index]);
}

static void sysPutNumber(int value, int radix, int width, char fill)
{
	char    buffer[40];
    int     bi = 0;
    unsigned int  uvalue;
    unsigned short  digit;
    unsigned short  left = FALSE;
    unsigned short  negative = FALSE;

    if (fill == 0)
        fill = ' ';

    if (width < 0)
    {
        width = -width;
        left = TRUE;
    }

    if (width < 0 || width > 80)
        width = 0;

    if (radix < 0)
    {
        radix = -radix;
        if (value < 0)
        {
            negative = TRUE;
            value = -value;
        }
    }

    uvalue = value;

    do
    {
        if (radix != 16)
        {
            digit = uvalue % radix;
            uvalue = uvalue / radix;
        }
        else
        {
            digit = uvalue & 0xf;
            uvalue = uvalue >> 4;
        }
        buffer[bi] = digit + ((digit <= 9) ? '0' : ('A' - 10));
        bi++;

        if (uvalue != 0)
        {
            if ((radix == 10)
                && ((bi == 3) || (bi == 7) || (bi == 11) || (bi == 15)))
            {
                buffer[bi++] = ',';
            }
        }
    }
    while (uvalue != 0);

    if (negative)
    {
        buffer[bi] = '-';
        bi += 1;
    }

    if (width <= bi)
        sysPutStringReverse(buffer, bi);
    else
    {
        width -= bi;
        if (!left)
            sysPutRepChar(fill, width);
        sysPutStringReverse(buffer, bi);
        if (left)
            sysPutRepChar(fill, width);
    }
}

static char *FormatItem(char *f, uint64_t a)
{
	char   c;
    int    fieldwidth = 0;
    int    leftjust = FALSE;
    int    radix = 0;
    char   fill = ' ';

	a = ptr_to_u32(a);

    if (*f == '0')
        fill = '0';

    while ((c = *f++) != 0)
    {
        if (c >= '0' && c <= '9')
        {
            fieldwidth = (fieldwidth * 10) + (c - '0');
        }
        else if (c == 'l')
            continue;
        else
            switch (c)
            {
                case '\000':
                    return (--f);
                case '%':
                    _PutChar_f('%');
                    return (f);
                case '-':
                    leftjust = TRUE;
                    break;
                case 'c':
                    {
                        if (leftjust)
                            _PutChar_f(a & 0x7f);

                        if (fieldwidth > 0)
                            sysPutRepChar(fill, fieldwidth - 1);

                        if (!leftjust)
                            _PutChar_f(a & 0x7f);
                        return (f);
                    }
                case 's':
                    {
                        if (leftjust)
                            sysPutString((char *)a);

                        if (fieldwidth > strlen((char *)a))
                            sysPutRepChar(fill, fieldwidth - strlen((char *)a));

                        if (!leftjust)
                            sysPutString((char *)a);
                        return (f);
                    }
                case 'd':
                case 'i':
                    radix = -10;
                    break;
                case 'u':
                    radix = 10;
                    break;
                case 'x':
                    radix = 16;
                    break;
                case 'X':
                    radix = 16;
                    break;
                case 'o':
                    radix = 8;
                    break;
                default:
                    radix = 3;
                    break;      /* unknown switch! */
            }
        if (radix)
            break;
    }

    if (leftjust)
        fieldwidth = -fieldwidth;

    sysPutNumber(a, radix, fieldwidth, fill);

    return (f);
}

int  sysIsKbHit()
{
#ifdef DEUBG_PORT_ONE_ONLY
	int id = 0;
#else
	int id = cpuid();
#endif

    if (DEBUG_PORT[id]->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk)
        return 0;
    else 
        return 1;
}
/// @endcond HIDDEN_SYMBOLS

/**
 *  @brief  system UART printf
 *  @param[in]  pcStr    output string
 *  @return   None
 */
#ifdef DEUBG_PORT_ONE_ONLY
static unsigned int mutex_print=0;
#endif
void sysprintf(char * pcStr,...)
{
	char  *argP;
	va_list va;

#ifdef DEUBG_PORT_ONE_ONLY
	cpu_spin_lock(&mutex_print);
#ifdef DEUBG_SHOW_CORE_INFO
	_PutChar_f((char)(0x30+cpuid()));
	_PutChar_f(':');
#endif
#endif
    va_start(va, pcStr);	/* point at the end of the format string */
    while (*pcStr) {
    	/* this works because args are all ints */
        if (*pcStr == '%')
            pcStr = FormatItem(pcStr + 1, va_arg(va, int));
        else
            _PutChar_f(*pcStr++);
    }
    va_end(va);
#ifdef DEUBG_PORT_ONE_ONLY
    cpu_spin_unlock(&mutex_print);
#endif
}

/**
 *  @brief  system UART get char
 *  @return   input char
 */
char sysgetchar(void)
{
#ifdef DEUBG_PORT_ONE_ONLY
	int id = 0;
#else
	int id = cpuid();
#endif

    while (1)
    {
        if(!(DEBUG_PORT[id]->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk))
        	return DEBUG_PORT[id]->DAT;
    }
}

/**
 *  @brief  system UART put char
 *  @param[in]  ch output char
 *  @return   None
 */
void sysputchar(char ch)
{
#ifdef DEUBG_PORT_ONE_ONLY
	int id = 0;
#else
	int id = cpuid();
#endif

    while (DEBUG_PORT[id]->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
    DEBUG_PORT[id]->DAT = ch;
}
