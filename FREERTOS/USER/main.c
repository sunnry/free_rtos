#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "misc.h"
#include "led.h"


void LED0_Task(void * pvParameters);
void LED1_Task(void * pvParameters);

static void preSetupHardware(void);


int main(void)
{
	  preSetupHardware();
    LED_Init();

    xTaskCreate(LED0_Task, (const char *)"LED0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(LED1_Task, (const char *)"LED1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, NULL);

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

static void preSetupHardware(void){
}
