#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>


#define LED_1_PIN											GPIO_Pin_2
#define LED_1_PORT            				GPIOA

#define LED_2_PIN											GPIO_Pin_4
#define LED_2_PORT            				GPIOA

#define BUTTON_1_PIN									GPIO_Pin_11
#define BUTTON_1_PORT            			GPIOA

// NRF
#define NRF_CSN_PIN										GPIO_Pin_14
#define NRF_CSN_PORT            			GPIOB

#define NRF_SCK_PIN										GPIO_Pin_15
#define NRF_SCK_PORT            			GPIOA

#define NRF_MOSI_PIN									GPIO_Pin_6
#define NRF_MOSI_PORT            			GPIOB

#define NRF_MISO_PIN									GPIO_Pin_7
#define NRF_MISO_PORT            			GPIOB

#define NRF_CE_PIN										GPIO_Pin_15
#define NRF_CE_PORT            				GPIOB

#define NRF_IRQ_PIN										GPIO_Pin_9
#define NRF_IRQ_PORT									GPIOB


void hwPinModeOutput(GPIO_TypeDef* port,uint16_t pin);
void hwPinModeInput(GPIO_TypeDef* port,uint16_t pin);
void LedOn(void);
void LedOff(void);
#endif //__HW_CONFIG_H


