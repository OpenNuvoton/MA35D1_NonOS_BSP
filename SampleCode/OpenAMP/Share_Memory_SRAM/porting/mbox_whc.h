/****************************************************************************
 * @file     mbox_whc.h
 *
 * @brief    mbox_whc module header file.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef MAILBOX_WHC_H_
#define MAILBOX_WHC_H_

#define COMMAND_RECEIVE_M4_MSG         0x60
#define COMMAND_SEND_ACK_TO_M4         0x61
#define COMMAND_SEND_MSG_TO_M4         0x80
#define COMMAND_RECEIVE_M4_ACK         0x81

#define RX_NO_MSG           1
#define RX_NEW_MSG          2
#define RX_BUF_FREE         3
#define TX_NO_ACK           4
#define TX_ACK              5

int Mbox_Notify(void *priv, uint32_t id);
int Mbox_Init(void);
int Mbox_Poll(struct rpmsg_endpoint *ept);


#endif /* MAILBOX_WHC_H_ */
