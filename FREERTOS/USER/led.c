#include "led.h"
#include "FreeRTOS.h"
#include "misc.h"




void LED_Init(void){
	
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
