#ifndef __HALL_H
#define __HALL_H	 
#include "sys.h"

#define HALL_RCC_CLOCKGPIO            RCC_AHBPeriph_GPIOC
#define HALL_RCC_CLOCKAFIO            RCC_APB2Periph_SYSCFG
#define HALL_GPIO_PORTSOURCE          EXTI_PortSourceGPIOC
#define HALL_EXTI_IRQHandler          EXTI4_15_IRQHandler
#define HALL_EXTI_IRQn                EXTI4_15_IRQn
#define HALL_PORT                     GPIOC

#define HALL_U_PIN          GPIO_Pin_13
#define HALL_U_PINSOURCE    EXTI_PinSource13
#define HALL_U_EXITLINE     EXTI_Line13
#define HALL_V_PIN          GPIO_Pin_14
#define HALL_V_PINSOURCE    EXTI_PinSource14
#define HALL_V_EXITLINE     EXTI_Line14
#define HALL_W_PIN          GPIO_Pin_15
#define HALL_W_PINSOURCE    EXTI_PinSource15
#define HALL_W_EXITLINE     EXTI_Line15

void hall_init(void);
			    
#endif
