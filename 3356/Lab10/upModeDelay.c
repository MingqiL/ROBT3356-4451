#include <msp430.h> 

/*
 * upModeDelay.c
 *
 * Build and download this program. Study it and use it as a template to implement a delay.
 * Note that TOGGLE_CNT is simply used to get a delay of 1 second delay.
 */

#define _10MS_CNT 10000 // number of clock events for 10 ms
#define TOGGLE_CNT 100 // toggle every 100 * 10 ms so you can see the LED flagh
#define LED1 BIT0

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop the dog from barking


    P1DIR |= LED1; // red LED is an output

    /* set TA0 control register. i.e the main timer control register
     *
     clock select SMCLK divisor 1, Up mode, enable TAIFG (main timer) interrupt.
     here's another  example of how predefined bit names can be used to form  words with bitwise ORing ...
     Timer Registers and their bit definitions are all defined in the msp430F5529.h within the include folder in
     the CCS project folder. They match the register description in the  user manual.
     Look it up to understand how this is working !!  This is a popular way to write to register bits in C*/
    TA0CTL = TASSEL_2 | ID_0 | MC_1 | TAIE;


    // TA0  Capture Control Channel 0 Control register
    // TA0CCTL0 =   nothing to really set in this register in this delay example

    // set up the capture/control register with the number of cycles.
    TA0CCR0 = _10MS_CNT - 1; // set rollover interval to 10 ms

    // clear TAIFG interrupt flag.
    TA0CTL &= ~TAIFG;

    /* another way to enable global interrupts is with an intrinsic function
     works just like _bis_SR_register(GIE);
     intrinsic functions are defined in the MSP430 C Compiler Doc */
    _enable_interrupts();


    while(1) {
    	//nothing to do
    }
}


    /* 	Define one of the TIMER A0's interrupt vectors
		Remember there are two ISR vectors for each timer.  They are defined in msp430F5529.h
		TIMER0_A1_VECTOR = 0xFFE8. Understand why this vector and not the other !! */
    #pragma vector = TIMER0_A1_VECTOR
    __interrupt void timer0A1Isr(void) {
    	/*   count is static and will persist for the life time of main.c.  Only accessible by function it is defined in
    	    Its useful for ISR's */
    	static int count = 0;


    	count++; // increment ms count. Only useful for generating a slower freq.  CLC division of MCLK would have worked too
    	if (count == TOGGLE_CNT) {
    		P1OUT ^= LED1; // toggle red LED
    		count = 0;
    	}

    	TA0CTL &= ~TAIFG;  // clear TAIFG interrupt flag. It will not clear automatically unless you do this
    }
