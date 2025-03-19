#include "trans_recieve_buff_control.h"
#include "ringbuffer.h"

RingBuffer  m_Modbus_Master_RX_RingBuff;
uint8_t     m_Modbus_Master_RX_Buff[200];

uint8_t Modbus_Master_RB_Initialize(void)
{
    rbInitialize(&m_Modbus_Master_RX_RingBuff, m_Modbus_Master_RX_Buff, sizeof(m_Modbus_Master_RX_Buff));
    return 1 ;
}

void Modbus_Master_Rece_Flush(void)
{
    rbClear(&m_Modbus_Master_RX_RingBuff);
}

uint8_t Modbus_Master_Rece_Available(void)
{
    if (m_Modbus_Master_RX_RingBuff.flagOverflow == 1)
    {
        rbClear(&m_Modbus_Master_RX_RingBuff);
    }

    return !rbIsEmpty(&m_Modbus_Master_RX_RingBuff);
}

uint8_t Modbus_Master_GetByte(uint8_t  *getbyte)
{

    if (UART_Read(ModBus_UART, (uint8_t *)getbyte, 1) == 1)
        return HAL_OK;
    else
        return HAL_ERROR;
}

void Modbus_Master_Rece_Handler(void)
{
    uint8_t byte;

    if (Modbus_Master_GetByte(&byte) == HAL_OK)
    {
        rbPush(&m_Modbus_Master_RX_RingBuff, (uint8_t)(byte & (uint8_t)0xFFU));
    }
}

uint8_t Modbus_Master_Read(void)
{
    uint8_t cur = 0xff;

    if (!rbIsEmpty(&m_Modbus_Master_RX_RingBuff))
    {
        cur = rbPop(&m_Modbus_Master_RX_RingBuff);
    }

    return cur;
}

uint8_t Modbus_Master_Write(uint8_t *buf, uint8_t length)
{
    UART_Write(ModBus_UART, (uint8_t *)buf, length);
    return HAL_OK;
}

uint32_t Modbus_Master_Millis(void)
{
    return HAL_GetTick();
}
