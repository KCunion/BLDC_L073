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
__IO uint16_t     g_hwSpeedDuty = 15;       // 速度占空比：0~100  为100是占空比为100%
__IO motor_state_t  g_tMotorState = STOP;    // 电机状态
__IO motor_dir_t    g_tMotorDirection = CW;  // 电机方向
__IO uint16_t     g_hwTimeCount = 0;     	 // 卡住超时溢出计数

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
static uint8_t s_KeyCount = 1;
int main(void)
{ 
    systick_init();

    key_init();
    hall_init();
    bldc_pwm_init();
  
    /* 无限循环 */
    while (1) {    
        if (key1_scan() == KEY_DOWN) {    // 功能选择
            s_KeyCount ++;
            if (s_KeyCount > 5) {
                s_KeyCount = 1;
            }
        }
        if(key2_scan() == KEY_DOWN) {   // 功能执行
            switch (s_KeyCount) {
                case 1:         // 电机启动
                    if (STOP == g_tMotorState) {
                        g_hwTimeCount = 0;
                        g_tMotorState = RUN;            
                        hall_exti_callback();
                        delay(12);
                        NVIC_EnableIRQ(HALL_EXTI_IRQn);
                        hall_exti_callback();
                    }          
                    break;
                case 2:         // 加速
                    g_hwSpeedDuty += 5;
                    if (g_hwSpeedDuty > 100) {
                        g_hwSpeedDuty=100;
                    }
                    break;
                case 3:         // 减速
                    g_hwSpeedDuty -= 5;
                    if (g_hwSpeedDuty < 7) {
                        g_hwSpeedDuty = 7;
                    }
                    break;
                case 4:         // 方向反转
                    if (CW == g_tMotorDirection) {
                        g_tMotorDirection = CCW;
                    }
                    else {
                        g_tMotorDirection=CW;
                    }
                    break;
                case 5:         // 停机
                    g_tMotorState = STOP;
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
            TIM_SetCompare2(BLDC_PWM_TIM,BLDC_TIM_PERIOD * g_hwSpeedDuty/100);
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
            TIM_SetCompare2(BLDC_PWM_TIM,BLDC_TIM_PERIOD * g_hwSpeedDuty/100);
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
            TIM_SetCompare3(BLDC_PWM_TIM,BLDC_TIM_PERIOD * g_hwSpeedDuty/100);
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
            TIM_SetCompare3(BLDC_PWM_TIM,BLDC_TIM_PERIOD * g_hwSpeedDuty/100);
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Enable);

            break;
        case 2: //A+ B-
            /* Next step: Step 6 Configuration -------------------------------------- */
            TIM_CCxCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_PWM_TIM,TIM_Channel_3,TIM_CCxN_Disable);

            /*  Channel1 configuration */
            TIM_SetCompare1(BLDC_PWM_TIM,BLDC_TIM_PERIOD * g_hwSpeedDuty/100);
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
            TIM_SetCompare1(BLDC_PWM_TIM,BLDC_TIM_PERIOD * g_hwSpeedDuty/100);
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
    if (RUN == g_tMotorState) {
        g_hwTimeCount ++;
        if(g_hwTimeCount>2000) {   // 2s超时，电机卡住不运转超过2s时间
            g_tMotorState = STOP;
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
uint8_t chMotorState;
void hall_exti_callback(void)
{
    __IO uint8_t chStep = 0;
    uint16_t hwHallState = (HALL_PORT ->IDR) & 0xe000; // 读取霍尔传感器信息

    if (g_tMotorState==STOP) {
        return;
    }
    chStep = hwHallState >> 13;
    if (g_tMotorDirection == CW) {  // 方向判断
        chStep =  7 - chStep;
    }
    chMotorState = chStep;
    bldc_phase_chaneg(chStep);    //驱动换相
    g_hwTimeCount = 0;
}
