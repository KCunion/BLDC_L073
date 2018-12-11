#include "sys.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef enum
{
  CW = 0,  // ˳ʱ�ӷ���
  CCW = 1  // ��ʱ�뷽��
}motor_dir_t;

typedef enum 
{
  STOP = 0,  // ͣ��
  RUN = 1    // ����
}motor_state_t;

/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
__IO uint16_t     hwSpeedDuty = 15;       // �ٶ�ռ�ձȣ�0~100  Ϊ100��ռ�ձ�Ϊ100%
__IO motor_state_t  tMotorState = STOP;    // ���״̬
__IO motor_dir_t    tMotorDirection = CW;  // �������
__IO uint16_t     hwTimeCount = 0;     	 // ��ס��ʱ�������

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
extern void bldc_phase_chaneg(uint8_t step);
extern void hall_exti_callback(void);
extern void systick_callback(void);
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static uint8_t key_count = 1;
int main(void)
{ 
    systick_init();

    key_init();
    hall_init();
    bldc_pwm_init();
  
    /* ����ѭ�� */
    while (1) {    
        if (key1_scan() == KEY_DOWN) {    // ����ѡ��
            key_count ++;
            if (key_count > 5) {
                key_count = 1;
            }
        }
        if(key2_scan() == KEY_DOWN) {   // ����ִ��
            switch (key_count) {
                case 1:         // �������
                    if (STOP == tMotorState) {
                        hwTimeCount = 0;
                        tMotorState = RUN;            
                        hall_exti_callback();
                        delay(12);
                        NVIC_EnableIRQ(HALL_EXTI_IRQn);
                        hall_exti_callback();
                    }          
                    break;
                case 2:         // ����
                    hwSpeedDuty += 5;
                    if (hwSpeedDuty > 100) {
                        hwSpeedDuty=100;
                    }
                    break;
                case 3:         // ����
                    hwSpeedDuty -= 5;
                    if (hwSpeedDuty < 7) {
                        hwSpeedDuty = 7;
                    }
                    break;
                case 4:         // ����ת
                    if (CW == tMotorDirection) {
                        tMotorDirection = CCW;
                    }
                    else {
                        tMotorDirection=CW;
                    }
                    break;
                case 5:         // ͣ��
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
  * ��������: ��ˢ��������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
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
        if(hwTimeCount>2000) {   // 2s��ʱ�������ס����ת����2sʱ��
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
uint8_t chMotorState;
void hall_exti_callback(void)
{
  __IO uint8_t chStep = 0;
  uint16_t hwHallState = (HALL_PORT ->IDR) & 0xe000; // ��ȡ������������Ϣ
  
  if (tMotorState==STOP) {
      return;
  }
  chStep = hwHallState >> 13;
  if (tMotorDirection == CW) {  // �����ж�
      chStep =  7 - chStep;
  }
  chMotorState = chStep;
  bldc_phase_chaneg(chStep);    //��������
  hwTimeCount = 0;
}
/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
