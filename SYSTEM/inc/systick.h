#ifndef __SYSTICK_H
#define __SYSTICK_H 			   
#include "sys.h"
	 
extern void systick_init(void);
extern void inc_systicks(void);
extern uint32_t get_systicks(void);
extern void delay(__IO uint32_t Delay);
extern void suspend_systicks(void);
extern void resume_systicks(void);

#endif





























