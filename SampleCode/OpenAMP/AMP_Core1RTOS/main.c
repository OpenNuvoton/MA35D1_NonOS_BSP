/**************************************************************************//**
 * @file     main.c
 *
 * @brief    This project demonstrates AMP between dual cores of A35,
 *           with core0 running Linux and core1 running FreeRTOS.
 *           Fill in your endpoints in "eptinst[]" to complete the design.
 * 
 * Core0      Core1 (this core)
 *   A  <----->  B (Tx & Rx) (High freq. short packet)
 *   C  <----->  D (Tx & Rx) (Low freq. long packet)
 *   E  <----->  F (Tx & Rx) (CRC test)
 * 
 * @note     TIMER8/TIMER9 has been assigned to OpenAMP for IPI.
 *
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "platform_info.h"
#include "rsc_table.h"

#define TASKB_TX_SIZE   0x400
#define TASKD_TX_SIZE   0x2800
#define TASKF_TX_SIZE   0x400

int ReadTaskB_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv);
int ReadTaskD_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv);
int ReadTaskF_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv);
void vPollTaskB( void *pvParameters );
void vSendTaskB( void *pvParameters );
void vPollTaskD( void *pvParameters );
void vSendTaskD( void *pvParameters );
void vPollTaskF( void *pvParameters );
void vSendTaskF( void *pvParameters );

/* User defined endpoints */
struct amp_endpoint eptinst[] = {
	{ {"eptA->B", EPT_TYPE_RX}, ReadTaskB_cb, vPollTaskB },        /* Task B */
	{ {"eptB->A", EPT_TYPE_TX, TASKB_TX_SIZE}, NULL, vSendTaskB }, /* Task B */
	{ {"eptC->D", EPT_TYPE_RX}, ReadTaskD_cb, vPollTaskD },        /* Task D */
	{ {"eptD->C", EPT_TYPE_TX, TASKD_TX_SIZE}, NULL, vSendTaskD }, /* Task D */
    { {"eptE->F", EPT_TYPE_RX}, ReadTaskF_cb, vPollTaskF },        /* Task F */
	{ {"eptF->E", EPT_TYPE_TX, TASKF_TX_SIZE}, NULL, vSendTaskF }, /* Task F */
};
char tx_bufB[TASKB_TX_SIZE];
char tx_bufD[TASKD_TX_SIZE];

TaskHandle_t createTaskHandle;
unsigned long throughput[4];

#define rpmsgEpt_to_ampEpt(ept) metal_container_of(ept, struct amp_endpoint, ept)

void UART16_Init()
{
    /* Enable UART16 clock */
    CLK_EnableModuleClock(UART16_MODULE);
    CLK_SetModuleClock(UART16_MODULE, CLK_CLKSEL3_UART16SEL_HXT, CLK_CLKDIV3_UART16(1));

    /* Set multi-function pins */
    SYS->GPK_MFPL &= ~(SYS_GPK_MFPL_PK2MFP_Msk | SYS_GPK_MFPL_PK3MFP_Msk);
    SYS->GPK_MFPL |= (SYS_GPK_MFPL_PK2MFP_UART16_RXD | SYS_GPK_MFPL_PK3MFP_UART16_TXD);

	/* Init UART to 115200-8n1 for print message */
	UART_Open(UART16, 115200);
}

void SYS_Init()
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Init UART for sysprintf */
    UART16_Init();

    // Enable HWSEM clock
    CLK_EnableModuleClock(HWS_MODULE);

    // Reset HWSEM
    SYS->IPRST0 = SYS_IPRST0_HWSEMRST_Msk;
    SYS->IPRST0 = 0;

    /* Enable global timer */
    global_timer_init();

    /* Lock protected registers */
    SYS_LockReg();
}

/**
 * @brief Create endpoint and its task
 * 
 * @param rpdev pointer to rpmsg_device
 * @param amp_ept pointer to an inst of amp_endpoint
 * @return int 
 */
int amp_create_ept(struct rpmsg_device *rpdev, struct amp_endpoint *amp_ept)
{
    struct rpmsg_endpoint_info *info = &amp_ept->eptinfo;
    int ret;
    static int id = 0; // serial number

    if(info->type == EPT_TYPE_TX) {
        ret = ma35_rpmsg_create_txept(&amp_ept->ept, rpdev, info->name, info->size);
    } else if (info->type == EPT_TYPE_RX) {
        ret = ma35_rpmsg_create_rxept(&amp_ept->ept, rpdev, info->name, amp_ept->cb);
    } else {
        sysprintf("Invalid endpoint type.\n");
        ret = -1;
    }

    if(ret >= 0) {
        xTaskCreate( amp_ept->task_fn, amp_ept->eptinfo.name, configMINIMAL_STACK_SIZE, &amp_ept->ept, tskIDLE_PRIORITY + 2, &amp_ept->taskHandle );
        sysprintf("rpmsg%d: %s endpoint \"%s\" is created.\n", id++,
            (info->type == EPT_TYPE_TX) ? "Tx" : "Rx",
            info->name);
    } else {
        sysprintf("Failed to create endpoint \"%s\", err: 0x%x.\n", info->name, ret);
    }

	return ret;
}

