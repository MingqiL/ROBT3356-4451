#include <msp430.h> 
#include "keypad.h"


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	struct KEYPAD_4X4 myKeyPad;        // declare a keypad variable

	keypadInit(&myKeyPad);      // pass keypad into c function
//	for(;;){
//	testProg(&myKeyPad);        // test function for connection testing
//	}
	for(;;){
	    if (!getKeyPress(&myKeyPad)){
	        decodeKeyCoord(&myKeyPad);  // decode curCoord to curKey
	        updateLedArray(&myKeyPad);  // update LED if key was pressed
	    }
	}



	return 0;
}
