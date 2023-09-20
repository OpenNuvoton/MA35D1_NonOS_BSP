/****************************************************************************
 * @file     openamp.h
 *
 * @brief    openamp header file.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __openamp_H
#define __openamp_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "openamp/open_amp.h"
#include "openamp_conf.h"


#define OPENAMP_destroy_ept rpmsg_destroy_ept

/* Initialize the openamp framework*/
int MA35D1_OpenAMP_Init(int RPMsgRole, rpmsg_ns_bind_cb ns_bind_cb);

/* Deinitialize the openamp framework*/
void OPENAMP_DeInit(void);

/* Initialize the endpoint struct*/
void OPENAMP_init_ept(struct rpmsg_endpoint *ept);

/* Create and register the endpoint */
int OPENAMP_create_endpoint(struct rpmsg_endpoint *ept, const char *name,
                            uint32_t dest, rpmsg_ept_cb cb,
                            rpmsg_ns_unbind_cb unbind_cb);

/* Check for new rpmsg reception */
void OPENAMP_check_for_message(struct rpmsg_endpoint *ept);

/* Wait loop on endpoint ready ( message dest address is know)*/
void OPENAMP_Wait_EndPointready(struct rpmsg_endpoint *rp_ept);

int OPENAMP_send_data(struct rpmsg_endpoint *ept, const void *data, int len);

int OPENAMP_check_TxAck(struct rpmsg_endpoint *ept);

#ifdef __cplusplus
}
#endif
#endif /*__openamp_H */


