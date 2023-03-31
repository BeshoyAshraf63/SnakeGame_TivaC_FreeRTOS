#ifndef _UART_
#define _UART_

void UART0_init(unsigned clk, unsigned baudrate);

void UART0_SendChar(char c);

void UART0_Print(const char *str);

int UART0_PrintDecimal(int n);

char UART0_GetReceivedChar(void);

#endif /* _UART_ */