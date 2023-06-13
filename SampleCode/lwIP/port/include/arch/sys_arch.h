/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define SYS_MBOX_NULL                   ( ( xQueueHandle ) NULL )
#define SYS_SEM_NULL                    ( ( xSemaphoreHandle ) NULL )
#define SYS_DEFAULT_THREAD_STACK_DEPTH  ( configMINIMAL_STACK_SIZE )

/** This is returned by _fromisr() sys functions to tell the outermost function
 * that a higher priority task was woken and the scheduler needs to be invoked.
 */
#define ERR_NEED_SCHED 123

#if !LWIP_COMPAT_MUTEX
struct _sys_mut {
    void *mut;
};
typedef struct _sys_mut sys_mutex_t;
#define sys_mutex_valid_val(mutex)      ((mutex).mut != NULL)
#define sys_mutex_valid(mutex)          (((mutex) != NULL) && sys_mutex_valid_val(*(mutex)))
#define sys_mutex_set_invalid(mutex)    ((mutex)->mut = NULL)
#endif /* !LWIP_COMPAT_MUTEX */

struct _sys_sem {
    void *sem;
};
typedef struct _sys_sem sys_sem_t;
#define sys_sem_valid_val(sema)         ((sema).sem != NULL)
#define sys_sem_valid(sema)             (((sema) != NULL) && sys_sem_valid_val(*(sema)))
#define sys_sem_set_invalid(sema)       ((sema)->sem = NULL)

struct _sys_mbox {
    void *mbx;
};
typedef struct _sys_mbox sys_mbox_t;
#define sys_mbox_valid_val(mbox)        ((mbox).mbx != NULL)
#define sys_mbox_valid(mbox)            (((mbox) != NULL) && sys_mbox_valid_val(*(mbox)))
#define sys_mbox_set_invalid(mbox)      ((mbox)->mbx = NULL)

struct _sys_thread {
    void *thread_handle;
};
typedef struct _sys_thread sys_thread_t;



#endif /* __ARCH_SYS_ARCH_H__ */

