#include "game.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* MCAL Includes */
#include "uart.h"

/* For using rand() */
#include "stdlib.h"

/* Global variables */
int32_t board[GAME_HEIGHT][GAME_WIDTH];
int32_t headX, headY;
int32_t headIndex, tailIndex;
uint8_t isPlaying;
uint8_t win;
uint8_t initialGame;
uint32_t score, level, timeElapsed;
char currentCmd, prevCmd;
uint8_t powerUp;

/* Binary Semaphores */
xSemaphoreHandle UART0_BinarySemaphore;
xSemaphoreHandle shouldPrint_BinarySemaphore;
xSemaphoreHandle shouldUpdate_BinarySemaphore;

/* Mutexes */
xSemaphoreHandle gameData_Mutex;

/* Absolute difference */
static uint8_t absolute(uint8_t x, uint8_t y);

uint8_t absolute(uint8_t x, uint8_t y)
{
	if(x > y) return x - y;
	else return y - x;
}

void gameInit(void)
{
	/* Init Binary Smaphores */
	vSemaphoreCreateBinary( UART0_BinarySemaphore );
	vSemaphoreCreateBinary( shouldPrint_BinarySemaphore );
	vSemaphoreCreateBinary( shouldUpdate_BinarySemaphore );
	
	/* Init Mutexes */
	gameData_Mutex = xSemaphoreCreateMutex();
	
	if(UART0_BinarySemaphore == NULL || shouldPrint_BinarySemaphore == NULL || shouldUpdate_BinarySemaphore == NULL || gameData_Mutex == NULL)
	{
		for(;;);
	}
	
	beginGame();
	initialGame = 1;
	score = 0;
	isPlaying = 0;
	level = 1;
}

void gamePrint(void)
{
	int i, j;
	uint8_t isPlayingLocal;
	uint32_t scoreLocal, timeLocal, levelLocal;
	
	xSemaphoreTake( gameData_Mutex, portMAX_DELAY );
	{
		isPlayingLocal = isPlaying;
		scoreLocal = score;
		timeLocal = timeElapsed;
		levelLocal = level;
	}
	xSemaphoreGive( gameData_Mutex );
	
	if(!isPlayingLocal)
	{
		/* Reset Screen */
		UART0_Print(TERMINAL_RESET);
		
		/* Move Cursor to Home */
		UART0_Print(TERMINAL_MOVE_TO_HOME);
		
		UART0_Print("***************************************************\n");
		UART0_Print("*                                                 *\n");
		UART0_Print("*              Welcome to Snake Game              *\n");
		UART0_Print("*                                                 *\n");
		UART0_Print("***************************************************\n");
		UART0_Print("\nInstructions:\n");
		UART0_Print("-------------\n");
		UART0_Print("\nTo move UP    Press: UP arrow\n");
		UART0_Print("\nTo move DOWN  Press: DOWN arrow\n");
		UART0_Print("\nTo move LEFT  Press: LEFT arrow\n");
		UART0_Print("\nTo move RIGHT Press: RIGHT arrow\n");
		UART0_Print("\nTo Start      Press: ENTER\n");
		
		if(!initialGame)
		{
			if(win)
			{
				UART0_Print("\n******************** YOU WON! *********************\n\n");
				UART0_Print("Up To Level:   ");
				UART0_PrintDecimal(level);
				UART0_Print("\n\nElapsed Time:  ");
				UART0_PrintDecimal(timeElapsed);
				UART0_Print(" s\n\nScore:         ");
				UART0_PrintDecimal(score);
				UART0_Print("\n");
			}
			else
			{
				UART0_Print("\n*************** GAME OVER YOU LOST! ***************\n\n");
				UART0_Print("Press ENTER to restart\n");
				UART0_Print("\n\nElapsed Time:  ");
				UART0_PrintDecimal(timeElapsed);
				UART0_Print(" s\n\nScore:         ");
				UART0_PrintDecimal(score);
				UART0_Print("\n");
			}
		}
		
		xSemaphoreTake( shouldPrint_BinarySemaphore, portMAX_DELAY );
		
		/* Reset Screen */
		UART0_Print(TERMINAL_RESET);
		return;
	}
	
	/* Move Cursor to Home */
	UART0_Print(TERMINAL_MOVE_TO_HOME);
	
	/* Top boundary */
	for(i = 0; i <= GAME_WIDTH + 1; i++)
	{
		if(i == 0) UART0_SendChar(TOP_LEFT_CHAR_ASCII);
		else if(i == GAME_WIDTH + 1) UART0_SendChar(TOP_RIGHT_CHAR_ASCII);
		else UART0_SendChar(HORIZONTAL_CHAR_ASCII);
	}
	UART0_Print("\n");
	
	/* Middle Area */
	for(i = 0; i < GAME_HEIGHT; i++)
	{
		UART0_SendChar(VERTICAL_CHAR_ASCII);
		for(j = 0; j < GAME_WIDTH + 1; j++)
		{
			if(j == GAME_WIDTH) 
			{
				UART0_SendChar(VERTICAL_CHAR_ASCII);
			}
			else
			{
				int32_t boardIJ, headIndexLocal;
				xSemaphoreTake( gameData_Mutex, portMAX_DELAY );
				{
					boardIJ = board[i][j];
					headIndexLocal = headIndex;
				}
				xSemaphoreGive( gameData_Mutex );
				
				if(boardIJ == 0) UART0_SendChar(' ');
				else if(boardIJ > 0 && boardIJ != headIndexLocal) UART0_SendChar(SNAKE_BODY_CHAR_ASCII);
				else if(boardIJ == headIndexLocal) UART0_SendChar(SNAKE_HEAD_CHAR_ASCII);
				else if(boardIJ == BOARD_POWERUP) UART0_SendChar(POWERUP_CHAR_ASCII);
				else if(boardIJ == BOARD_LARGE_POWERUP) UART0_SendChar(LARGE_POWERUP_CHAR_ASCII);
			}
		}
		if(i == 0)
		{
			UART0_Print("   Level:          ");
			UART0_PrintDecimal(levelLocal);
		}
		else if(i == 2)
		{
			UART0_Print("   Score:          ");
			UART0_PrintDecimal(scoreLocal);
		}
		else if(i == 4)
		{
			UART0_Print("   Elapsed Time:   ");
			UART0_PrintDecimal(timeLocal);
			UART0_Print(" s");
		}
		UART0_Print("\n");
	}
	
	/* Bottom boundary */
	for(i = 0; i <= GAME_WIDTH + 1; i++)
	{
		if(i == 0) UART0_SendChar(BOTTOM_LEFT_CHAR_ASCII);
		else if(i == GAME_WIDTH + 1) UART0_SendChar(BOTTOM_RIGHT_CHAR_ASCII);
		else UART0_SendChar(HORIZONTAL_CHAR_ASCII);
	}
	UART0_Print("\n");
}

