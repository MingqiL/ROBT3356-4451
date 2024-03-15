#include <msp430.h> 


/*
 * main.c
 */

#define LED1 BIT0
#define LED2 BIT7
#define PBS1  BIT1
#define DELAY_1000 1000

volatile int counter = 0;

int main(void){


    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer. Never remove.

    P4DIR |= LED2;              //  set direction for LED2
    P1DIR |= LED1;              //  set direction for LED1

    //Input Port Configuration
    P2OUT |= PBS1;              // set P2OUT.1 = 1, so that pullup is selected. See Table 12-1 in User Manual
    P2REN |= PBS1;              // enable pull up resistor for push button switch

    // configure the interrupts
    P2IES &= ~PBS1;             // set edge detection to L->H
    P2IFG = 0;                  // clear interrupt flags by READING the register.
    P2IE |= PBS1;               // enable P2.1 interrupt. Always do this last.  QUESTION.  Why do you do this last?

    // enable the global interrupts with intrinsic function _bis_SR_register
    _bis_SR_register(GIE);

    while (1) {

        // wait for the phone to ring.  Here is where you could put the CPU to sleep with its low power modes and just wait for an interrupt
        P4OUT ^= LED2; // just toggle the led
        _delay_cycles(1000000);
    }

    return 0;
}


// Here is where the ISR address is assigned to the PORT2_VECTOR address
#pragma vector = PORT2_VECTOR // this statement must be left as is. PORT2_VECTOR is defined in msp430.h as are the other interrupt vector addresses.
 __interrupt void Port2_ISR1 (void) // you can choose any name for the ISR. Port2_ISR1
    {



        // QUESTION: Is the CPU still listening to interrupts when it has entered the ISR?


        /* Debounce the press.  This is assuming bouncing happens during a press.
         * QUESTION:  Is it a good idea to debounce in an ISR?  Why or why not?
         * */
         _delay_cycles((long int) 5*DELAY_1000);


        //QUESTION:  Explain what this statement below is doing and what it is for. Use a diagram of a switch changing from H --> L --> H with bouncing at press and release
            while (!(PBS1 & P2IN)){
            }


      /* debounce the released switch.
       * QUESTION:  Is this _delay_cycles really necessary at this location in the ISR?  Can you explain why or why not using a diagram of a switch that bounces both when pressed and released ?  */
        _delay_cycles((long int) 5*DELAY_1000);


        // ok,  now do something in the ISR ...
        P1OUT ^= LED1;

        // QUESTION: what other ways besides as a global variable can counter be declared so it can be used to keep track of ISR entry
        counter++;

        /* clear interrupt flag at end of ISR
        QUESTION: what would happen if the flag wasn't cleared*/
        P2IFG &= ~PBS1;
    }