/**
 * @brief Start AMP
 * 
 * @param rpdev pointer to rpmsg_device
 * @return int 
 */
int amp_open(struct rpmsg_device *rpdev)
{
    int i, ret, inst;
    inst = sizeof(eptinst) / sizeof(eptinst[0]);

    for(i = 0; i < inst; i++) {
		ret = amp_create_ept(rpdev, &eptinst[i]);
		if (ret < 0) {
			return -EINVAL;
		}
	}
    
    return 0;
}

/**
 * @brief Recycle endpoint
 * 
 * @param amp_ept pointer to an inst of amp_endpoint
 * @return int 
 */
int amp_destroy_ept(struct amp_endpoint *amp_ept)
{
    int ret;

    if(amp_ept->taskHandle)
    {
        vTaskDelete(amp_ept->taskHandle);
        amp_ept->taskHandle = NULL;
    }

    ret = ma35_rpmsg_destroy_ept(&amp_ept->ept);
    if(ret)
        sysprintf("Failed to destroy endpoint \"%s\".\n", amp_ept->eptinfo.name);
    else
        sysprintf("Endpoint \"%s\" is destroyed.\n", amp_ept->eptinfo.name);

    return ret;
}

/**
 * @brief End AMP
 * 
 * @return int 
 */
int amp_close(void)
{
    int i, ret, inst;
    inst = sizeof(eptinst) / sizeof(eptinst[0]);

    for(i = 0; i < inst; i++) {
		ret = amp_destroy_ept(&eptinst[i]);
	}
    
    return 0;
}

/**
 * @brief User Rx callback, do not call this directly.
 *        The function is called when a POLLIN event is received.
 * 
 * @param ept rpmsg endpoint
 * @param data pointer to rx buffer provided by driver
 * @param len length of rx buffer
 * @param src unused
 * @param priv unused
 * @return int always return RPMSG_SUCCESS
 */
int ReadTaskB_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
	char *rxbuf = data;
	int rxlen = len;
	(void)src;
	(void)priv;

    /* Start of user read function */
    throughput[0] += rxlen;
    /* End of user read function */

	return RPMSG_SUCCESS;
}

/**
 * @brief User poll task, call ma35_rpmsg_poll to get status
 *
 * @param pvParameters ept
 */
void vPollTaskB( void *pvParameters )
{
    int ret;

    for( ;; )
    {
        ret = ma35_rpmsg_poll(pvParameters);
        if(ret == RPMSG_ERR_PERM)
        {
            sysprintf("%s: Remote closed.\n", pcTaskGetName(NULL));
            /* 1. do nothing and try reconnecting 2. call ma35_rpmsg_destroy_ept and exit */
            // ma35_rpmsg_destroy_ept(pvParameters);
            // vTaskDelete(NULL);
        }
        else if(ret == RPMSG_ERR_NO_BUFF)
            sysprintf("%s: Rx buffer is full.\n", pcTaskGetName(NULL));

        vTaskDelay(2);
    }
}

/**
 * @brief User Tx task, call ma35_rpmsg_send to send data
 *
 * @param pvParameters ept
 */
void vSendTaskB( void *pvParameters )
{
	int i, ret, size, j = 0;

    for( ;; )
    {
        /* Start of user write function */
        size = rand();
        size = size % TASKB_TX_SIZE;
        size = size ? size : TASKD_TX_SIZE;

        for(i = 0; i < size; i++)
            tx_bufB[i] = size + i;
        /* End of user write function */

        ret = ma35_rpmsg_send(pvParameters, tx_bufB, size);
        if(ret < 0) {
			if(ret == RPMSG_ERR_PERM) 
			{
                sysprintf("%s: Remote closed.\n", pcTaskGetName(NULL));
                /* 1. do nothing and try reconnecting 2. call ma35_rpmsg_destroy_ept and exit */
                // ma35_rpmsg_destroy_ept(pvParameters);
                // vTaskDelete(NULL);
            }
			else if(ret == RPMSG_ERR_NO_BUFF)
                sysprintf("%s: Tx blocking.\n", pcTaskGetName(NULL));
			// else if(ret == RPMSG_ERR_INIT)
			// 	sysprintf("%s: Remote ept not ready.\n", pcTaskGetName(NULL));
		}
        else {
            throughput[1] += ret;
        }

        vTaskDelay(10);
    }
}

