#include "key.h"

void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(BKEY_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = KEY1_PIN;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  
    GPIO_Init(KEY1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = KEY2_PIN;  
    GPIO_Init(KEY2_PORT, &GPIO_InitStructure);
} 


void key_delay(void)
{  
    uint32_t i,j;
    for (i=0;i<100;++i) {
        for (j=0;j<1000;++j){
        }	
    }	
}

key_state_t key1_scan(void)
{
  if (GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN)==KEY1_DOWN_LEVEL) {
    key_delay();
    if (GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN)==KEY1_DOWN_LEVEL) {
        while(GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN)==KEY1_DOWN_LEVEL);
        return KEY_DOWN;
        }
    }
    return KEY_UP;
}

key_state_t key2_scan(void)
{
    if (GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN)==KEY2_DOWN_LEVEL) {
        key_delay();
        if(GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN)==KEY2_DOWN_LEVEL) {
            while(GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN)==KEY2_DOWN_LEVEL);
            return KEY_DOWN;
        }
    }
    return KEY_UP;
}
