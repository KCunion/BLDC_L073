#include "sys.h"

void System_Clock_Init(u8 PLL)
{
    unsigned char temp=0;   
    RCC ->CR |= RCC_CR_HSEON;  
    while(!(RCC ->CR&RCC_CR_HSERDY));
    RCC ->CFGR = RCC_CFGR_PPRE1_2;					//APB1=DIV2;APB2=DIV1;AHB=DIV1;
    
    RCC ->CFGR |= RCC_CFGR_PLLSRC;					//PLLSRC ON 
    RCC ->CR &= ~(RCC_CR_PLLON);					//Clear PLL   //RCC->CR &=~(7<<20);	
    
    RCC ->CR &= ~((uint32_t)0x3f << 26);	
    RCC ->CR |= (PLL - 1) << 26;
    
    FLASH ->ACR |= FLASH_ACR_LATENCY_1 | FLASH_ACR_PRFTBE | FLASH_ACR_PRFTBS;	  //FLASH delay 2 SysTick
    
    RCC ->CR |= RCC_CR_PLLON;						//PLLON
    while(!(RCC ->CR & RCC_CR_PLLRDY));			//wait PLL lock
    RCC ->CFGR |= RCC_CFGR_SW_PLL;					//set PLL as system clock	 
    while(temp != 0x02) 
{    
        temp = RCC ->CFGR >> 2;
        temp &= 0x03;
    }    
}	



