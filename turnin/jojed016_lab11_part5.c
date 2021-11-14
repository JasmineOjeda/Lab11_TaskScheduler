/*	Author: Jasmine Ojeda jojed016@ucr.edu
 *	Lab Section: 022
 *	Assignment: Lab 11  Exercise 5
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/1lbk1I8dQ5s 
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "scheduler.h"
#include "timer.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

unsigned char player_loc = 1;
unsigned char upObs_loc = 16;
unsigned char downObs_loc = 25;
unsigned char obstacle = '#';
unsigned char pause = 1;
unsigned char lose = 0;
unsigned char message[] = "You lose :(";

enum pauseStates { pause_start, pause_wait, pause_press };

int pauseSMTick(int state) {
    unsigned tmp_C = (~PINC) & 0x01;

    switch(state) {
        case pause_start:
	    pause = 1;
	    lose = 0;
	    state = pause_wait;
	    break;
	case pause_wait:
	    if (tmp_C) {
                pause = (~pause) & 0x01;
		lose = 0;
		state = pause_press;
	    }
	    else {
                state = pause_wait;
	    }
	    break;
	case pause_press:
	    if (tmp_C) {
		state = pause_press;
            }
	    else {
		state = pause_wait;
            }
	    break;
	default: state = pause_start; break;
    }

    return state;
}

enum playerUpStates { playerUp_start, playerUp_wait, playerUp_press };

int playerUpSMTick(int state) {
    unsigned tmp_C1 = ((~PINC) & 0x02) >> 1;
    unsigned tmp_C2 = ((~PINC) & 0x04) >> 2;
    
    switch(state) {
        case playerUp_start:
            player_loc = 1;
            state = playerUp_wait;
            break;
        case playerUp_wait:
	    if (!pause) {
                if (tmp_C1) {
                    player_loc = 1;
                    state = playerUp_press;
                }
	        else if(tmp_C2) {
                    player_loc = 17;
		    state = playerUp_press;
	        }
                else {
                    state = playerUp_wait;
                }
	    }
            break;
        case playerUp_press:
            if (tmp_C1 || tmp_C2) {
                state = playerUp_press;
            }
            else {
                state = playerUp_wait;
            }
            break;
        default: state = playerUp_start; break;
    }

    return state;
}

enum obstacleStates { obstacle_start, obstacle_update};

int obstacleSMTick(int state) {

    switch(state) {
        case obstacle_start:
	    upObs_loc = 16;
	    downObs_loc = 25;
	    state = obstacle_update;
	    break;
	case obstacle_update:
            state = obstacle_update;
	    break;
	default: state = obstacle_start; break;
    }

    switch(state) {
        case obstacle_update:
            if (!pause) {
	        if (upObs_loc > 1) {
                    upObs_loc--;
                }
	        else {
		    upObs_loc = 16;
	        }

	        if (downObs_loc > 17) {
		    downObs_loc--;
	        }
	        else {
		    downObs_loc = 32;
	        }
	    }

            break;
    }

    return state;
}

enum collisionStates { collision_start, collision_detect};

int collisionSMTick(int state) {
    switch(state) {
        case collision_start:
            lose = 0;
	    state = collision_detect;
	    break;
	case collision_detect:
            state = collision_detect;
	    break;
	default: state = collision_start; break;
    }

    switch(state) {
        case collision_detect:
	    if (player_loc == upObs_loc || player_loc == downObs_loc) {
                lose = 1;
		pause = 1;
		upObs_loc = 16;
		downObs_loc = 25;
		player_loc = 1;
	    }

	    break;
    }

    return state;
}

enum displayStates { display_start, display_update, display_lose};

int displaySMTick(int state) {
    switch(state) {
        case display_start:
	    state = display_update;
	    break;
	case display_update:
	    if (!lose) {
                state = display_update;
	    }
	    else {
		LCD_ClearScreen();
		LCD_DisplayString(3, message);
		state = display_lose;
            }
	    break;
	case display_lose:
	    if (!lose) {
		LCD_ClearScreen();
                state = display_update;
            }
            else {
                state = display_lose;
            }
	    break;
	default: state = display_start; break;
    }

    switch(state) {
        case display_update:
	    if (upObs_loc != 16) {
		LCD_Cursor(upObs_loc + 1);
	        LCD_WriteData(' ');
	    }
	    else {
		LCD_Cursor(1);
		LCD_WriteData(' ');
	    }
	    LCD_Cursor(upObs_loc);
	    LCD_WriteData(obstacle);


	    if (downObs_loc != 32) {
		LCD_Cursor(downObs_loc + 1);
                LCD_WriteData(' ');
            }
            else {
		LCD_Cursor(17);
                LCD_WriteData(' ');
            }
	    LCD_Cursor(downObs_loc);
	    LCD_WriteData(obstacle);
            

	    if (player_loc == 1) {
		LCD_Cursor(17);
                LCD_WriteData(' ');
	    }
	    else {
		LCD_Cursor(1);
                LCD_WriteData(' ');
	    }
	    LCD_Cursor(player_loc);
	    LCD_WriteData('0');
	    break;
    }

    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    DDRA = 0xFF; PORTA = 0x00;

    DDRC = 0x00; PORTC = 0xFF;

    /* Insert your solution below */
    
    static task task1, task2, task3, task4, task5;
    task * tasks[] = {&task1, &task2, &task3, &task4, &task5};

    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
    const char start = -1;
    const unsigned long GCD = 10;

    task1.state = start;
    task1.period = 10;
    task1.elapsedTime = task1.period;
    task1.TickFct = &pauseSMTick;

    task2.state = start;
    task2.period = 10;
    task2.elapsedTime = task2.period;
    task2.TickFct = &playerUpSMTick;

    task3.state = start;
    task3.period = 50;
    task3.elapsedTime = task3.period;
    task3.TickFct = &obstacleSMTick;

    task4.state = start;
    task4.period = 50;
    task4.elapsedTime = task4.period;
    task4.TickFct = &collisionSMTick;

    task5.state = start;
    task5.period = 10;
    task5.elapsedTime = task5.period;
    task5.TickFct = &displaySMTick;

    TimerSet(GCD);
    TimerOn();

    unsigned short i;
    LCD_init();
    
    while (1) {
        for (i = 0; i < numTasks; i++) {
            if (tasks[i]->elapsedTime >= tasks[i]->period) {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
		tasks[i]->elapsedTime = 0;
	    }

            tasks[i]->elapsedTime += GCD;
	}

	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
