/**************************************************************************//**
 * @file     main_blinky.c
 *
 * @brief    A simple blinky sample.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/**
* main_blinky() creates one queue, and two tasks.  It then starts the
* scheduler.
*
* The Queue Send Task:
* The queue send task is implemented by the prvQueueSendTask() function in
* this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
* block for 200 milliseconds, before sending the value 100 to the queue that
* was created within main_blinky().  Once the value is sent, the task loops
* back around to block for another 200 milliseconds...and so on.
*
* The Queue Receive Task:
* The queue receive task is implemented by the prvQueueReceiveTask() function
* in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
* blocks on attempts to read data from the queue that was created within
* main_blinky().  When data is received, the task checks the value of the
* data, and if the value equals the expected 100, outputs a message to the
* UART.  The 'block time' parameter passed to the queue receive function
* specifies that the task should be held in the Blocked state indefinitely to
* wait for data to be available on the queue.  The queue receive task will only
* leave the Blocked state when the queue send task writes to the queue.  As the
* queue send task writes to the queue every 200 milliseconds, the queue receive
* task leaves the Blocked state every 200 milliseconds, and therefore outputs
* a message every 200 milliseconds.
*/

#include "NuMicro.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 200 )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

#define ENQUEUE_VAR							( 100UL )
#define LED_0								( PJ14 )

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

void main_blinky( void )
{
    sysprintf("+-----------------------------------------------+\n");
    sysprintf("|          MA35D1 FreeRTOS blinky demo          |\n");
    sysprintf("+-----------------------------------------------+\n\n");

    /* Create the queue. */
    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

    if( xQueue != NULL )
    {
        /* Start the two tasks as described in the comments at the top of this
        file. */
        xTaskCreate( prvQueueReceiveTask,				/* The function that implements the task. */
                     "Rx", 								/* The text name assigned to the task - for debug only as it is not used by the kernel. */
                     configMINIMAL_STACK_SIZE, 			/* The size of the stack to allocate to the task. */
                     NULL, 								/* The parameter passed to the task - not used in this case. */
                     mainQUEUE_RECEIVE_TASK_PRIORITY, 	/* The priority assigned to the task. */
                     NULL );								/* The task handle is not required, so NULL is passed. */

        xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );

        /* Start the tasks and timer running. */
        vTaskStartScheduler();
    }
}
/*-----------------------------------------------------------*/

static void prvQueueSendTask( void *pvParameters )
{
    TickType_t xNextWakeTime;
    const uint32_t ulValueToSend = ENQUEUE_VAR;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again. */
        vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

        /* Send to the queue - causing the queue receive task to unblock and
        toggle the LED.  0 is used as the block time so the sending operation
        will not block - it shouldn't need to block as the queue should always
        be empty at this point in the code. */
        xQueueSend( xQueue, &ulValueToSend, 0U );
    }
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
    uint32_t ulReceivedValue;
    const uint32_t ulExpectedValue = ENQUEUE_VAR;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    for( ;; )
    {
        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
        xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

        /* To get here something must have been received from the queue, but is
        it the expected value?  If it is, toggle the LED. */
        if( ulReceivedValue == ulExpectedValue )
        {
            sysprintf( "%d received\r\n", ENQUEUE_VAR );
            LED_0 ^= 1;
            ulReceivedValue = 0U;
        }
    }
}
