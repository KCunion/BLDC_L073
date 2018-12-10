#ifndef __HALL_H
#define __HALL_H	 
#include "sys.h"

#define HALL_RCC_CLOCKGPIO            RCC_AHBPeriph_GPIOC
#define HALL_RCC_CLOCKAFIO            RCC_APB2Periph_AFIO
#define HALL_GPIO_PORTSOURCE          EXTI_PortSourceGPIOC
#define HALL_EXTI_IRQHandler          EXTI4_15_IRQHandler
#define HALL_EXTI_IRQn                EXTI4_15_IRQn
#define HALL_PORT                     GPIOC

#define HALL_U_PIN          GPIO_Pin_6
#define HALL_U_PINSOURCE    EXTI_PinSource6
#define HALL_U_EXITLINE     EXTI_Line6
#define HALL_V_PIN          GPIO_Pin_7
#define HALL_V_PINSOURCE    EXTI_PinSource7
#define HALL_V_EXITLINE     EXTI_Line7
#define HALL_W_PIN          GPIO_Pin_8
#define HALL_W_PINSOURCE    EXTI_PinSource8
#define HALL_W_EXITLINE     EXTI_Line8

void hall_init(void);
			    
#endif
