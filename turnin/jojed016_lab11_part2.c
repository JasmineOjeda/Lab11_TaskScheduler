/*	Author: Jasmine Ojeda jojed016@ucr.edu
 *	Lab Section: 022
 *	Assignment: Lab 11  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/z16d42O9TCg
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

unsigned char str_output[] = "CS120B is Legend... wait for it DARY!";
unsigned char str_size = sizeof(str_output) / sizeof(str_output[0]);
unsigned char substr_size = 0x00;
unsigned char str_index = 0x00;
unsigned char lcd_index = 0x00;
unsigned char *substr_output = NULL;
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

enum updateLCDStates { updateLCD_start, updateLCD_increment};

int updateLCDSMTick(int state) {
    switch(state) {
        case updateLCD_start:
	    state = updateLCD_increment;
	    break;
	case updateLCD_increment:
	    state = updateLCD_increment;
	    break;
	default: state = updateLCD_start;
    }

    switch(state) {
        case updateLCD_start:
	    str_index = 0;
	    lcd_index = 16;
	    break;
        case updateLCD_increment:
	    if (str_index < str_size - 1) {
		if (substr_size < 16) {
		    substr_size++;
		}
		if (lcd_index <= 1) {
	            str_index++;
		}
	    }
	    else {
		substr_size = 0;
                str_index = 0;
		lcd_index = 16;
	    }

	    if (lcd_index > 1) {
                lcd_index--;
	    }
	    break;
    }

    substr_output = malloc((substr_size + 1) * (sizeof *substr_output));
    memcpy(substr_output, &str_output[str_index], substr_size);
    substr_output[substr_size] = '\0';

    return 0;
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
	    LCD_DisplayString(lcd_index, substr_output);
	    free(substr_output);
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
    
    static task task1, task2, task3;
    task * tasks[] = {&task1, &task2, &task3};

    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
    const char start = -1;
    const unsigned long GCD = 250;

    task1.state = start;
    task1.period = 250;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadInputSMTick;

    task2.state = start;
    task2.period = 250;
    task2.elapsedTime = task2.period;
    task2.TickFct = &updateLCDSMTick;

    task3.state = start;
    task3.period = 250;
    task3.elapsedTime = task3.period;
    task3.TickFct = &displaySMTick;

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
