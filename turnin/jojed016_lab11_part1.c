/*	Author: Jasmine Ojeda jojed016@ucr.edu
 *	Lab Section: 022
 *	Assignment: Lab 11  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
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
#include <string.h>
#endif

unsigned char output = 0x00;

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

           switch(x) {
                case '\0': output = 0x1F; break;
		case '0': output = 0x00; break;
		case '1': output = 0x01; break;
		case '2': output = 0x02; break;
		case '3': output = 0x03; break;
		case '4': output = 0x04; break;
		case '5': output = 0x05; break;
		case '6': output = 0x06; break;
		case '7': output = 0x07; break;
	        case '8': output = 0x08; break;
	        case 'A': output = 0x0A; break;
		case 'B': output = 0x0B; break;
	        case 'C': output = 0x0C; break;
		case 'D': output = 0x0D; break;
		case '*': output = 0x0E; break;
		case '#': output = 0x0F; break;
		default: output = 0x1B; break;
	    }
	   break;
    }

    return state;
}

enum display_States { display_display };

int displaySMTick(int state) {
    unsigned char tmp_B;
    switch(state) {
        case display_display: state = display_display; break;
	default: state = display_display; break;
    }

    switch(state) {
        case display_display:
	    tmp_B = output ;
	    break;
    }

    PORTB = tmp_B;

    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;
    //DDRD = 0xFF; PORTD = 0x00;

    //DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xF0; PORTC = 0x0F;

    /* Insert your solution below */
    
    static task task1, task2;
    task * tasks[] = {&task1, &task2};

    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
    const char start = -1;
    const unsigned char GCD = 10;

    task1.state = start;
    task1.period = 10;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadInputSMTick;

    task2.state = start;
    task2.period = 10;
    task2.elapsedTime = task2.period;
    task2.TickFct = &displaySMTick;

    TimerSet(GCD);
    TimerOn();

    unsigned short i;
    
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
