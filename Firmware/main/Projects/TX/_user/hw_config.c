#include "stm32f10x.h"
#include "hw_config.h"


void hwPinModeOutput(GPIO_TypeDef* port,uint16_t pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(port, &GPIO_InitStructure );	
}

void hwPinModeInput(GPIO_TypeDef* port,uint16_t pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(port, &GPIO_InitStructure );	
}

void LedOn(void)
{
	GPIO_SetBits(LED_1_PORT, LED_1_PIN);
	GPIO_SetBits(LED_2_PORT, LED_2_PIN);
}

void LedOff(void)
{
	GPIO_ResetBits(LED_1_PORT, LED_1_PIN);
	GPIO_ResetBits(LED_2_PORT, LED_2_PIN);
}