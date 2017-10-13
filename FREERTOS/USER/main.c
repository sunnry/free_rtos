#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "misc.h"
#include "led.h"
#include "nrf24.h"

//#define		PURE_TX_DEBUG			1

#define nRF24_WAIT_TIMEOUT         (uint32_t)0x000FFFFF

// Result of packet transmission
typedef enum {
	nRF24_TX_ERROR  = (uint8_t)0x00, // Unknown error
	nRF24_TX_SUCCESS,                // Packet has been transmitted successfully
	nRF24_TX_TIMEOUT,                // It was timeout during packet transmit
	nRF24_TX_MAXRT                   // Transmit failed with maximum auto retransmit count
} nRF24_TXResult;


void NRF24TX_Task(void * pvParameters);
void LED0_Task(void * pvParameters);
void LED1_Task(void * pvParameters);

static void preSetupHardware(void);

static void NRF_GPIO_Init(void);

static nRF24_TXResult nRF24_TransmitPacket(uint8_t *pBuf, uint8_t length);

int main(void)
{
	  preSetupHardware();
    LED_Init();
		NRF_GPIO_Init();

    xTaskCreate(LED0_Task, (const char *)"LED0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(LED1_Task, (const char *)"LED1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

		xTaskCreate(NRF24TX_Task, (const char *)"NRF24TX_Task", 512, NULL, configMAX_PRIORITIES - 1, NULL);
	
    vTaskStartScheduler();
}

void NRF24TX_Task(void * pvParameters){
		
  /*configure nrf24 transmit parameters*/	
	static nRF24_TXResult tx_res;
	static const uint8_t nRF24_ADDR[] = {'E','S','B'};
	uint8_t payload_length = 10;
	uint8_t nrf24_payload[10] = {'i',' ','l','o','v','e',' ','s','t','m'};
	
	nRF24_SetRFChannel(40);  
	nRF24_SetDataRate(nRF24_DR_250kbps);
	nRF24_SetCRCScheme(nRF24_CRC_2byte);
	nRF24_SetAddrWidth(3);
		
	//configure TX PIPE
	nRF24_SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address
	nRF24_SetAddr(nRF24_PIPE0, nRF24_ADDR);  // program address for pipe#0, must be same as TX (for Auto-ACK)
		
	nRF24_SetTXPower(nRF24_TXPWR_0dBm); // Set TX power (maximum)
		
	nRF24_SetAutoRetr(nRF24_ARD_2500us, 10); // Configure auto retransmit: 10 retransmissions with pause of 2500s in between
		
	nRF24_EnableAA(nRF24_PIPE0); // Enable Auto-ACK for pipe#0 (for ACK packets)
	
	#ifdef PURE_TX_DEBUG 
		nRF24_SetAutoRetr(nRF24_ARD_2500us, 0);
		nRF24_DisableAA(nRF24_PIPE0);
	#endif
		
	nRF24_SetOperationalMode(nRF24_MODE_TX); // Set operational mode (PTX == transmitter)
		
	nRF24_ClearIRQFlags(); // Clear any pending IRQ flags
		
	nRF24_SetPowerMode(nRF24_PWR_UP); // Wake the transceiver

	
	for(;;){
		
		tx_res = nRF24_TransmitPacket(nrf24_payload, payload_length);
		
		switch (tx_res) {
			case nRF24_TX_SUCCESS:
				//UART_SendStr("OK");
				break;
			case nRF24_TX_TIMEOUT:
				//UART_SendStr("TIMEOUT");
				break;
			case nRF24_TX_MAXRT:
				//UART_SendStr("MAX RETRANSMIT");
				break;
			default:
				//UART_SendStr("ERROR");
				break;
		}
    	//UART_SendStr("\r\n");
		vTaskDelay(5 / portTICK_PERIOD_MS);  //delay 5ms
	}
	
}


void LED0_Task(void * pvParameters)
{
    while (1)
    {
			  //GPIO_ResetBits(GPIOA,GPIO_Pin_8);
				GPIOA->ODR ^= GPIO_Pin_8;
        vTaskDelay(800 / portTICK_RATE_MS);
    }
}

void LED1_Task(void * pvParameters)
{
    while (1)
    {
			  //GPIO_SetBits(GPIOA,GPIO_Pin_8);
				GPIOD->ODR ^= GPIO_Pin_2;
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

void NRF_GPIO_Init(void){	
	
	GPIO_InitTypeDef nrf24_irq;
	GPIO_InitTypeDef nrf24_spi;
	
	SPI_InitTypeDef	 SPI;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	//configure nf24 irq
	nrf24_irq.GPIO_Mode = GPIO_Mode_Out_PP;
	nrf24_irq.GPIO_Speed = GPIO_Speed_2MHz;
	nrf24_irq.GPIO_Pin = nRF24_IRQ_PIN;
	GPIO_Init(nRF24_IRQ_PORT,&nrf24_irq);
	
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
	
	// Initialize the nRF24L01 GPIO pins
	nRF24_GPIO_Init();
	
	// RX/TX disabled
  nRF24_CE_L();
	
	//check NRF state
	while (!nRF24_Check()) {
  };
	
	nRF24_Init();
}


static void preSetupHardware(void){
	
		RCC_DeInit(); //Reset the RCC Clock configuration to the default reset state
	
		RCC_HSEConfig(RCC_HSE_ON); //configure the external high speed oscillator
		
		while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET){//wait HSE ready
		}
	
		//HCLK = SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		
		/* PCLK2 = HCLK */
		RCC_PCLK2Config( RCC_HCLK_Div1 );

		/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config( RCC_HCLK_Div2 );

		/* PLLCLK = 8MHz * 9 = 72 MHz. */
		RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );

		/* Enable PLL. */
		RCC_PLLCmd( ENABLE );

		/* Wait till PLL is ready. */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){
		}

		/* Select PLL as system clock source. */
		RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

		/* Wait till PLL is used as system clock source. */
		while( RCC_GetSYSCLKSource() != 0x08 ){
		}

		NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );		
		
		/* Configure HCLK clock as SysTick clock source. */
		SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
		
}


