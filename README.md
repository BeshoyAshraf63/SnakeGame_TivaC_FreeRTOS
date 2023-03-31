#Important Notes
1- The game is tested on a real TIVA C board and is verified to be working on it.\
2- I used TeraTerm as the serial terminal to display the game, we also used control characters to reset the terminal, hide cursor, and return cursor to
home location.\
3- The code with compiled using Keil v5, with compiler ARMCLANG v 6.18nAnd the version of freertos used is 10.4.6.\

# Features the game supports:
1- Multiple levels. Each level has a higher speed than that of the level before it.\
2- Displaying the total score of all levels during playing and after wining orlosing. The score is based on the length of the snake.\
3- Displaying the elapsed time since the start of the game.\
4- 2 kinds of Power Ups, regular power ups (+) that increase the score by 5, and the length of the snake by 1, and special power ups that are shown at a lower probability that increase the score by 10 and the length by 2.\
5- 100 fps, as the draw task is repeated periodically every 10 ms.\
6- Supports very fast uart communication where we set the Uart to work at 115200 baud rate.\
7- The microcontroller works with a clock frequency of 50 MHZ.\
#Tasks
1- UART0_HandlerTask:
A task for handling the Uart0 receive ISR, it starts the game when ENTER is
pressed or updates the game state when the control arrows are pressed\
2- Task_UpdateTime:
A task for updating the elapsed time, a simple periodic task that is run every
1s to update the time\
3- Task_GameUpdate:
A task that contains the main logic for controlling the movement of the snake
in the data structures and global variables used, so the draw task can use
these data to update the screen.
It's responsible for movement, winning, losing, showing power ups at random
locations, increasing the snake length and more.
It's a periodic task but it's period changes by changing the level (speed), it
starts at a period of 250 ms at level 1 and the period decreases as the level
increases\
4- Task_GamePrint:
A task responsible for printing to screen using uart, the uart driver we made
supports sending characters, string and decimel values, we use a combination
of these functions in this task to show
1- The welcome msg at the beginning
2- The win screen after winning
3- GameOver screen after losing
4- The actual game board update (100 fps) on the screen
