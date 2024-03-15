/*
 * keypad.c
 *
 *  Created on: 2023Äê11ÔÂ8ÈÕ
 *      Author: leeqi
 */
#include <msp430.h>
#include "keypad.h"

void keypadInit(KEYPAD_4X4 *keypad)
{
    P6DIR |= COL_MASK;   // set P6<3:0> as output pins
    P2DIR &= ~ROW_MASK;  // reset P2<7:4> as input pins
    P2REN |= ROW_MASK;   // enable pull resistor for P2<7:4>
    P2OUT &= ~ROW_MASK;  // use pull-down resistor for P2<7:4>
    P3DIR |= LED_MASK;   // set P3<7:0> as output pins

    (*keypad).currentKeyCoord = 0;  // initialize KEYPAD_4X4: curCoord
    (*keypad).currentKey = 0;       // initialize KEYPAD_4X4: keyCnt
    //(*keypad).keyBuffer = 0;        // initialize KEYPAD_4X4: keyBuf
    (*keypad).keyPressCnt = 0;      // initialize KEYPAD_4X4: curKey


}

unsigned char getKeyPress(KEYPAD_4X4 *keypad)
{
    unsigned char presCnt = keypad[0].keyPressCnt;
    unsigned int checkCol = 0;
//    unsigned char writeFlag = 1;
    unsigned char pressFlag = 1;
//    unsigned int writeIndx = 0;

    COL |= 0xF;         // output HIGH for P6<3:0>

    if ((ROW & ROW_MASK) != 0x00)           // P2IN detects signals (key pressed)
    {
        __delay_cycles(300);
        keypad[0].currentKeyCoord = (ROW &= ROW_MASK);  // record ROW values
        COL &= ~COL_MASK;                   // reset COL

        checkCol = 0;
        while(1){    // infinite loop
            COL |= 0x01<<checkCol;          // check for Col number each by each
            __delay_cycles(200);
            if ((ROW & ROW_MASK) != 0x00)   // P2IN detects signal, write coordinates
            {
                __delay_cycles(2000);
                pressFlag = 0;
//                writeFlag = 0;
                keypad[0].currentKeyCoord |= (0x01<<checkCol);    // set Col coordinate
//                writeFlag = 0;
                keypad[0].keyPressCnt++;
                keypad[0].keyBuffer[presCnt] = keypad[0].currentKeyCoord;       // record the current press

                break;

            }
            checkCol++;                     // check for next Col
        }
        presCnt++;
        keypad[0].keyPressCnt = presCnt;
        while ((ROW & ROW_MASK) != 0x00){           // P2IN detects signals (key pressed)
        }
        __delay_cycles(3000);
    }
    return pressFlag;
}

void decodeKeyCoord(KEYPAD_4X4* keypad)
{
        keypad[0].currentKey = keypad[0].currentKeyCoord;

}

void testProg(KEYPAD_4X4 *keypad)
{
    unsigned char presCnt = keypad[0].keyPressCnt;
    unsigned int checkCol = 0;
    unsigned char writeFlag = 1;

    COL |= 0xF;         // output HIGH for P6<3:0>

    if ((ROW & ROW_MASK) != 0x00)           // P2IN detects signals (key pressed)
    {
        __delay_cycles(300);
        keypad[0].currentKeyCoord = (ROW &= ROW_MASK);  // record ROW values
        COL &= ~COL_MASK;                   // reset COL

        checkCol = 0;
        while(1){    // infinite loop
            COL |= 0x01<<checkCol;          // check for Col number each by each
            __delay_cycles(200);
            if ((ROW & ROW_MASK) != 0x00)   // P2IN detects signal, write coordinates
            {
                __delay_cycles(2000);
                keypad[0].currentKeyCoord |= (0x01<<checkCol);    // set Col coordinate
                writeFlag = 0;
                keypad[0].keyPressCnt++;
                if (!writeFlag){
                    writeFlag = 1;
                    if(presCnt != 0) {
                        keypad[0].keyBuffer[presCnt+1] = keypad[0].keyBuffer[presCnt];  // move the previous press
                        keypad[0].keyBuffer[0] = keypad[0].currentKeyCoord;       // record the current press
                        break;
                    }

                    else {
                        keypad[0].keyBuffer[presCnt] = keypad[0].currentKeyCoord;
                        break;
                    }
                }

            }
            checkCol++;                     // check for next Col
        }

        presCnt++;
        keypad[0].keyPressCnt = presCnt;
        while ((ROW & ROW_MASK) != 0x00){           // P2IN detects signals (key pressed)
        }
        __delay_cycles(2000);
        updateLedArray(keypad);    // update LED if key was pressed
    }
}

void updateLedArray(KEYPAD_4X4 *keypad)
{
    LED_ARRAY = keypad[0].currentKeyCoord;
}