/**
 * @brief User Rx callback, do not call this directly.
 *        The function is called when a POLLIN event is received.
 * 
 * @param ept rpmsg endpoint
 * @param data pointer to rx buffer provided by driver
 * @param len length of rx buffer
 * @param src unused
 * @param priv unused
 * @return int always return RPMSG_SUCCESS
 */
int ReadTaskD_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
	char *rxbuf = data;
	int rxlen = len;
	(void)src;
	(void)priv;

    /* Start of user read function */
    throughput[2] += rxlen;
    /* End of user read function */

	return RPMSG_SUCCESS;
}

/**
 * @brief User poll task, call ma35_rpmsg_poll to get status
 *
 * @param pvParameters ept
 */
void vPollTaskD( void *pvParameters )
{
    int ret;

    for( ;; )
    {
        ret = ma35_rpmsg_poll(pvParameters);
        if(ret == RPMSG_ERR_PERM)
        {
            sysprintf("%s: Remote closed.\n", pcTaskGetName(NULL));
            /* 1. do nothing and try reconnecting 2. call ma35_rpmsg_destroy_ept and exit */
            // ma35_rpmsg_destroy_ept(pvParameters);
            // vTaskDelete(NULL);
        }
        else if(ret == RPMSG_ERR_NO_BUFF)
            sysprintf("%s: Rx buffer is full.\n", pcTaskGetName(NULL));

        vTaskDelay(10);
    }
}

/**
 * @brief User Tx task, call ma35_rpmsg_send to send data
 *
 * @param pvParameters ept
 */
void vSendTaskD( void *pvParameters )
{
	int i, ret, size, j = 0;

    for( ;; )
    {
        /* Start of user write function */
        size = rand();
        size = size % TASKD_TX_SIZE;
        size = size ? size : TASKD_TX_SIZE;

        for(i = 0; i < size; i++)
            tx_bufD[i] = size + i;
        /* End of user write function */

        ret = ma35_rpmsg_send(pvParameters, tx_bufD, size);
        if(ret < 0) {
			if(ret == RPMSG_ERR_PERM) 
			{
                sysprintf("%s: Remote closed.\n", pcTaskGetName(NULL));
                /* 1. do nothing and try reconnecting 2. call ma35_rpmsg_destroy_ept and exit */
                // ma35_rpmsg_destroy_ept(pvParameters);
                // vTaskDelete(NULL);
            }
			else if(ret == RPMSG_ERR_NO_BUFF)
                sysprintf("%s: Tx blocking.\n", pcTaskGetName(NULL));
			// else if(ret == RPMSG_ERR_INIT)
			// 	sysprintf("%s: Remote ept not ready.\n", pcTaskGetName(NULL));
		}
        else {
            throughput[3] += ret;
        }

        vTaskDelay(100);
    }
}

extern uint32_t crc32(uint32_t crc, void *data, size_t length);
/**
 * @brief User Rx callback, do not call this directly.
 *        The function is called when a POLLIN event is received.
 * 
 * @param ept rpmsg endpoint
 * @param data pointer to rx buffer provided by driver
 * @param len length of rx buffer
 * @param src unused
 * @param priv unused
 * @return int always return RPMSG_SUCCESS
 */
int ReadTaskF_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
    struct amp_endpoint *eptRead = rpmsgEpt_to_ampEpt(ept);
    struct amp_endpoint *eptSend = eptRead + 1;
	char *rxbuf = data;
	int rxlen = len;
	(void)src;
	(void)priv;

    /* Start of user read function */
    uint32_t crc_cal = crc32(0UL, rxbuf, rxlen);
    // signal to vSendTaskF
    xTaskNotify(eptSend->taskHandle, crc_cal, eSetValueWithOverwrite);
    /* End of user read function */

	return RPMSG_SUCCESS;
}

/**
 * @brief User poll task, call ma35_rpmsg_poll to get status
 *
 * @param pvParameters ept
 */
void vPollTaskF( void *pvParameters )
{
    int ret;

    for( ;; )
    {
        ret = ma35_rpmsg_poll(pvParameters);
        if(ret == RPMSG_ERR_PERM)
        {
            sysprintf("%s: Remote closed.\n", pcTaskGetName(NULL));
            /* 1. do nothing and try reconnecting 2. call ma35_rpmsg_destroy_ept and exit */
            // ma35_rpmsg_destroy_ept(pvParameters);
            // vTaskDelete(NULL);
        }
        else if(ret == RPMSG_ERR_NO_BUFF)
            sysprintf("%s: Rx buffer is full.\n", pcTaskGetName(NULL));

        vTaskDelay(100);
    }
}

