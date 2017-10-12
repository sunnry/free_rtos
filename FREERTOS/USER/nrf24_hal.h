#ifndef __NRF24_HAL_H
#define __NRF24_HAL_H


// Hardware abstraction layer for NRF24L01+ transceiver (hardware depended functions)
// GPIO pins definition
// GPIO pins initialization and control functions
// SPI transmit functions


// Peripheral libraries
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>

//the following SPI configuration is based on mini stm32f103rct6 development board

// SPI port peripheral
#define nRF24_SPI_PORT             SPI1

// nRF24 GPIO peripherals
#define nRF24_GPIO_PERIPHERALS     (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN)

// CE (chip enable) pin (PA4)
#define nRF24_CE_PORT              GPIOA
#define nRF24_CE_PIN               GPIO_Pin_4
#define nRF24_CE_L()               GPIO_ResetBits(nRF24_CE_PORT, nRF24_CE_PIN)
#define nRF24_CE_H()               GPIO_SetBits(nRF24_CE_PORT, nRF24_CE_PIN)

// CSN (chip select negative) pin (PC4)
#define nRF24_CSN_PORT             GPIOC
#define nRF24_CSN_PIN              GPIO_Pin_4
#define nRF24_CSN_L()              GPIO_ResetBits(nRF24_CSN_PORT, nRF24_CSN_PIN)
#define nRF24_CSN_H()              GPIO_SetBits(nRF24_CSN_PORT, nRF24_CSN_PIN)

// IRQ pin (PC5)
#define nRF24_IRQ_PORT             GPIOC
#define nRF24_IRQ_PIN              GPIO_Pin_5


// Function prototypes
void nRF24_GPIO_Init(void);
uint8_t nRF24_LL_RW(uint8_t data);

#endif // __NRF24_HAL_H
