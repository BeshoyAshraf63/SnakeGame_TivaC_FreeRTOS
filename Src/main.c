#include "TM4C123.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* MCAL Includes */
#include "uart.h"

/* Game Tasks Include */
#include "gameTasks.h"

/* Game Include */
#include "game.h"

int main( void )
{
	/* Init UART */
	UART0_init(SystemCoreClock, 115200);

	/* Game Init */
	gameInit();
	
	/* Create Tasks */
	xTaskCreate( Task_GamePrint, "Game Print", 200, NULL, 1, NULL );

	xTaskCreate( Task_GameUpdate, "Game Update", 200, NULL, 2, NULL );

	xTaskCreate( Task_UpdateTime, "UpdateTime", 200, NULL, 3, NULL );
	
	xTaskCreate( UART0_HandlerTask, "UART ISR", 200, NULL, 4, NULL );

	/* Start Schedular */
	vTaskStartScheduler();
	
	for( ;; );
}

void vApplicationMallocFailedHook( void )
{
	for( ;; );
}


void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName)
{
	for( ;; );
}


void vApplicationIdleHook( void )
{
	
}
