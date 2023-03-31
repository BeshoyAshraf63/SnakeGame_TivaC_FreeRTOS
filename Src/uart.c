#include "uart.h"

#include "TM4C123.h"

void UART0_init(unsigned clk, unsigned baudrate)
{
    double uart_clk = clk / (16 * baudrate);

		SYSCTL->RCGCUART |= 0x01U;     
		SYSCTL->RCGCGPIO |= 0x01U;       
		
		while(!(SYSCTL->PRGPIO & 0x01));
		UART0->CTL &= ~0x01U;           

		UART0->IBRD = (unsigned)uart_clk;
		UART0->FBRD = (unsigned)((uart_clk - (unsigned)uart_clk) * 64 + 0.5) ;

		UART0->LCRH = 0x60;

		UART0->CC = 0x00;

		UART0->CTL = (1 << 0) | (1 << 8) | (1 << 9);
	
		GPIOA->DEN   |= 0x01U | 0x02U;
		GPIOA->AFSEL |= 0x01U | 0x02U;
		GPIOA->AMSEL &= (~0x01U) & (~0x02U);
		GPIOA->PCTL = (GPIOA->PCTL & (~0xFFU)) | (0x11U);
			
		UART0->ICR &= ~(0x0780);
		UART0->IM  = 0x0010;
		
		/* Set the priority */
		NVIC_SetPriority( 5, 5 );

		/* Enable the interrupt. */
		NVIC_EnableIRQ( 5 );
}

void UART0_SendChar(char c)
{
		while((UART0->FR & (1<<5)) != 0);   /* wait until fifo is not full, 0-> not full*/
		UART0->DR = c;
}

void UART0_Print(const char *str)
{
	while(*str)
	{
		
		if((*str) == '\n') UART0_SendChar('\r');
		UART0_SendChar(*str);
		
		str++;
	}
}

int UART0_PrintDecimal(int n)
{
	int count, temp, mask;
	long long nLong;
	count = 0;
	mask = 1;
	
	if(n == 0)
	{
		UART0_SendChar('0');
		return 1;
	}
	
	nLong = (long long)n;
	if(n < 0)
	{
		UART0_SendChar('-');
		nLong = -1 * (long long)n;
		count++;
	}
	
	temp = (long)nLong;
	while(temp)
	{
		temp /= 10;
		count++;
		if(temp != 0) mask *= 10;
	}
	
	while(mask)
	{
		UART0_SendChar((char)((nLong / mask) % 10) + '0');
		mask /= 10;
	}
	
	
	return count;
}

char UART0_GetReceivedChar(void)
{
	return (UART0->DR & 0xFF);
}
