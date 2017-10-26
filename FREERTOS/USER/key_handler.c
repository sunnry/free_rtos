#include "key_handler.h"
#include "stm32f10x_exti.h"

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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


void EXTI1_IRQHandler(void){
	
	if(EXTI_GetITStatus(EXTI_Line1) == SET){
		
	}
	
	EXTI_ClearITPendingBit(EXTI_Line1);
}


