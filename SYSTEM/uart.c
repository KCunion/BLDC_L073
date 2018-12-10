#include "uart.h" 


#define GPIO_PORT(n) ((GPIO_TypeDef *) (GPIOA_BASE + n*0x400))

#if (UARTPORT == 0X30)
#define DEBUG_UART UART2
#else
#define DEBUG_UART UART1
#endif

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif 

#ifdef USE_IAR
PUTCHAR_PROTOTYPE
{
	while((DEBUG_UART->CSR&UART_IT_TXIEN)==0);
	DEBUG_UART->TDR = (ch & (uint16_t)0x00FF);      
	return ch;
}
#else
#pragma import(__use_no_semihosting)             
            
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       

_sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((DEBUG_UART->CSR&UART_IT_TXIEN)==0);
	DEBUG_UART->TDR = (ch & (uint16_t)0x00FF);      
	return ch;
}
#endif
/**
* @brief  This function will initialize the UART.
* @param bound: Configuring UART baud rate.
* @param index: GPIO pin and multiplexing options
	bit 0-3: Port chose
	bit 4-7: Pin & Remap 
	According to the data UartTX & UartRX
* @retval : None
*/
u8 UartTX[] = {0x91, 0xa3, 0x60, 0x21};//PA9/PA10/PB8/PA2
u8 UartRX[] = {0xa1, 0x93, 0x70, 0x31};//PA10/PA9/PB7/PA3

void uart_initwBaudRate(u32 bound,u8 index)
{
	u8 *uartrx,*uarttx;
	u8 idx,port,rx_pin,rx_afio,tx_pin,tx_afio;
	GPIO_InitTypeDef GPIO_InitStructure;
	UART_InitTypeDef UART_InitStructure;
	uarttx = UartTX;
	uartrx = UartRX;
	idx = (index&0xf0) >> 4;
	port = index & 0x7;
	tx_afio = uarttx[idx]&0x7;
	rx_afio = uartrx[idx]&0x7;
	tx_pin = (uarttx[idx]&0xf0) >> 4;
	rx_pin = (uartrx[idx]&0xf0) >> 4;
	
	if(idx < 3)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
	else
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA << port, ENABLE);
	//UART_TXInit
	GPIO_InitStructure.GPIO_Pin = 1 << tx_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIO_PORT(port), &GPIO_InitStructure);
	//UART_RXInit
	GPIO_InitStructure.GPIO_Pin = 1 << rx_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_PORT(port), &GPIO_InitStructure); 

	GPIO_PinAFConfig(GPIO_PORT(port),tx_pin,tx_afio);
	GPIO_PinAFConfig(GPIO_PORT(port),rx_pin,rx_afio);

	UART_InitStructure.UART_BaudRate = bound;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
	UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;	

	UART_Init(DEBUG_UART, &UART_InitStructure);
	
	UART_Cmd(DEBUG_UART, ENABLE); 
}
/**
* @brief  This function will send a character.
* @param ch: This character will be sent.
* @retval : None
*/
void Uart_PutChar (char ch)
{
	while((DEBUG_UART->CSR&UART_IT_TXIEN)==0);
	DEBUG_UART->TDR = (ch & (uint16_t)0x00FF);    
}
/**
* @brief  This function will send a string.
* @param buff: The string of this address will be sent.
* @param len: The length of the string to be sent.
* @retval : None
*/
void Uart_PutBuff (uint8_t *buff, uint32_t len)
{
	while(len--)
	{
		Uart_PutChar(*buff);
		buff++;
	}
}
/**
* @brief  VCAN Uart serial port assistant interface function.
* @param wareaddr: The array data address to be sent.
* @param waresize: The size of the array data to be sent.
* @retval : None
*/
void Send_Ware(void *wareaddr, uint32_t waresize)
{
#define CMD_WARE     3
	uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};
	uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};

	Uart_PutBuff(cmdf, sizeof(cmdf));
	Uart_PutBuff((uint8_t *)wareaddr, waresize);
	Uart_PutBuff(cmdr, sizeof(cmdr));
}

