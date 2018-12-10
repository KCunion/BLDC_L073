#include "hall.h"

extern void hall_exti_callback(void);
void hall_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_AHBPeriphClockCmd(HALL_RCC_CLOCKGPIO,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = HALL_U_PIN | HALL_V_PIN | HALL_W_PIN;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
    GPIO_Init(HALL_PORT, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(HALL_GPIO_PORTSOURCE,HALL_U_PINSOURCE);
    SYSCFG_EXTILineConfig(HALL_GPIO_PORTSOURCE,HALL_V_PINSOURCE);
    SYSCFG_EXTILineConfig(HALL_GPIO_PORTSOURCE,HALL_W_PINSOURCE);

    EXTI_InitStructure.EXTI_Line = HALL_U_EXITLINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_InitStructure.EXTI_Line = HALL_V_EXITLINE;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_InitStructure.EXTI_Line = HALL_W_EXITLINE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = HALL_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x02; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
    NVIC_Init(&NVIC_InitStructure); 
}

void HALL_EXTI_IRQHandler(void)
{
    /* 确保是否产生了EXTI Line中断 */
	if (EXTI_GetITStatus(HALL_U_EXITLINE | HALL_V_EXITLINE | HALL_W_EXITLINE) != RESET) {
        /* 清除中断标志位	*/
        EXTI_ClearITPendingBit(HALL_U_EXITLINE|HALL_V_EXITLINE|HALL_W_EXITLINE);   
        hall_exti_callback();
    }  
}

