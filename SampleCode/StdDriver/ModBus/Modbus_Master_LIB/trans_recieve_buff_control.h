/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TANS_RECEIEVE_BUFF_CONTROL_H
#define __TANS_RECEIEVE_BUFF_CONTROL_H
#include "NuMicro.h"
#include "bsp_board.h"

uint8_t Modbus_Master_RB_Initialize(void);
void Modbus_Master_Rece_Flush(void);
uint8_t Modbus_Master_Rece_Available(void);
uint8_t Modbus_Master_GetByte(uint8_t  *getbyte);
void Modbus_Master_Rece_Handler(void);
uint8_t Modbus_Master_Read(void);
uint8_t Modbus_Master_Write(uint8_t *buf, uint8_t length);
uint32_t Modbus_Master_Millis(void);
#endif
/********END OF FILE****/
