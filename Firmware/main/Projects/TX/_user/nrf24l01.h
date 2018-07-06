
#ifndef TM_NRF24L01_H
#define TM_NRF24L01_H 111

#include "stm32f10x.h"
#include "hw_config.h"
#include "nRF_API.h"

/* Pins configuration */
#define NRF24L01_CE_LOW				GPIO_ResetBits(NRF_CE_PORT, NRF_CE_PIN)
#define NRF24L01_CE_HIGH			GPIO_SetBits(NRF_CE_PORT, NRF_CE_PIN)
#define NRF24L01_CSN_LOW			GPIO_ResetBits(NRF_CSN_PORT, NRF_CSN_PIN)
#define NRF24L01_CSN_HIGH			GPIO_SetBits(NRF_CSN_PORT, NRF_CSN_PIN)

/**
 * @}
 */
 
/**
 * @defgroup TM_NRF24L01P_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Transmission status enumeration
 */
typedef enum {
	TM_NRF24L01_Transmit_Status_Lost = 0x00,   /*!< Message is lost, reached maximum number of retransmissions */
	TM_NRF24L01_Transmit_Status_Ok = 0x01,     /*!< Message sent successfully */
	TM_NRF24L01_Transmit_Status_Sending = 0xFF /*!< Message is still sending */
} TM_NRF24L01_Transmit_Status_t;

/**
 * @brief  Data rate enumeration
 */
typedef enum {
	TM_NRF24L01_DataRate_2M,  /*!< Data rate set to 2Mbps */
	TM_NRF24L01_DataRate_1M,  /*!< Data rate set to 1Mbps */
	TM_NRF24L01_DataRate_250k /*!< Data rate set to 250kbps */
} TM_NRF24L01_DataRate_t;

/**
 * @brief  Output power enumeration
 */
typedef enum {
	TM_NRF24L01_OutputPower_M18dBm,	/*!< Output power set to -18dBm */
	TM_NRF24L01_OutputPower_M12dBm, /*!< Output power set to -12dBm */
	TM_NRF24L01_OutputPower_M6dBm,  /*!< Output power set to -6dBm */
	TM_NRF24L01_OutputPower_0dBm    /*!< Output power set to 0dBm */
} TM_NRF24L01_OutputPower_t;

/* Clear interrupt flags */
#define NRF24L01_CLEAR_INTERRUPTS   do { TM_NRF24L01_WriteRegister(0x07, 0x70); } while (0)

/* Gets interrupt status from device */
#define NRF24L01_GET_INTERRUPTS     TM_NRF24L01_GetStatus()

/* Interrupt masks */
#define NRF24L01_IRQ_DATA_READY     0x40 /*!< Data ready for receive */
#define NRF24L01_IRQ_TRAN_OK        0x20 /*!< Transmission went OK */
#define NRF24L01_IRQ_MAX_RT         0x10 /*!< Max retransmissions reached, last transmission failed */

/**
 * @}
 */

/**
 * @defgroup TM_NRF24L01P_Functions
 * @brief    Library Functions
 *
 * Here are listed very basic functions to work with NRF modules
 *
 * @{
 */

/**
 * @brief  Initializes NRF24L01+ module
 * @param  channel: channel you will use for communication, from 0 to 125 eg. working frequency from 2.4 to 2.525 GHz
 * @param  payload_size: maximum data to be sent in one packet from one NRF to another.
 * @note   Maximal payload size is 32bytes
 * @retval 1
 */
uint8_t TM_NRF24L01_Init(uint8_t channel, uint8_t payload_size);

/**
 * @brief  Sets own address. This is used for settings own id when communication with other modules
 * @note   "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *         if you want to get successful communication
 * @param  *adr: Pointer to 5-bytes length array with address
 * @retval None
 */
void TM_NRF24L01_SetMyAddress(uint8_t* adr);

/**
 * @brief  Sets address you will communicate with
 * @note   "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *         if you want to get successful communication
 * @param  *adr: Pointer to 5-bytes length array with address
 * @retval None
 */
void TM_NRF24L01_SetTxAddress(uint8_t* adr);

/**
 * @brief  Gets number of retransmissions needed in last transmission
 * @param  None
 * @retval Number of retransmissions, between 0 and 15.
 */
uint8_t TM_NRF24L01_GetRetransmissionsCount(void);

/**
 * @brief  Sets NRF24L01+ to TX mode
 * @note   In this mode is NRF able to send data to another NRF module
 * @param  None
 * @retval None
 */
void TM_NRF24L01_PowerUpTx(void);

/**
 * @brief  Sets NRF24L01+ to RX mode
 * @note   In this mode is NRF able to receive data from another NRF module.
 *         This is default mode and should be used all the time, except when sending data
 * @param  None
 * @retval None
 */
void TM_NRF24L01_PowerUpRx(void);

/**
 * @brief  Sets NRF24L01+ to power down mode
 * @note   In power down mode, you are not able to transmit/receive data.
 *         You can wake up device using @ref TM_NRF24L01_PowerUpTx() or @ref TM_NRF24L01_PowerUpRx() functions
 * @param  None
 * @retval None
 */
void TM_NRF24L01_PowerDown(void);

/**
 * @brief  Gets transmissions status
 * @param  None
 * @retval Transmission status. Return is based on @ref TM_NRF24L01_Transmit_Status_t enumeration
 */
TM_NRF24L01_Transmit_Status_t TM_NRF24L01_GetTransmissionStatus(void);

/**
 * @brief  Transmits data with NRF24L01+ to another NRF module
 * @param  *data: Pointer to 8-bit array with data.
 *         Maximum length of array can be the same as "payload_size" parameter on initialization
 * @retval None
 */
void TM_NRF24L01_Transmit(uint8_t *data);

/**
 * @brief  Checks if data is ready to be read from NRF24L01+
 * @param  None
 * @retval Data ready status:
 *            - 0: No data available for receive in bufferReturns
 *            - > 0: Data is ready to be collected
 */
uint8_t TM_NRF24L01_DataReady(void);

/**
 * @brief  Gets data from NRF24L01+
 * @param  *data: Pointer to 8-bits array where data from NRF will be saved
 * @retval None
 */
void TM_NRF24L01_GetData(uint8_t *data);

/**
 * @brief  Sets working channel
 * @note   Channel value is just an offset in units MHz from 2.4GHz
 *         For example, if you select channel 65, then operation frequency will be set to 2.465GHz.
 * @param  channel: RF channel where device will operate
 * @retval None 
 */
void TM_NRF24L01_SetChannel(uint8_t channel);

/**
 * @brief  Sets RF parameters for NRF24L01+
 * @param  DataRate: Data rate selection for NRF module. This parameter can be a value of @ref TM_NRF24L01_DataRate_t enumeration
 * @param  OutPwr: Output power selection for NRF module. This parameter can be a value of @ref TM_NRF24L01_OutputPower_t enumeration
 * @retval None
 */
void TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_t DataRate, TM_NRF24L01_OutputPower_t OutPwr);

/**
 * @brief  Gets NRLF+ status register value
 * @param  None
 * @retval Status register from NRF
 */
uint8_t TM_NRF24L01_GetStatus(void);

/* Private */
void TM_NRF24L01_WriteRegister(uint8_t reg, uint8_t value);


#endif

