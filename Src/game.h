#ifndef _GAME_
#define _GAME_

#include "TM4C123.h"

#define GAME_WIDTH								41
#define GAME_HEIGHT								21

#define TOP_LEFT_CHAR_ASCII				201
#define TOP_RIGHT_CHAR_ASCII			187
#define HORIZONTAL_CHAR_ASCII			205
#define VERTICAL_CHAR_ASCII				186
#define BOTTOM_LEFT_CHAR_ASCII		200
#define BOTTOM_RIGHT_CHAR_ASCII		188

#define SNAKE_BODY_CHAR_ASCII			176
#define SNAKE_HEAD_CHAR_ASCII			178
#define POWERUP_CHAR_ASCII				'+'
#define LARGE_POWERUP_CHAR_ASCII	'$'

#define NULL_CMD									0xFF
#define BEGIN_CMD									0xD
#define MOVE_UP										'w'
#define MOVE_DOWN									's'
#define MOVE_LEFT									'a'
#define MOVE_RIGHT								'd'

#define TERMINAL_MOVE_TO_HOME			"\033[H"
#define TERMINAL_RESET						"\033[2J"
#define TERMINAL_HIDE_CURSOR			"\033[?25l"

#define BOARD_EMPTY								0
#define BOARD_POWERUP							-1
#define BOARD_LARGE_POWERUP				-2

#define WIN_LENGTH								10

void winGame(void);

void loseGame(void);

void beginGame(void);

void gameInit(void);

void gamePrint(void);

void gameUpdate(void);


#endif /* _GAME_ */