void gameUpdate()
{
	uint8_t i, j, isPlayingLocal;
	
	xSemaphoreTake( gameData_Mutex, portMAX_DELAY );
	{
		isPlayingLocal = isPlaying;
	}
	xSemaphoreGive( gameData_Mutex );
	
	if(!isPlayingLocal)
	{
		xSemaphoreTake( shouldUpdate_BinarySemaphore, portMAX_DELAY );
		return;
	}

	if(isPlaying)
	{
		if(!powerUp)
		{
			uint8_t x, y, largePowerupProp;
			portTickType ticks;
			ticks = xTaskGetTickCount();
			srand(ticks);
			x = rand() % GAME_HEIGHT;
			y = rand() % GAME_WIDTH;
			largePowerupProp = rand() % 10;
			
			while(board[x][y] != 0)
			{
				x = rand() % GAME_HEIGHT;
				y = rand() % GAME_WIDTH;
			}
			powerUp = 1;
			if(largePowerupProp < 2) board[x][y] = BOARD_LARGE_POWERUP;
			else board[x][y] = BOARD_POWERUP;
		}
		xSemaphoreTake( gameData_Mutex, portMAX_DELAY );
		{
			if((currentCmd == NULL_CMD) ||
				(currentCmd != MOVE_UP && currentCmd != MOVE_DOWN && currentCmd != MOVE_LEFT && currentCmd != MOVE_RIGHT) ||
				(absolute(currentCmd, prevCmd) <= 5))
			{
				currentCmd = prevCmd;
			}
			if(currentCmd == MOVE_RIGHT)
			{
				if(headX == GAME_WIDTH - 1) loseGame();
				else headX ++;
			}
			else if(currentCmd == MOVE_LEFT)
			{
				if(headX == 0) loseGame();
				else headX --;
			}
			else if(currentCmd == MOVE_UP)
			{
				if(headY == 0) loseGame();
				else headY --;
			}
			else if(currentCmd == MOVE_DOWN)
			{
				if(headY == GAME_HEIGHT - 1) loseGame();
				else headY ++;
			}
			
			if(board[headY][headX] != BOARD_EMPTY && board[headY][headX] != BOARD_POWERUP && board[headY][headX] != BOARD_LARGE_POWERUP) loseGame();
			if(board[headY][headX] == BOARD_POWERUP)
			{
				powerUp = 0;
				score += 5;
				tailIndex -= 1;
			}
			if(board[headY][headX] == BOARD_LARGE_POWERUP)
			{
				powerUp = 0;
				score += 15;
				tailIndex -= 3;
			}
			headIndex ++;
			board[headY][headX] = headIndex;
			
			for(i = 0; i < GAME_HEIGHT; i++){
				for(j = 0; j < GAME_WIDTH; j++){
					if(board[i][j] == tailIndex)
					{
						board[i][j] = 0;
					}
				}
			}
			tailIndex++;
			prevCmd = currentCmd;
			currentCmd = NULL_CMD;
			
			if(headIndex - tailIndex >= WIN_LENGTH) winGame();
		}
		xSemaphoreGive( gameData_Mutex );
	}
}

void winGame(void)
{
	win = 1;
	isPlaying = 0;
	level ++;
}

void loseGame(void)
{
	win = 0;
	isPlaying = 0;
	level = 1;
	score = 0;
}

void beginGame(void)
{
	int i, j, dx;
	
	/* Init board to zeros */
	for(i = 0; i < GAME_HEIGHT; i++)
	{
		for(j = 0; j < GAME_WIDTH; j++)
		{
			board[i][j] = BOARD_EMPTY;
		}
	}
	
	initialGame = 0;
	win = timeElapsed = 0;
	currentCmd = NULL_CMD;
	prevCmd = MOVE_RIGHT;
	headX = GAME_WIDTH / 2;
	headY = GAME_HEIGHT / 2;
	headIndex = 6;
	tailIndex = 1;
	dx = headX;
	powerUp = 0;
	
	for(i = 0; i < headIndex; i++){
		dx++;
		board[headY][dx - headIndex] = i + 1;
	}
}
