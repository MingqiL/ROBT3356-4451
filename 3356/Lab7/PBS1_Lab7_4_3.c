/*************************************************************************************************
 * PBS1_Lab7_4_3.c
 * - C source file for 4.3 of ROBT3356Lab7_2017.  Basic polling loop with PB state flow.
 *
 *  Author: Greg Scutt
 *  Created: November 10th, 2016
 *  Modified: Novemeber 17th, 2017
 **************************************************************************************************/

#include <msp430.h> 
#include <stdio.h>

// style rule:   all #define names must be ALLCAPS all the time.
#define LED1 BIT0			//	LED1
#define PB1  BIT1			// push button switch

// try to integrate these definitions into code below after you understand the loop.
#define BUTTON_UP P2IN & PB1
#define BUTTON_DOWN !(P2IN & PB1)
#define LED1_ON P1OUT |= LED1
#define LED1_OFF P1OUT &= ~LED1

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Don't disturn the dog. Ever.
	
    volatile unsigned char counter = 0;	// define a counter. Use it to count push events on the PB switch


    // setup output PORT1 to write to LED1. Affect only the necassary bits !! use a mask like LED1. see pre processor defns
	P1DIR |= LED1;


    // setup input PORT to read push button switch 1. Affect only the necassary bits !! use a mask like PB1. see pre processor defns
	P2DIR &= ~PB1;
	P2REN |= PB1;
	P2OUT |= PB1;


    // make sure you are very clear about how this POLLING loop works ... see Davies 4.4.3 for a discussion
	// here I am implementing the 2 loop polling method that Davies discusses in 4.4.3, except I use while loops.
	// Where in this code will you debounce the switch?
    while (1) {
		
    		// button up state. stay in this while loop until button is pressed
        	while (BUTTON_UP) {
        	    if (BUTTON_DOWN) break;
        	}


        	// button pressed state. take action !
        	if (BUTTON_DOWN) {	// check if it is still low after delay. Understand this line of code.
        		P1OUT |= LED1; // Yes: Turn LED1 on (active high!)
        	}



        	// button down state. stay in loop until it is released.
        	while (BUTTON_DOWN) {
        	    if (BUTTON_UP) break;
        	}



        	// button released state
        	P1OUT &= ~LED1; // Turn LED1 off (active high!)
        	counter++;

    }

   }
