#ifndef _TASKS_
#define _TASKS_

void UART0_HandlerTask( void *pvParameters );

void Task_GamePrint( void *pvParameters );

void Task_GameUpdate( void *pvParameters );

void Task_UpdateTime( void *pvParameters );

void UART0_Handler( void );

#endif /* _TASKS_ */