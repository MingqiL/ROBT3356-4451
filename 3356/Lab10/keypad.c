/*
 * keypad.c
 *
 *  Created on: 2023Äê11ÔÂ8ÈÕ
 *      Author: leeqi
 */
#include <msp430.h>
#include "keypad.h"

//volatile unsigned char pbState = RELEASE;

void keypadInit(KEYPAD_4X4 *keypad)
{
    P6DIR |= COL_MASK;   // set P6<3:0> as output pins

    P2DIR &= ~ROW_MASK;  // reset P2<7:4> as input pins
    P2REN |= ROW_MASK;   // enable pull resistor for P2<7:4>
    P2OUT &= ~ROW_MASK;  // use pull-down resistor for P2<7:4>
    P2IFG = 0;
    P2IE  |= ROW_MASK;
    P2IES &= ~ROW_MASK;

    P3DIR |= LED_MASK;   // set P3<7:0> as output pins

    (*keypad).currentKeyCoord = 0;  // initialize KEYPAD_4X4: curCoord
    (*keypad).currentKey = 0;       // initialize KEYPAD_4X4: keyCnt
    //(*keypad).keyBuffer = 0;        // initialize KEYPAD_4X4: keyBuf
    (*keypad).keyPressCnt = 0;      // initialize KEYPAD_4X4: curKey

    _bis_SR_register(GIE);

    RELEASE = 1;
    PRESS = 0;


}

unsigned char getKeyPress(KEYPAD_4X4 *keypad)
{
    unsigned char presCnt = keypad[0].keyPressCnt;
    unsigned int checkCol = 0;
    unsigned char pressFlag = 1;


    //

    COL &= ~COL_MASK;                   // reset COL

    keypad[0].currentKeyCoord = (0x01<<(getRow+3));
    checkCol = 0;
    while(1){    // infinite loop
        COL |= 0x01<<checkCol;          // check for Col number each by each
        if ((ROW & ROW_MASK) != 0x00)   // P2IN detects signal, write coordinates
        {
            pressFlag = 0;
            keypad[0].currentKeyCoord |= (0x01<<checkCol);    // set Col coordinate
            keypad[0].keyPressCnt++;
            keypad[0].keyBuffer[presCnt] = keypad[0].currentKeyCoord;       // record the current press
            break;
        }
        checkCol++;                     // check for next Col
    }
    presCnt++;
    keypad[0].keyPressCnt = presCnt;
    while ((ROW & ROW_MASK) != 0x00)
    {           // P2IN detects signals (key pressed)
    }
    __delay_cycles(3000);
    return pressFlag;
}

void decodeKeyCoord(KEYPAD_4X4* keypad)
{
    volatile unsigned char key = keypad[0].currentKeyCoord;
    switch(key){
    case 0x11: keypad[0].currentKey = '1'; break;
    case 0x12: keypad[0].currentKey = '2'; break;
    case 0x14: keypad[0].currentKey = '3'; break;
    case 0x18: keypad[0].currentKey = 'A'; break;
    case 0x21: keypad[0].currentKey = '4'; break;
    case 0x22: keypad[0].currentKey = '5'; break;
    case 0x24: keypad[0].currentKey = '6'; break;
    case 0x28: keypad[0].currentKey = 'B'; break;
    case 0x41: keypad[0].currentKey = '7'; break;
    case 0x42: keypad[0].currentKey = '8'; break;
    case 0x44: keypad[0].currentKey = '9'; break;
    case 0x48: keypad[0].currentKey = 'C'; break;
    case 0x81: keypad[0].currentKey = '*'; break;
    case 0x82: keypad[0].currentKey = '0'; break;
    case 0x84: keypad[0].currentKey = '#'; break;
    case 0x88: keypad[0].currentKey = 'D'; break;
    default: break;
    }
}

void updateLedArray(KEYPAD_4X4 *keypad)
{
    LED_ARRAY = keypad[0].currentKeyCoord;
}
