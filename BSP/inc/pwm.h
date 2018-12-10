#ifndef __PWM_H_
#define __PWM_H_
#include "HAL_device.h"
#include "HAL_conf.h"
extern uint32_t SystemCoreClock;
//BLDC对应PWM输出IO时钟
#define BLDC_GPIO_CLK       (RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB)
//BLDC对应PWM引脚
#define BLDC_UH_PORT        GPIOA
#define BLDC_UH_PIN         GPIO_Pin_8
#define BLDC_VH_PORT        GPIOA
#define BLDC_VH_PIN         GPIO_Pin_9
#define BLDC_WH_PORT        GPIOA
#define BLDC_WH_PIN         GPIO_Pin_10

#define BLDC_UL_PORT        GPIOA
#define BLDC_UL_PIN         GPIO_Pin_7
#define BLDC_VL_PORT        GPIOB
#define BLDC_VL_PIN         GPIO_Pin_0
#define BLDC_WL_PORT        GPIOB
#define BLDC_WL_PIN         GPIO_Pin_1
//BLDC刹车IO
#define BLDC_BKP_PORT       GPIOA
#define BLDC_BKP_PIN        GPIO_Pin_6

#define BLDC_UH_PIN_SRC     GPIO_PinSource8
#define BLDC_VH_PIN_SRC     GPIO_PinSource9
#define BLDC_WH_PIN_SRC     GPIO_PinSource10
#define BLDC_UL_PIN_SRC     GPIO_PinSource7
#define BLDC_VL_PIN_SRC     GPIO_PinSource0
#define BLDC_WL_PIN_SRC     GPIO_PinSource1
#define BLDC_BKP_PIN_SRC    GPIO_PinSource6
//BLDC对应PWM与BKP引脚复用选择
#define BLDC_UH_PIN_AF      GPIO_AF_2
#define BLDC_VH_PIN_AF      GPIO_AF_2
#define BLDC_WH_PIN_AF      GPIO_AF_2
#define BLDC_UL_PIN_AF      GPIO_AF_2
#define BLDC_VL_PIN_AF      GPIO_AF_2
#define BLDC_WL_PIN_AF      GPIO_AF_2
#define BLDC_BKP_PIN_AF     GPIO_AF_2
//BLDC使用TIM相关配置
#define BLDC_PWM_TIM                TIM1
#define BLDC_TIM_CLOCK_CMD          RCC_APB2PeriphClockCmd
#define BLDC_TIM_CLK                RCC_APB2Periph_TIM1
#define BLDC_TIM_PWM_FREQ           20000
#define BLDC_TIM_PRESCALER          0
#define BLDC_TIM_PERIOD             (uint16_t)(SystemCoreClock/(BLDC_TIM_PRESCALER+1)/BLDC_TIM_PWM_FREQ)

#define BLDC_TIM_REPETITIONCOUNTER  0
#define BLDC_TIM_DEADTIME           5

extern void bldc_pwm_init(void);

#endif
