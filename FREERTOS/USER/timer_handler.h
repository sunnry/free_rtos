#ifndef  __TIMER_HANDLER_H
#define	 __TIMER_HANDLER_H

#define 	KEY_DETECT_TIMES		10
#define		TIMER3_PRESCALER		35999     //36000-1=35999 
#define		TIMER3_PERIOD				100				//72MHz  72000000/36000=2000 max

void TIMER3_RCC_Configuration(void);

void TIMER3_Configuration(void);

void NVIC_TIMER3_Configuration(void);

#endif