// Function to transmit data packet
// input:
//   pBuf - pointer to the buffer with data to transmit
//   length - length of the data buffer in bytes
// return: one of nRF24_TX_xx values

static nRF24_TXResult nRF24_TransmitPacket(uint8_t *pBuf, uint8_t length) {
	volatile uint32_t wait = nRF24_WAIT_TIMEOUT;
	uint8_t status;

	// Deassert the CE pin (in case if it still high)
	nRF24_CE_L();

	// Transfer a data from the specified buffer to the TX FIFO
	nRF24_WritePayload(pBuf, length);

	// Start a transmission by asserting CE pin (must be held at least 10us)
	nRF24_CE_H();

	// Poll the transceiver status register until one of the following flags will be set:
	//   TX_DS  - means the packet has been transmitted
	//   MAX_RT - means the maximum number of TX retransmits happened
	// note: this solution is far from perfect, better to use IRQ instead of polling the status
	do {
		status = nRF24_GetStatus();
		if (status & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT)) {
			break;
		}
	} while (wait--);

	// Deassert the CE pin (Standby-II --> Standby-I)
	nRF24_CE_L();

	if (!wait) {
		// Timeout
		return nRF24_TX_TIMEOUT;
	}

	// Check the flags in STATUS register
	//UART_SendStr("[");
	//UART_SendHex8(status);
	//UART_SendStr("] ");

	// Clear pending IRQ flags
    nRF24_ClearIRQFlags();

	if (status & nRF24_FLAG_MAX_RT) {
		// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
		return nRF24_TX_MAXRT;
	}

	if (status & nRF24_FLAG_TX_DS) {
		// Successful transmission
		return nRF24_TX_SUCCESS;
	}

	// Some banana happens, a payload remains in the TX FIFO, flush it
	nRF24_FlushTX();

	return nRF24_TX_ERROR;
}
