
#include "stm32f10x.h"
#include "nRF_API.h"
#include "hw_config.h"
#include "rtt_log.h"
//**********************************************************//

//**********************************************************//
// Declare HW/SW SPI Mode variable
BYTE SPI_Mode = SW_MODE;

// Variable that indicates nRF24L01 interrupt source
BYTE IRQ_Source;
//********************************************************************************************************************//

void CE_Pin(BYTE state)                                // Set/reset CSN pin
{
  if(state)
    GPIO_SetBits(NRF_CE_PORT, NRF_CE_PIN);
  else
    GPIO_ResetBits(NRF_CE_PORT, NRF_CE_PIN);
}

void CSN_Pin(BYTE state)                                // Set/reset CSN pin
{
  if(state)
    GPIO_SetBits(NRF_CSN_PORT, NRF_CSN_PIN);
  else
    GPIO_ResetBits(NRF_CSN_PORT, NRF_CSN_PIN);
}

void SCK_Pin(BYTE state)                                // Set/reset SCK pin
{
  if(state)
    GPIO_SetBits(NRF_SCK_PORT, NRF_SCK_PIN);
  else
    GPIO_ResetBits(NRF_SCK_PORT, NRF_SCK_PIN);
}

void MOSI_Pin(BYTE state)                               // Set/reset MOSI pin
{
  if(state)
    GPIO_SetBits(NRF_MOSI_PORT, NRF_MOSI_PIN);
  else
    GPIO_ResetBits(NRF_MOSI_PORT, NRF_MOSI_PIN);
}

BYTE MISO_Pin(void)                                     // Read MISO pin
{
  return GPIO_ReadInputDataBit(NRF_MISO_PORT, NRF_MISO_PIN);
}

BYTE SPI_HW(BYTE byte)                                  // Write one byte using F320's hardware SPI
{
  return(byte);                                      // return received byte
}
//**********************************************************//
//
//  Function: SPI_RW
//
//  Description:
//  Writes one byte to nRF24L01, and return the byte read
//  from nRF24L01 during write, according to SPI protocol
//
//  In/Out parameters:
//  In: 'byte', current byte to be written
//  Out: 'SPI0DAT', HW SPI mode, 'byte' SW SPI mode,
//
//  Author: RSK   Date: 28.11.05
//**********************************************************//
BYTE SPI_RW(BYTE byte)
{
	BYTE bit_ctr;
  if (SPI_Mode == HW_MODE)                        // Select HW or SW SPI..
  {
    return(SPI_HW(byte));                         // Perform HW SPI operation
  }
  else                                            // software SPI....
  {
    for(bit_ctr=0;bit_ctr<8;bit_ctr++)
    {
      MOSI_Pin(byte & 0x80);                      // output 'byte', MSB to MOSI
      byte = (byte << 1);                         // shift next bit into MSB..
      SCK_Pin(1);                                 // Set SCK high..
      byte |= MISO_Pin();                         // capture current MISO bit
      SCK_Pin(0);                                 // ..then set SCK low again
    }
    MOSI_Pin(0);                                  // MOSI pin low before return

    return(byte);                                 // return 'read' byte
  }
}


//**********************************************************//
//
//  Function: SPI_Read
//
//  Description:
//  Read one byte from nRF24L01 register, 'reg'
//
//
//  In/Out parameters:
//  In: reg, register to read
//  Out: return reg_val, register value.
//
//
//  Author: RSK   Date: 28.11.05
//**********************************************************//
BYTE SPI_Read(BYTE reg)
{
	BYTE reg_val;
                                      
  CSN_Pin(0); 																		// CSN low, initialize SPI communication...
  SPI_RW(reg);                                    // Select register to read from..
  reg_val = SPI_RW(0);                            // ..then read registervalue
  CSN_Pin(1);                                     // CSN high, terminate SPI communication

  return(reg_val);                                // return register value
}


