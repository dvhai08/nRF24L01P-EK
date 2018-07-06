#include "stm32f10x.h"
#include "rtt_log.h"
#include "hw_config.h"
#include "sys_tick.h"
#include "nRF_API.h"
#include "Protocol_API.h"
#include "defines.h"
#include "nrf24l01.h"

/* My address */
uint8_t MyAddress[] = {
    0xE7,
    0xE7,
    0xE7,
    0xE7,
    0xE7
};
/* Receiver address */
uint8_t TxAddress[] = {
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E
};
 
uint8_t dataOut[32], dataIn[32];

Timeout_Type tTimeout;

void SystemClockInit(void);

int tx_main(void);
int rx_main(void);
//
int main(void)
{
	SystemClockInit();
	__enable_irq();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
												| RCC_APB2Periph_GPIOB
												| RCC_APB2Periph_GPIOC
												| RCC_APB2Periph_GPIOD
												| RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	if (SysTick_Config(SystemCoreClock / 1000))
	{
			__disable_irq();
			NVIC_SystemReset();
	}
	
	hwPinModeOutput(LED_1_PORT, LED_1_PIN);
	hwPinModeOutput(LED_2_PORT, LED_2_PIN);
	hwPinModeOutput(NRF_CE_PORT, NRF_CE_PIN);
	hwPinModeOutput(NRF_MOSI_PORT, NRF_MOSI_PIN);
	hwPinModeOutput(NRF_SCK_PORT, NRF_SCK_PIN);
	hwPinModeOutput(NRF_CSN_PORT, NRF_CSN_PIN);
	
	hwPinModeInput(NRF_MISO_PORT, NRF_MISO_PIN);
	hwPinModeInput(NRF_IRQ_PORT, NRF_IRQ_PIN);

	NRF_LOG_PRINTF("\r\nSYSTEM START\r\n");
	
//	tx_main();
	
	rx_main();
	
	while(1)
	{			
	}
}

int tx_main(void) {
    TM_NRF24L01_Transmit_Status_t transmissionStatus;

    
    /* Initialize NRF24L01+ on channel 15 and 32bytes of payload */
    /* By default 2Mbps data rate and 0dBm output power */
    /* NRF24L01 goes to RX mode by default */
    TM_NRF24L01_Init(15, 32);
    
    /* Set 2MBps data rate and -18dBm output power */
    TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_2M, TM_NRF24L01_OutputPower_0dBm);
    
    /* Set my address, 5 bytes */
    TM_NRF24L01_SetMyAddress(MyAddress);
    /* Set TX address, 5 bytes */
    TM_NRF24L01_SetTxAddress(TxAddress);

    while (1) {
        /* Every 2 seconds */
        
            /* Fill data with something */
            sprintf((char *)dataOut, "abcdefghijklmnoszxABCDEFCBDA");
            /* Display on USART */
            NRF_LOG_PRINTF("pinging: ");
            /* Reset time, start counting microseconds */
            
            /* Transmit data, goes automatically to TX mode */
            TM_NRF24L01_Transmit(dataOut);
            
            /* Turn on led to indicate sending */
            GPIO_SetBits(LED_1_PORT, LED_1_PIN);
						GPIO_SetBits(LED_2_PORT, LED_2_PIN);
            /* Wait for data to be sent */
            do {
                transmissionStatus = TM_NRF24L01_GetTransmissionStatus();
            } while (transmissionStatus == TM_NRF24L01_Transmit_Status_Sending);
            /* Turn off led */
            GPIO_ResetBits(LED_1_PORT, LED_1_PIN);
						GPIO_ResetBits(LED_2_PORT, LED_2_PIN);
            
            /* Go back to RX mode */
            TM_NRF24L01_PowerUpRx();
            
						InitTimeout(&tTimeout,100);
						
            /* Wait received data, wait max 100ms, if time is larger, then data were probably lost */
            while (!TM_NRF24L01_DataReady() && (CheckTimeout(&tTimeout) != SYSTICK_TIMEOUT));
         
            
            /* Get data from NRF2L01+ */
            TM_NRF24L01_GetData(dataIn);
            
            /* Check transmit status */
            if (transmissionStatus == TM_NRF24L01_Transmit_Status_Ok) {
                /* Transmit went OK */
                NRF_LOG_PRINTF(": OK\n");
            } else if (transmissionStatus == TM_NRF24L01_Transmit_Status_Lost) {
                /* Message was LOST */
                NRF_LOG_PRINTF(": LOST\n");
            } else {
                /* This should never happen */
                NRF_LOG_PRINTF(": SENDING\n");
            }
						
						SysTick_DelayMs(1000);
				
    }
}


int rx_main(void) {
	TM_NRF24L01_Transmit_Status_t transmissionStatus;

	
	
	/* Initialize NRF24L01+ on channel 15 and 32bytes of payload */
	/* By default 2Mbps data rate and 0dBm output power */
	/* NRF24L01 goes to RX mode by default */
	TM_NRF24L01_Init(15, 32);
	
	/* Set RF settings, Data rate to 2Mbps, Output power to -18dBm */
	TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_2M, TM_NRF24L01_OutputPower_0dBm);
	
	/* Set my address, 5 bytes */
	TM_NRF24L01_SetMyAddress(MyAddress);
	/* Set TX address, 5 bytes */
	TM_NRF24L01_SetTxAddress(TxAddress);

	while (1) {
		/* If data is ready on NRF24L01+ */
		if (TM_NRF24L01_DataReady()) {
			/* Get data from NRF24L01+ */
			TM_NRF24L01_GetData(dataIn);
			
			/* Send it back, automatically goes to TX mode */
			TM_NRF24L01_Transmit(dataIn);
			
			/* Start send */
			LedOn();
			/* Wait for data to be sent */
			do {
				transmissionStatus = TM_NRF24L01_GetTransmissionStatus();
			} while (transmissionStatus == TM_NRF24L01_Transmit_Status_Sending);
			/* Send done */
			LedOff();
			
			/* Go back to RX Mode */
			TM_NRF24L01_PowerUpRx();		
		}
	}
}

/**@brief Init system clock use HSI
 */
void SystemClockInit(void)
{                                                                           
  /*Configure all clocks to max for best performance.
   * If there are EMI, power, or noise problems, try slowing the clocks*/

  /* First set the flash latency to work with our clock*/
  /*000 Zero wait state, if 0  MHz < SYSCLK <= 24 MHz
    001 One wait state, if  24 MHz < SYSCLK <= 48 MHz
    010 Two wait states, if 48 MHz < SYSCLK <= 72 MHz */
  FLASH_SetLatency(FLASH_Latency_1);

  /* Start with HSI clock (internal 8mhz), divide by 2 and multiply by 9 to
   * get maximum allowed frequency: 36Mhz
   * Enable PLL, wait till it's stable, then select it as system clock*/
  RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Set HCLK, PCLK1, and PCLK2 to SCLK (these are default */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div1);
  RCC_PCLK2Config(RCC_HCLK_Div1);
}


/*****END OF FILE****/
