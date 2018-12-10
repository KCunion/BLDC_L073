#ifndef __UART_H
#define __UART_H
#include "HAL_conf.h"
#include  "stdio.h"
#include "dtype.h"
		   
#define UART_BAUDRATE	115200
#define UARTPORT		0X00	//RX:PA10 TX:PA9
//#define UARTPORT		0X10	//RX:PA9 TX:PA10
//#define UARTPORT		0X21	//RX:PB7 TX:PB6
//#define UARTPORT		0X30	//RX:PA3 TX:PA2

void uart_initwBaudRate(u32 bound,u8 index);
void Uart_PutChar (char ch);
void Uart_PutBuff (uint8_t *buff, uint32_t len);
void Send_Ware(void *wareaddr, uint32_t waresize);
#endif