/**
 * @brief User Tx task, call ma35_rpmsg_send to send data
 *
 * @param pvParameters ept
 */
void vSendTaskF( void *pvParameters )
{
	int i, ret, size = sizeof(uint32_t);
    uint32_t txbuf;

    for( ;; )
    {
        /* Start of user write function */
        // This task is woken up by ReadTaskF_cb
        txbuf = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        /* End of user write function */

        ret = ma35_rpmsg_send(pvParameters, &txbuf, size);
        if(ret < 0) {
            if(ret == RPMSG_ERR_NO_BUFF)
                sysprintf("%s: Tx blocking.\n", pcTaskGetName(NULL));
        }
    }
}

void vEndpointCreateTask( void *pvParameters )
{
    struct rpmsg_device *rpdev = pvParameters;
    int delay = 1000;

    for( ;; )
    {
        if(ma35_rpmsg_remote_ready())
        {
            amp_open(rpdev);
            delay = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        vTaskDelay(delay);
    }
}

void vCmdTask( void *pvParameters )
{
    struct amp_endpoint *amp_ept = pvParameters;
    char c;
    unsigned long stattick = 0;
    int inst, avail, total;

    while(!ma35_rpmsg_remote_ready());

    for( ;; )
    {
        ++stattick;
        if (UART_IS_RX_READY(UART16))
        {
            c = sysgetchar();
            sysprintf("%c\n", c);
            switch(c)
            {
                case '0':
                    amp_destroy_ept(&amp_ept[c-'0']);
                    break;
                case '1':
                    amp_destroy_ept(&amp_ept[c-'0']);
                    break;
                case '2':
                    amp_destroy_ept(&amp_ept[c-'0']);
                    break;
                case '3':
                    amp_destroy_ept(&amp_ept[c-'0']);
                    break;
                case 'g':
                    ma35_rpmsg_get_buffer_size(&avail, &total);
                    sysprintf("Avail/Total shared memory: %d/%d bytes\n", avail, total);
                    break;
                case 'h':
                    sysprintf("Heap available: %d bytes\n", xPortGetFreeHeapSize());
                    break;
                case 'r':
                    sysprintf("Restart tasks.\n");
                    amp_close();
                    stattick = 0;
                    throughput[0] = throughput[1] = throughput[2] = throughput[3] = 0;
                    xTaskNotify(createTaskHandle, 10, eSetValueWithOverwrite);
                    break;
                case 's':
                    if(stattick)
                        sysprintf("Statistics: %lu, %lu, %lu, %lu\n",
                            throughput[0]/stattick, throughput[1]/stattick, throughput[2]/stattick, throughput[3]/stattick);
                    break;
                case 'z':
                    stattick = 0;
                    throughput[0] = throughput[1] = throughput[2] = throughput[3] = 0;
                    sysprintf("Reset statistics.\n");
                    break;
                default:
                    break;
            }
        }

        vTaskDelay(1000);
    }
}

int main(void)
{
    void *platform;
	struct rpmsg_device *rpdev;
	int ret;

    SYS_Init();

    sysprintf("\n\nMA35D1 AMP core#1 demo\n");
    sysprintf("Please refer to source code for detailed command info.\n\n");
    fflush(stdout);

	/* Initialize platform */
	ret = platform_init(0, NULL, &platform);
	if (ret) {
		sysprintf("Failed to initialize platform.\r\n");
		ret = -1;
	} else {
		rpdev = platform_create_rpmsg_vdev(platform, 0,
						  VIRTIO_DEV_DRIVER, // only driver, we support both tx/rx
						  NULL,
						  NULL);
        if (!rpdev) {
			sysprintf("Failed to create rpmsg virtio device.\r\n");
			ret = -1;
		} else {
			ret = 0;
		}
	}

    if(!ret)
    {
        xTaskCreate( vEndpointCreateTask, "CreateEpt", configMINIMAL_STACK_SIZE, rpdev, tskIDLE_PRIORITY + 1, &createTaskHandle );
        xTaskCreate( vCmdTask, "CmdTask", configMINIMAL_STACK_SIZE, eptinst, tskIDLE_PRIORITY + 1, NULL );
        vTaskStartScheduler();
        /* Should never be reached */
    }

    platform_release_rpmsg_vdev(rpdev, platform);
    platform_cleanup(platform);

    return 0;
}