//**********************************************************//
//
//  Function: SPI_RW_Reg
//
//  Description:
//  Writes value 'value' to register 'reg'
//
//
//  In/Out parameters:
//  In: 'reg' register to write value 'value' to.
//  Return status byte.
//
//  Author: RSK   Date: 28.11.05
//**********************************************************//
BYTE SPI_RW_Reg(BYTE reg, BYTE value)
{
BYTE status;

  CSN_Pin(0);                                     // CSN low, init SPI transaction
  status = SPI_RW(reg);                           // select register
  SPI_RW(value);                                  // ..and write value to it..
  CSN_Pin(1);                                     // CSN high again

  return(status);                                 // return nRF24L01 status byte
}


//**********************************************************//
//
//  Function: SPI_Write_Buf
//
//  Description:
//  Writes contents of buffer '*pBuf' to nRF24L01
//  Typically used to write TX payload, Rx/Tx address
//
//
//  In/Out parameters:
//  In: register 'reg' to write, buffer '*pBuf*' contains
//  data to be written and buffer size 'buf_size' is #of
//  bytes to be written
//  Out: return nRF24L01 status byte.
//
//  Author: RSK   Date: 28.11.05
//**********************************************************//
BYTE SPI_Write_Buf(BYTE reg, BYTE *pBuf, BYTE bytes)
{
BYTE status,byte_ctr;

  CSN_Pin(0);                                     // Set CSN low, init SPI tranaction
  status = SPI_RW(reg);                           // Select register to write to and read status byte

  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)     // then write all byte in buffer(*pBuf)
    SPI_RW(*pBuf++);

  CSN_Pin(1);                                     // Set CSN high again

  return(status);                                 // return nRF24L01 status byte
}


//**********************************************************//
//
//  Function: SPI_Read_Buf
//
//  Description:
//  Reads 'bytes' #of bytes from register 'reg'
//  Typically used to read RX payload, Rx/Tx address
//
//
//  In/Out parameters:
//  In: 'reg', register to read from, '*pBuf' are buffer
//  the read bytes are stored to and 'bytes' are #of bytes
//  to read.
//  Out: return nRF24L01 status byte.
//
//  Author: RSK   Date: 28.11.05
//**********************************************************//
BYTE SPI_Read_Buf(BYTE reg, BYTE *pBuf, BYTE bytes)
{
BYTE status,byte_ctr;

  CSN_Pin(0);                                     // Set CSN low, init SPI tranaction
  status = SPI_RW(reg);                           // Select register to write to and read status byte

  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0);                   // Perform SPI_RW to read byte from nRF24L01

  CSN_Pin(1);                                     // Set CSN high again

  return(status);                                 // return nRF24L01 status byte
}



//**********************************************************
//
//  nRF24L01 Program example
//
//  Description:
//  This example code is contains two functions,
//  'TX_Mode': program nRF24L01 to function as a PRIM:TX,
//  i.e. program TX_Address, RX_Address for auto ack,
//  TX Payload, setup of 'Auto Retransmit Delay' &
//  'Auto Retransmit Count', select RF channel,
//  Datarate & RF output power.
//
//  'RX_Mode': program nRF24L01 to function as a PRIM:RX,
//  i.e. ready to receive the packet that was sent with
//  the 'TX_Mode' function.
//
//
//  Author: RSK   Date: 28.11.05
//**********************************************************


// Predefine a static TX address
BYTE TX_ADDRESS[TX_ADR_LENGTH]  = {0x34,0x43,0x10,0x10,0x01}; 
// Predefine TX payload packet..
BYTE TX_PAYLOAD[TX_PLOAD_WIDTH] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                                         0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};  

BYTE SPI_Buffer[32];                       // Buffer to hold data from 'SPI_Read_Buf()' function

