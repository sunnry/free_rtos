#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "misc.h"
#include "semphr.h"

extern SemaphoreHandle_t  key_xSemaphore;

void LED_GPIO_Init(void){
	
	GPIO_InitTypeDef  LED0;
	GPIO_InitTypeDef	LED1;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	LED0.GPIO_Pin = GPIO_Pin_8;
	LED0.GPIO_Mode = GPIO_Mode_Out_PP;
	LED0.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&LED0);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	LED1.GPIO_Pin = GPIO_Pin_2;
	LED1.GPIO_Mode = GPIO_Mode_Out_PP;
	LED1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&LED1);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
}

void LED0_Task(void * pvParameters)
{
    while (1)
    {
				if(xSemaphoreTake(key_xSemaphore,portMAX_DELAY) == pdTRUE){
					GPIOA->ODR ^= GPIO_Pin_8;
					//vTaskDelay(800 / portTICK_RATE_MS);
				}
    }
}

void LED1_Task(void * pvParameters)
{
    while (1)
    {
			  //GPIO_SetBits(GPIOA,GPIO_Pin_2);
				GPIOD->ODR ^= GPIO_Pin_2;
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

