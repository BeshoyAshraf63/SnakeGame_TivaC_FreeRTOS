#include "gameTasks.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* MCAL Includes */
#include "uart.h"

/* Game Include */
#include "game.h"

/* Global Variables */
extern xSemaphoreHandle UART0_BinarySemaphore;
extern xSemaphoreHandle shouldPrint_BinarySemaphore;
extern xSemaphoreHandle shouldUpdate_BinarySemaphore;
extern xSemaphoreHandle gameData_Mutex;

extern uint32_t level, timeElapsed;
extern char currentCmd;
extern uint8_t isPlaying;
extern uint8_t win, score;

void UART0_HandlerTask( void *pvParameters )
{
	xSemaphoreTake( UART0_BinarySemaphore, 0 );
	for( ;; )
	{
		xSemaphoreTake( UART0_BinarySemaphore, portMAX_DELAY );
		xSemaphoreTake( gameData_Mutex, portMAX_DELAY );
		{
			currentCmd = UART0_GetReceivedChar();
			if(currentCmd == BEGIN_CMD && !isPlaying)
			{
				isPlaying = 1;
				if(!win) score = 0;
				beginGame();
				xSemaphoreGive(shouldPrint_BinarySemaphore);
				xSemaphoreGive( shouldUpdate_BinarySemaphore );
			}
		}
		xSemaphoreGive( gameData_Mutex );
	}
}


void Task_GamePrint( void *pvParameters )
{
	portTickType lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	xSemaphoreTake( shouldPrint_BinarySemaphore, 0 );
	
	/* hide cursor */
	UART0_Print(TERMINAL_HIDE_CURSOR);
	for( ;; )
	{
		gamePrint();
		vTaskDelayUntil( &lastWakeTime, ( 10 / portTICK_RATE_MS ) );
	}	
}

void Task_GameUpdate( void *pvParameters )
{
	portTickType lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	xSemaphoreTake( shouldUpdate_BinarySemaphore, 0 );
	
	for( ;; )
	{
		uint32_t delayTime;
		gameUpdate();
		if(isPlaying)
			delayTime = (level > 5) ? (50 / portTICK_RATE_MS)  : (((6 - level) * 50) / portTICK_RATE_MS) ;
		else
			delayTime = portMAX_DELAY;
		xSemaphoreTake( shouldUpdate_BinarySemaphore, delayTime );
	}
}

void Task_UpdateTime( void *pvParameters )
{
	portTickType lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	for( ;; )
	{
		if(isPlaying)
			timeElapsed ++;
		else
			timeElapsed = 0;
		vTaskDelayUntil( &lastWakeTime, ( 1000 / portTICK_RATE_MS ) );
	}
}

void UART0_Handler( void )
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( UART0_BinarySemaphore, &xHigherPriorityTaskWoken );
	UART0->ICR |= (0x010);
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}