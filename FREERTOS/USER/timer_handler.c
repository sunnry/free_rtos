#include "FreeRTOS.h"
#include "semphr.h"
#include "timer_handler.h"
#include "stm32f10x_tim.h"

extern SemaphoreHandle_t  key_xSemaphore;

static unsigned int kcount = 0; 

void TIMER3_RCC_Configuration(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
}

void TIMER3_Configuration(void){
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
    
  TIM_TimeBaseStructure.TIM_Period = TIMER3_PERIOD;//??:72MHz  72000000/36000=2000  
  TIM_TimeBaseStructure.TIM_Prescaler = TIMER3_PRESCALER;//36000-1=35999  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
    
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );  
  TIM_Cmd(TIM3,ENABLE); 
}

void NVIC_TIMER3_Configuration(void){
	
	NVIC_InitTypeDef NVIC_InitStructure;    
            
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;    
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;    
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
  NVIC_Init(&NVIC_InitStructure); 
	
}


void TIM3_IRQHandler(void){
	
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
  if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)==Bit_RESET)  
  {  
		kcount ++;
		
		if(kcount >= KEY_DETECT_TIMES){
			xSemaphoreGiveFromISR(key_xSemaphore,&xHigherPriorityTaskWoken);
			kcount = 0;
		} 
  }  
}

