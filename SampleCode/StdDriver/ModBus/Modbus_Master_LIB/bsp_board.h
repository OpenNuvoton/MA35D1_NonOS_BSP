/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_BOARD_H
#define __BSP_BOARD_H
/* Includes ------------------------------------------------------------------*/

#include "NuMicro.h"
typedef enum
{
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;
extern uint32_t HAL_GetTick(void);

#define ModBus_UART UART1

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */


#endif
/********END OF FILE****/
