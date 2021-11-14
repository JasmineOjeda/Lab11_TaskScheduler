/*	Author: Jasmine Ojeda jojed016@ucr.edu
 *	Lab Section: 022
 *	Assignment: Lab 11  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/-33vvWPGT18 
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
//#include "lcd_8bit_task.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

unsigned char output = ' ';

enum keypadInputSM_States { keypadInput_input };

int keypadInputSMTick(int state) {
    static unsigned char x;
    switch(state) {
        case keypadInput_input:
	    state = keypadInput_input;
	    break;
	default:
	    state = keypadInput_input;
	    break;
    }

    switch(state) {
        case keypadInput_input:
           x = GetKeypadKey();
	   break;
    }
    
    if (x != '\0') {
        output = x;
    }

    return state;
}

enum displayStates { display_display };

int displaySMTick(int state) {
    switch(state) {
        case display_display: state = display_display; break;
	default: state = display_display; break;
    }

    switch(state) {
        case display_display:
	    LCD_ClearScreen();	
	    LCD_WriteData(output);
	    break;
    }

    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    DDRA = 0xFF; PORTA = 0x00;

    DDRC = 0xF0; PORTC = 0x0F;

    /* Insert your solution below */
    
    static task task1, task2;
    task * tasks[] = {&task1, &task2};

    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
    const char start = -1;
    const unsigned long GCD = 10;

    task1.state = start;
    task1.period = 10;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadInputSMTick;

    task2.state = start;
    task2.period = 250;
    task2.elapsedTime = task2.period;
    task2.TickFct = &displaySMTick;

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