//**********************************************************
//
//  Function: TX_Mode
//
//  Description:
//  This function initializes one nRF24L01 device to
//  TX mode, set TX address, set RX address for auto.ack,
//  fill TX payload, select RF channel, datarate & TX pwr.
//  PWR_UP is set, CRC(2 bytes) is enabled, & PRIM:TX.
//
//  ToDo: One high pulse(>10탎) on CE will now send this
//  packet and expext an acknowledgment from the RX device.
//
//
//  Author: RSK   Date: 28.11.05
//**********************************************************
void TX_Mode(void)
{
  SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_LENGTH);    // Writes TX_Address to nRF24L01
  SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_LENGTH); // RX_Addr0 same as TX_Adr for Auto.Ack
  SPI_Write_Buf(NRF_WR_TX_PLOAD, TX_PAYLOAD, TX_PLOAD_WIDTH); // Writes data to TX payload

  SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x0);            // Enable Auto.Ack:Pipe0
  SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);        // Enable Pipe0
  SPI_RW_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1a);       // 500탎 + 86탎, 10 retrans...
  SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 40);              // Select RF channel 40
  SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);         // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0e);           // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..

	CE_Pin(1);
  //  This device is now ready to transmit one packet of 16 bytes payload to a RX device at address
  //  '3443101001', with auto acknowledgment, retransmit count of 10(retransmit delay of 500탎+86탎)
  //  RF channel 40, datarate = 2Mbps with TX power = 0dBm.
}


//**********************************************************
//
//  Function: RX_Mode
//
//  Description:
//  This function initializes one nRF24L01 device to
//  RX Mode, set RX address, writes RX payload width,
//  select RF channel, datarate & LNA HCURR.
//  After init, CE is toggled high, which means that
//  this device is now ready to receive a datapacket.
//
//  Author: RSK   Date: 28.11.05
//**********************************************************
void RX_Mode(void)
{
  SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_LENGTH); // Use the same address on the RX device as the TX device

  SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x00);            // Enable Auto.Ack:Pipe0
  SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);        // Enable Pipe0
  SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 40);              // Select RF channel 40
  SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); // Select same RX payload width as TX Payload width
  SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);         // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0f);           // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..

  CE_Pin(1); // Clear CE pin high to enable RX device

  //  This device is now ready to receive one packet of 16 bytes payload from a TX device sending to address
  //  '3443101001', with auto acknowledgment, retransmit count of 10, RF channel 40 and datarate = 2Mbps.

}



void EXTI9_5_IRQHandler(void)
{
	BYTE temp,rx_pw;
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)	
	{
		
		
		temp = SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0x70);    // Read STATUS byte and clear IRQ flag's(nRF24L01)

		if(temp & MAX_RT) 
		{
			NRF_LOG_PRINTF("Indicates max #of retransmit interrupt\r\n");
			IRQ_Source = MAX_RT;          // Indicates max #of retransmit interrupt
		}
		if(temp & TX_DS)
		{
			NRF_LOG_PRINTF("Indicates TX data succsessfully sent\r\n");
			IRQ_Source = TX_DS;           // Indicates TX data succsessfully sent
		}
		if(temp & RX_DR)  // In RX mode, check for data received
		{
			NRF_LOG_PRINTF("In RX mode, check for data received\r\n");
			// Data received, so find out which datapipe the data was received on:
//			temp = (0x07 & (temp > 1)); // Shift bits in status byte one bit to LSB and mask 'Data Pipe Number'
//			rx_pw = SPI_Read(NRF_READ_REG + RX_PW_P0 + temp); // Read current RX_PW_Pn register, where Pn is the pipe the data was received on..
//			SPI_Read_Buf(NRF_RD_RX_PLOAD, SPI_Buffer, rx_pw); // Data from RX Payload register is now copied to SPI_Buffer[].

			IRQ_Source = RX_DR; // Indicates RX data received
		}
//		NRF_LOG_PRINTF("temp %d\r\n", temp);
		LED_1_PORT->ODR ^= LED_1_PIN;
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}



