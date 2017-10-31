#include "key_handler.h"
#include "stm32f10x_exti.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "portmacro.h"
#include "predefines.h"

extern SemaphoreHandle_t  key_xSemaphore;

void EXTI_KEY_Configuration(void){
	
	EXTI_InitTypeDef	EXTI1_key0_InitStructure;
	
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
	EXTI1_key0_InitStructure.EXTI_Line    = EXTI_Line1;
	EXTI1_key0_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI1_key0_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI1_key0_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI1_key0_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2;  //数字越小优先级越高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


void EXTI1_IRQHandler(void){
	
	BaseType_t xHigherPriorityTaskWoken;
	
	//EXTI->IMR &= ~(1<<(EXTI_Line1 - 1));
	
	xHigherPriorityTaskWoken = pdFALSE;
	
	if(EXTI_GetITStatus(EXTI_Line1) == SET){
		
		xSemaphoreGiveFromISR(key_xSemaphore,&xHigherPriorityTaskWoken);
		
	}
	
	EXTI_ClearITPendingBit(EXTI_Line1);
	
	//EXTI->IMR |= 1 << (EXTI_Line1 - 1);
	
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


