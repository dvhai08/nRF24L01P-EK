#include "stm32f10x_it.h"
#include "sys_tick.h"
#include "hw_config.h"

extern uint8_t rfDataIncomingFlag;
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  NVIC_SystemReset();
	while(1);
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	SysTick_Task();
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
