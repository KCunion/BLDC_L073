#include "pwm.h"   

void bldc_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //使能BLDC对应PWM输出IO时钟
    RCC_AHBPeriphClockCmd(BLDC_GPIO_CLK, ENABLE);
    //配置BLDC U相上桥对应IO为复用输出功能
    GPIO_InitStructure.GPIO_Pin = BLDC_UH_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(BLDC_UH_PORT, &GPIO_InitStructure);
    //配置BLDC V相上桥对应IO为复用输出功能
    GPIO_InitStructure.GPIO_Pin = BLDC_VH_PIN;
    GPIO_Init(BLDC_VH_PORT, &GPIO_InitStructure);
    //配置BLDC W相上桥对应IO为复用输出功能
    GPIO_InitStructure.GPIO_Pin = BLDC_WH_PIN;
    GPIO_Init(BLDC_WH_PORT, &GPIO_InitStructure);
    //配置BLDC U相下桥对应IO为复用输出功能
    GPIO_InitStructure.GPIO_Pin = BLDC_UL_PIN;
    GPIO_Init(BLDC_UL_PORT, &GPIO_InitStructure);
    //配置BLDC V相下桥对应IO为复用输出功能
    GPIO_InitStructure.GPIO_Pin = BLDC_VL_PIN;
    GPIO_Init(BLDC_VL_PORT, &GPIO_InitStructure);
    //配置BLDC W相下桥对应IO为复用输出功能
    GPIO_InitStructure.GPIO_Pin = BLDC_WL_PIN;
    GPIO_Init(BLDC_WL_PORT, &GPIO_InitStructure);
    //配置BLDC刹车输入对应IO为上拉输入功能
    GPIO_InitStructure.GPIO_Pin = BLDC_BKP_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
    GPIO_Init(BLDC_BKP_PORT, &GPIO_InitStructure);
    //配置BLDC对应IO复用为TIM的PWM功能
    GPIO_PinAFConfig(BLDC_UH_PORT, BLDC_UH_PIN_SRC, BLDC_UH_PIN_AF);
    GPIO_PinAFConfig(BLDC_VH_PORT, BLDC_VH_PIN_SRC, BLDC_VH_PIN_AF);
    GPIO_PinAFConfig(BLDC_WH_PORT, BLDC_WH_PIN_SRC, BLDC_WH_PIN_AF);
    GPIO_PinAFConfig(BLDC_UL_PORT, BLDC_UL_PIN_SRC, BLDC_UL_PIN_AF);
    GPIO_PinAFConfig(BLDC_VL_PORT, BLDC_VL_PIN_SRC, BLDC_VL_PIN_AF);
    GPIO_PinAFConfig(BLDC_WL_PORT, BLDC_WL_PIN_SRC, BLDC_WL_PIN_AF);
    //配置BLDC刹车输入对应IO复用为TIM_BKP功能
    GPIO_PinAFConfig(BLDC_BKP_PORT, BLDC_BKP_PIN_SRC,BLDC_BKP_PIN_AF);
}

void bldc_pwm_init(void)
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
    //配置BLDC对应PWM对应输出GPIO
    bldc_gpio_config();
    //使能BLDC_PWM_TIM时钟
    BLDC_TIM_CLOCK_CMD(BLDC_TIM_CLK, ENABLE);
    //设置在下一个更新事件装入活动的自动重装载寄存器周期的值 Period
    //设置用来作为TIMx时钟频率除数的预分频值  Prescaler
    //设置时钟分割:TDTS = TIM_CKD_DIV1
    //TIM向上计数模式
    TIM_TimeBaseStructure.TIM_Period        = BLDC_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler     = BLDC_TIM_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = BLDC_TIM_REPETITIONCOUNTER;
    TIM_TimeBaseInit(BLDC_PWM_TIM, &TIM_TimeBaseStructure);

    //运行模式下开启状态选择
    //空闲模式下开启状态选择
    //软件错误锁定配置:锁定关闭无保护
    //DTG[7:0]死区发生器配置:(死区时间DT)
    TIM_BDTRInitStruct.TIM_OSSRState    = TIM_OSSRState_Enable;		
    TIM_BDTRInitStruct.TIM_OSSIState    = TIM_OSSIState_Enable;		
    TIM_BDTRInitStruct.TIM_LOCKLevel    = TIM_LOCKLevel_OFF;		
    TIM_BDTRInitStruct.TIM_DeadTime     = BLDC_TIM_DEADTIME;						
    //TDTS = 125nS(8MHz)
    //DTG[7: 5] = 0xx => DT = DTG[7: 0] * Tdtg, Tdtg = TDTS;
    //DTG[7: 5] = 10x => DT =(64+DTG[5: 0]) * Tdtg, Tdtg = 2 * TDTS;
    //DTG[7: 5] = 110 => DT =(32+DTG[4: 0]) * Tdtg, Tdtg = 8 * TDTS; 
    //DTG[7: 5] = 111=> DT =(32 + DTG[4: 0]) *  Tdtg, Tdtg = 16 * TDTS;

    //刹车配置：使能刹车
    //刹车输入极性选择:低电平有效
    //自动输出使能配置:MOE只能软件置1
    TIM_BDTRInitStruct.TIM_Break            = TIM_Break_Disable;  			
    TIM_BDTRInitStruct.TIM_BreakPolarity    = TIM_BreakPolarity_Low;		
    TIM_BDTRInitStruct.TIM_AutomaticOutput  = TIM_AutomaticOutput_Enable;	
    TIM_BDTRConfig( BLDC_PWM_TIM, &TIM_BDTRInitStruct);
    //模式配置：PWM模式1
    //输出状态设置：使能输出
    //互补通道输出状态设置：使能输出
    //设置待装入捕获比较寄存器的脉冲值
    //输出比较极性高
    //N输出比较极性高
    TIM_OCInitStructure.TIM_OCMode 		    = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState    = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = 0;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;      
    TIM_OCInitStructure.TIM_OCNPolarity     = TIM_OCNPolarity_High; 
    TIM_OCInitStructure.TIM_OCIdleState     = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState    = TIM_OCNIdleState_Set;
    TIM_OC1Init(BLDC_PWM_TIM, &TIM_OCInitStructure);
    TIM_OC2Init(BLDC_PWM_TIM, &TIM_OCInitStructure);
    TIM_OC3Init(BLDC_PWM_TIM, &TIM_OCInitStructure);
    //使能CH1、2、3预装载
    TIM_OC1PreloadConfig(BLDC_PWM_TIM, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(BLDC_PWM_TIM, TIM_OCPreload_Enable);  	
    TIM_OC3PreloadConfig(BLDC_PWM_TIM, TIM_OCPreload_Enable);
    //使能TIMx在ARR上的预装载寄存器
    TIM_ARRPreloadConfig(BLDC_PWM_TIM, ENABLE); 
    //使能BLDC_PWM_TIM
    TIM_Cmd(BLDC_PWM_TIM, ENABLE);  			
    //主输出使能 使能MOE位
    TIM_CtrlPWMOutputs(BLDC_PWM_TIM, ENABLE);
    //关闭CH1、2、3通道输出
    TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCx_Disable);
    TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCxN_Disable);
    TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCx_Disable);
    TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCxN_Disable);
    TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Disable);
    TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCxN_Disable);
}
