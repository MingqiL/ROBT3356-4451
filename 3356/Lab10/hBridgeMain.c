#include <msp430.h>
#include "keypad.h"
#include "hBridgeCtr.h"

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    struct KEYPAD_4X4 myKeyPad;        // declare a keypad variable
    struct HBRIDGE_CTRL myBridge;       // declare a hbridge variable

    keypadInit(&myKeyPad);      // pass keypad into c function
    hBridgeInit(&myBridge);

    for(;;){
        COL |= 0xF;         // output HIGH for P6<3:0>

        if (PRESS){             // in case the button is pressed, enter below

            __delay_cycles(1000);
            getKeyPress(&myKeyPad); // to get key pressed
            P2IE |= ROW_MASK;       // enable the interrupt again
            while (PRESS);//waiting for release

            P2IE = 0;               // disable the P2 interrupt right after the button release
            __delay_cycles(1000);   // SW debounce
            decodeKeyCoord(&myKeyPad);  // decode curCoord to curKey
            updateLedArray(&myKeyPad);  // update LED if key was pressed
            P2IFG &= ~ROW_MASK;         // clear the flag
            P2IE |= ROW_MASK;           // enable P2IE

            updateBridge(&myBridge, &myKeyPad); // update the H-bridge parameters

        }

    }

    return 0;
}

#pragma vector = PORT2_VECTOR
__interrupt void Port2_ISR1 (void)
{
    switch(__even_in_range(P2IV,16))
    {
    case 0: break; //no interupt
    case 2: //2.0
        break;
    case 4: //2.1
        break;
    case 6: //2.2
        break;
    case 8: //2.3
        break;
    case 10: //2.4
        //when it jump to this case ur row 1 press: set row =1
        if(RELEASE==1)   // when the button released
        {
            getRow=1;
            P2IES |= ROW_MASK;
            PRESS=1;
            RELEASE=0;
        }
        else if (PRESS){
            PRESS=0;
            RELEASE=1;
            P2IES &= ~ROW_MASK;
        }
        P2IFG &= ~ROW_MASK;
        P2IE = 0;
        break;
    case 12: //2.5
        //when it jump to this case ur row 1 press: set row =2
        if(RELEASE==1)  // when the button released
        {
            getRow=2;
            P2IES |= ROW_MASK;
            PRESS=1;
            RELEASE=0;
        }
        else if (PRESS){
            PRESS=0;
            RELEASE=1;
            P2IES &= ~ROW_MASK;
        }
        P2IFG &= ~ROW_MASK;
        P2IE = 0;
        break;
    case 14: //2.6
        //when it jump to this case ur row 1 press: set row =3
        if(RELEASE==1)  // when the button released
        {
            getRow=3;
            P2IES |= ROW_MASK;
            PRESS=1;
            RELEASE=0;
        }
        else if (PRESS){
            PRESS=0;
            RELEASE=1;
            P2IES &= ~ROW_MASK;
        }
        P2IFG &= ~ROW_MASK;
        P2IE = 0;
        break;
    case 16: //2.7
        //when it jump to this case ur row 1 press: set row =4
        if(RELEASE==1)  // when the button released
        {
            getRow=4;
            P2IES |= ROW_MASK;
            PRESS=1;
            RELEASE=0;
        }
        else if (PRESS){
            PRESS=0;
            RELEASE=1;
            P2IES &= ~ROW_MASK;
        }
        P2IFG &= ~ROW_MASK;
        P2IE = 0;
        break;
    default : break;
    }

}
