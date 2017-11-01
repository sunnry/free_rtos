#include "FreeRTOS.h"
#include "misc.h"
#include "nrf24.h"
#include "nrf24_handler.h"


void NRF24_IRQ_Init(void){
	
	GPIO_InitTypeDef nrf24_irq;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 
	
	//configure nf24 irq
	nrf24_irq.GPIO_Mode = GPIO_Mode_Out_PP;
	nrf24_irq.GPIO_Speed = GPIO_Speed_2MHz;
	nrf24_irq.GPIO_Pin = nRF24_IRQ_PIN;
	GPIO_Init(nRF24_IRQ_PORT,&nrf24_irq);

}

void NRF24_SPI_Init(void){
	
	GPIO_InitTypeDef nrf24_spi;
	SPI_InitTypeDef	 SPI;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	//configure spi1 pins
	nrf24_spi.GPIO_Mode = GPIO_Mode_AF_PP;
	nrf24_spi.GPIO_Speed = GPIO_Speed_50MHz;
	nrf24_spi.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA,&nrf24_spi);
	
	//configure spi1
	SPI.SPI_Mode = SPI_Mode_Master;
	SPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//SPI_BaudRatePrescaler_2;
	SPI.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI.SPI_CPOL = SPI_CPOL_Low;
  SPI.SPI_CPHA = SPI_CPHA_1Edge;
  SPI.SPI_CRCPolynomial = 7;
  SPI.SPI_DataSize = SPI_DataSize_8b;
  SPI.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(nRF24_SPI_PORT,&SPI);
	SPI_NSSInternalSoftwareConfig(nRF24_SPI_PORT, SPI_NSSInternalSoft_Set);
	SPI_Cmd(nRF24_SPI_PORT, ENABLE);
}


void NRF_CE_CS_RCC_Configuration(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //CE pin
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);   //CS pin
}


void NRF_GPIO_Init(void){	
		
	NRF_CE_CS_RCC_Configuration();
	NRF24_IRQ_Init();
	NRF24_SPI_Init();
		
	// Initialize the nRF24L01 CE CS GPIO pins
	nRF24_GPIO_Init();
	
	// RX/TX disabled
  nRF24_CE_L();
	
	//check NRF state
	while (!nRF24_Check()) {
  };
	
	nRF24_Init();
}
