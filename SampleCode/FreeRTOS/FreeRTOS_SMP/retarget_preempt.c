/**************************************************************************//**
 * @file     retarget_preempt.c
 * @brief    FreeRTOS-SMP thread-safe debug port (UART) I/O.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

/* FreeRTOS headers. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*-----------------------------------------------------------
 * Configuration.
 *----------------------------------------------------------*/

/* Both cores share a single UART (UART0) for debug output. */
#define DEUBG_PORT_ONE_ONLY

/* Compile-time default for the core-ID prefix. */
#ifndef SAFE_PRINTF_SHOW_CORE_ID
#define SAFE_PRINTF_SHOW_CORE_ID    0
#endif

/* Debug port for Core0 and Core1. */
UART_T *DEBUG_PORT[2] = {UART0, UART16};

void _PutChar_f(unsigned char ch)
{
#ifdef DEUBG_PORT_ONE_ONLY
    int id = 0;
#else
    int id = cpuid();
#endif
    while (DEBUG_PORT[id]->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
    DEBUG_PORT[id]->DAT = ch;
    if (ch == '\n') {
        while (DEBUG_PORT[id]->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
        DEBUG_PORT[id]->DAT = '\r';
    }
}

void sysPutString(const char *string)
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

static const char *FormatItem(const char *f, uint64_t a)
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

int sysIsKbHit(void)
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

char sysgetchar(void)
{
#ifdef DEUBG_PORT_ONE_ONLY
    int id = 0;
#else
    int id = cpuid();
#endif

    while (1)
    {
        if (!(DEBUG_PORT[id]->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk))
            return DEBUG_PORT[id]->DAT;
    }
}

void sysputchar(const char ch)
{
#ifdef DEUBG_PORT_ONE_ONLY
    int id = 0;
#else
    int id = cpuid();
#endif

    while (DEBUG_PORT[id]->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
    DEBUG_PORT[id]->DAT = ch;
}

/*===========================================================================
 * Thread-safe sysprintf
 *
 * Lock strategy:
 *   - Task context (scheduler running, not in ISR):
 *       FreeRTOS recursive mutex with priority inheritance.
 *       This is preemption-safe: if a high-priority task preempts the
 *       holder, it blocks on the mutex instead of deadlocking on a
 *       bare-metal spinlock.
 *
 *   - ISR context, or scheduler not yet running:
 *       ARMv8 spinlock (LDAXR/STXR).  FreeRTOS mutexes cannot be
 *       used from ISR context.  Before the scheduler starts, no
 *       preemption can occur, so the spinlock is safe.
 *
 * The mutex must be created before the first task-context call.
 * Call vSafePrintfInit() from your demo entry point before
 * vTaskStartScheduler().
 *===========================================================================*/

/* FreeRTOS recursive mutex for task-context serialisation. */
static SemaphoreHandle_t xPrintMutex = NULL;

/* ARMv8 spinlock for ISR / pre-scheduler fallback. */
static unsigned int ulISRPrintLock = 0;

/* Run-time toggle for core-ID prefix. */
static volatile int xShowCoreId = SAFE_PRINTF_SHOW_CORE_ID;

/**
 * @brief  Initialise the thread-safe printf subsystem.
 *
 *         Must be called once before vTaskStartScheduler().
 */
void vSafePrintfInit(void)
{
    if (xPrintMutex == NULL)
    {
        xPrintMutex = xSemaphoreCreateRecursiveMutex();
        configASSERT(xPrintMutex);
    }
}

/**
 * @brief  Enable or disable the "[coreN] " prefix at run-time.
 *
 * @param[in]  xEnable   Non-zero to enable, 0 to disable.
 */
void vSafePrintfShowCoreId(int xEnable)
{
    xShowCoreId = xEnable;
}

/**
 * @brief  Thread-safe system UART printf.
 *
 *         Signature and behaviour match the original sysprintf()
 *         from Library/StdDriver/src/retarget.c exactly, except
 *         the locking is FreeRTOS-aware.
 *
 * @param[in]  pcStr    printf-style format string.
 */
void sysprintf(const char *pcStr, ...)
{
    va_list va;
    BaseType_t xUseRTOSMutex = pdFALSE;

    /* Decide which lock to acquire. */
    if ((xPrintMutex != NULL) &&
        (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) &&
        (portCHECK_IF_IN_ISR() == 0))
    {
        xUseRTOSMutex = pdTRUE;
    }

    /* --- Acquire --- */
    if (xUseRTOSMutex)
    {
        xSemaphoreTakeRecursive(xPrintMutex, portMAX_DELAY);
    }
    else
    {
        cpu_spin_lock(&ulISRPrintLock);
    }

    /* --- Optional core-ID prefix --- */
    if (xShowCoreId)
    {
        _PutChar_f('[');
        _PutChar_f('c');
        _PutChar_f('o');
        _PutChar_f('r');
        _PutChar_f('e');
        _PutChar_f((unsigned char)('0' + cpuid()));
        _PutChar_f(']');
        _PutChar_f(' ');
    }

    /* --- Format and emit --- */
    va_start(va, pcStr);
    while (*pcStr) {
        if (*pcStr == '%')
            pcStr = FormatItem(pcStr + 1, va_arg(va, int));
        else
            _PutChar_f(*pcStr++);
    }
    va_end(va);

    /* --- Release --- */
    if (xUseRTOSMutex)
    {
        xSemaphoreGiveRecursive(xPrintMutex);
    }
    else
    {
        cpu_spin_unlock(&ulISRPrintLock);
    }
}
