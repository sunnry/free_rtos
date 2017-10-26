#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "misc.h"
#include "predefines.h"
#include "led.h"
#include "nrf24.h"
#include "nrf24_tx_task.h"
#include "nrf24_rx_task.h"
#include "key_handler.h"


void LED0_Task(void * pvParameters);
void LED1_Task(void * pvParameters);

static void preSetupHardware(void);

static void NRF_GPIO_Init(void);


int main(void)
{
	  preSetupHardware();
    LED_Init();
		NRF_GPIO_Init();
		EXTI_KEY_Configuration();

    xTaskCreate(LED0_Task, (const char *)"LED0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(LED1_Task, (const char *)"LED1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

#ifdef TRANSMIT_MODE
		xTaskCreate(NRF24TX_Task, (const char *)"NRF24TX_Task", 512, NULL, configMAX_PRIORITIES - 1, NULL);
#else	
		xTaskCreate(NRF24RX_Task, (const char *)"NRF24RX_Task", 512, NULL, configMAX_PRIORITIES - 1, NULL);
#endif	
    vTaskStartScheduler();
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
	
	GPIO_InitTypeDef key0_exti;
	
	SPI_InitTypeDef	 SPI;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	//configure nf24 irq
	nrf24_irq.GPIO_Mode = GPIO_Mode_Out_PP;
	nrf24_irq.GPIO_Speed = GPIO_Speed_2MHz;
	nrf24_irq.GPIO_Pin = nRF24_IRQ_PIN;
	GPIO_Init(nRF24_IRQ_PORT,&nrf24_irq);
	
	//configure key0 EXTI interrupt
	key0_exti.GPIO_Mode = GPIO_Mode_IPU;
	//key0_exti.GPIO_Speed = GPIO_Speed_50MHz;
	key0_exti.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOC,&key0_exti);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);
	
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


