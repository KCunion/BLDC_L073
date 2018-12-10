#include "sys.h"

/* 私有类型定义 --------------------------------------------------------------*/
typedef enum
{
  CW = 0,  // 顺时钟方向
  CCW = 1  // 逆时针方向
}motor_dir_t;

typedef enum 
{
  STOP = 0,  // 停机
  RUN = 1    // 运行
}motor_state_t;

/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
__IO uint16_t     hwSpeedDuty = 15;       // 速度占空比：0~100  为100是占空比为100%
__IO motor_state_t  tMotorState = STOP;    // 电机状态
__IO motor_dir_t    tMotorDirection = CW;  // 电机方向
__IO uint16_t     hwTimeCount = 0;     	 // 卡住超时溢出计数

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
extern void bldc_phase_chaneg(uint8_t step);
extern void hall_exti_callback(void);
extern void systick_callback(void);
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static uint8_t key_count = 1;
int main(void)
{ 
    systick_init();

    key_init();
    hall_init();
    bldc_pwm_init();
  
    /* 无限循环 */
    while (1) {    
        if (key1_scan() == KEY_DOWN) {    // 功能选择
            key_count ++;
            if (key_count > 5) {
                key_count = 1;
            }
        }
        if(key2_scan() == KEY_DOWN) {   // 功能执行
            switch (key_count) {
                case 1:         // 电机启动
                    if (STOP == tMotorState) {
                        hwTimeCount = 0;
                        tMotorState = RUN;            
                        hall_exti_callback();
//                        HAL_Delay(12);
                        NVIC_EnableIRQ(HALL_EXTI_IRQn);
                        hall_exti_callback();
                    }          
                    break;
                case 2:         // 加速
                    hwSpeedDuty += 5;
                    if (hwSpeedDuty > 100) {
                        hwSpeedDuty=100;
                    }
                    break;
                case 3:         // 减速
                    hwSpeedDuty -= 5;
                    if (hwSpeedDuty < 7) {
                        hwSpeedDuty = 7;
                    }
                    break;
                case 4:         // 方向反转
                    if (CW == tMotorDirection) {
                        tMotorDirection = CCW;
                    }
                    else {
                        tMotorDirection=CW;
                    }
                    break;
                case 5:         // 停机
                    tMotorState = STOP;
                    EXTI_ClearITPendingBit(HALL_U_EXITLINE|HALL_V_EXITLINE|HALL_W_EXITLINE);
                    NVIC_DisableIRQ(HALL_EXTI_IRQn);
                    TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCx_Disable);
                    TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCxN_Disable);
                    TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCx_Disable);
                    TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCxN_Disable);
                    TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Disable);
                    TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCxN_Disable);
                    break;
            }
        }
    }
}

/**
  * 函数功能: 无刷驱动换相
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
//static uint8_t s_chHallStates;
void bldc_phase_chaneg(uint8_t step)
{
    switch (step) {
        case 4: //B+ C-
            /* Next step: Step 2 Configuration -------------------------------------- */ 
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCxN_Disable);

            /*  Channel1 configuration */
            /*  Channel2 configuration */    
            TIM_SetCompare2(BLDC_PWM_TIM,BLDC_TIM_PERIOD * hwSpeedDuty/100);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCx_Enable);
            /*  Channel3 configuration */
            TIM_SetCompare3(BLDC_PWM_TIM,BLDC_TIM_PERIOD);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCxN_Enable);
            break;
        case 5: //B+ A-
            /* Next step: Step 3 Configuration -------------------------------------- */      
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCxN_Disable);

            /*  Channel1 configuration */
            TIM_SetCompare1(BLDC_PWM_TIM,BLDC_TIM_PERIOD);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCxN_Enable);

            /*  Channel2 configuration */
            TIM_SetCompare2(BLDC_PWM_TIM,BLDC_TIM_PERIOD * hwSpeedDuty/100);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCx_Enable);
            /*  Channel3 configuration */
            break;
        case 1: //C+ A-
            /* Next step: Step 4 Configuration -------------------------------------- */
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCxN_Disable);

            /*  Channel1 configuration */
            TIM_SetCompare1(BLDC_PWM_TIM,BLDC_TIM_PERIOD);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCxN_Enable);

            /*  Channel2 configuration */ 
            /*  Channel3 configuration */
            TIM_SetCompare3(BLDC_PWM_TIM,BLDC_TIM_PERIOD * hwSpeedDuty/100);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Enable);

            break;
        case 3: //C+ B-
            /* Next step: Step 5 Configuration -------------------------------------- */
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCxN_Disable);    

            /*  Channel1 configuration */      
            /*  Channel2 configuration */   
            TIM_SetCompare2(BLDC_PWM_TIM,BLDC_TIM_PERIOD);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCxN_Enable);

            /*  Channel3 configuration */      
            TIM_SetCompare3(BLDC_PWM_TIM,BLDC_TIM_PERIOD * hwSpeedDuty/100);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Enable);

            break;
        case 2: //A+ B-
            /* Next step: Step 6 Configuration -------------------------------------- */
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCxN_Disable);

            /*  Channel1 configuration */
            TIM_SetCompare1(BLDC_PWM_TIM,BLDC_TIM_PERIOD * hwSpeedDuty/100);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCx_Enable);
            /*  Channel2 configuration */
            TIM_SetCompare2(BLDC_PWM_TIM,BLDC_TIM_PERIOD);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCxN_Enable);
            /*  Channel3 configuration */

            break;
        case 6: //A+ C-
            /* Next step: Step 1 Configuration -------------------------------------- */
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCxN_Disable);

            /*  Channel1 configuration */
            TIM_SetCompare1(BLDC_PWM_TIM,BLDC_TIM_PERIOD * hwSpeedDuty/100);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCx_Enable);
            /*  Channel2 configuration */      
            /*  Channel3 configuration */
            TIM_SetCompare3(BLDC_PWM_TIM,BLDC_TIM_PERIOD);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCxN_Enable);

            break;
        default:
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCxN_Disable);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCxN_Disable);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCxN_Disable);
            break;
    }
}

void systick_callback(void)
{
    if (RUN == tMotorState) {
        hwTimeCount ++;
        if(hwTimeCount>2000) {   // 2s超时，电机卡住不运转超过2s时间
            tMotorState = STOP;
            EXTI_ClearITPendingBit(HALL_U_EXITLINE | HALL_V_EXITLINE | HALL_W_EXITLINE);
            NVIC_DisableIRQ(HALL_EXTI_IRQn);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_1,TIM_CCxN_Disable);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_2,TIM_CCxN_Disable);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCxN_Disable);
        }
    }
}

void hall_exti_callback(void)
{
  __IO uint8_t chStep = 0;
  uint16_t hwHallState = (HALL_PORT ->IDR) & 0x01c0; // 读取霍尔传感器信息
  
  if (tMotorState==STOP) {
      return;
  }
  chStep = hwHallState >> 6;
  if (tMotorDirection == CW) {  // 方向判断
      chStep =  7 - chStep;
  }
  bldc_phase_chaneg(chStep);    //驱动换相
  hwTimeCount = 0;
}
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
