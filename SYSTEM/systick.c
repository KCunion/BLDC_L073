#include "systick.h"

extern void systick_callback(void);
extern u32 SystemCoreClock;

static __IO uint32_t s_wSysticks;

/**
* @brief  This function will be used to initialize SysTick.
* @param : None
* @retval : None
*/
void systick_init()
{
    /* SystemFrequency / 1000    1ms中断一次
    * SystemFrequency / 100000	 10us中断一次
    * SystemFrequency / 1000000 1us中断一次
    */
    SysTick_Config(SystemCoreClock / 1000);

    NVIC_SetPriority(SysTick_IRQn, 0);
}
void inc_systicks(void)
{
    s_wSysticks++;
}
uint32_t get_systicks(void)
{
    return s_wSysticks;
}
void delay(__IO uint32_t Delay)
{
    uint32_t tickstart = 0;
    tickstart = get_systicks();
    while ((get_systicks() - tickstart) < Delay) {
    }
}
void suspend_systicks(void)
{
    CLEAR_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
}
void resume_systicks(void)
{
    SET_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
}

void SysTick_Handler(void)
{
    inc_systicks();
    systick_callback();
}
