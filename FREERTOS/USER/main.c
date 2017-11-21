#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "misc.h"
#include "semphr.h"
#include "predefines.h"
#include "led.h"
#include "nrf24_handler.h"
#include "nrf24.h"
#include "nrf24_tx_task.h"
#include "nrf24_rx_task.h"
#include "key_handler.h"
#include "timer_handler.h"
#include "protocol.h"

SemaphoreHandle_t  key_xSemaphore;

QueueHandle_t  userInputQueueHandler = NULL;

static void preSetupHardware(void);

QueueHandle_t createRequiredQueue(void){
	return xQueueCreate(1,sizeof(struct ControlFrameCtx));
}



int main(void)
{
		userInputQueueHandler = createRequiredQueue();
	
	  preSetupHardware();
	
    LED_GPIO_Init();
	
		NRF_GPIO_Init();
	
		//EXTI_KEY0_GPIO_Init();  //this two lines of code use external interput to handle key press
		//EXTI_KEY0_Configuration();   
	
		TIMER3_RCC_Configuration();  //this three lines of code use timer3 interrupt to handle key press  
		TIMER3_Configuration();
		NVIC_TIMER3_Configuration();
	
		key_xSemaphore = xSemaphoreCreateBinary();

    xTaskCreate(LED0_Task, (const char *)"LED0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(LED1_Task, (const char *)"LED1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

#ifdef TRANSMIT_MODE
		xTaskCreate(NRF24TX_Task, (const char *)"NRF24TX_Task", 512, NULL, configMAX_PRIORITIES - 1, NULL);
#else	
		xTaskCreate(NRF24RX_Task, (const char *)"NRF24RX_Task", 512, NULL, configMAX_PRIORITIES - 1, NULL);
#endif	
    vTaskStartScheduler();
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


