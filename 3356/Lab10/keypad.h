/*
 * keypad.h
 *
 *  Created on: 2023Äê11ÔÂ8ÈÕ
 *      Author: leeqi
 */


#define ROW P2IN        // P2 Port for ROW
#define ROW_MASK 0xF0
#define COL P6OUT       // P6 Port for COL
#define COL_MASK 0x0F
#define LED_ARRAY P3OUT // P3 Port for LED
#define LED_MASK 0xFF
#define KEY_BUFF_SZ 255

#define REVERSE 0;
#define FORWARD 1;
#define STOP 1;



volatile unsigned char PRESS;
volatile unsigned char RELEASE;
volatile unsigned char getRow;

typedef struct KEYPAD_4X4 {
    unsigned char currentKeyCoord;      // the row, column coordinate of the pressed key.
    unsigned char keyPressCnt;          // number of key presses since last reset
    unsigned char keyBuffer[KEY_BUFF_SZ];   // stores history of key presses
    unsigned char currentKey;           // the literal key from keypad.
}KEYPAD_4X4;

void keypadInit(KEYPAD_4X4 *keypad);            // initiate the pins
void testProg(KEYPAD_4X4 *keypad);              // connection testing
unsigned char getKeyPress(KEYPAD_4X4 *keypad);  // return 0 if key pressed, -1 if not;
                                                // update curCoord, keyCnt, keyBuf
void decodeKeyCoord(KEYPAD_4X4 *keypad);   // decode the curCoord to curKey and assign
void updateLedArray(KEYPAD_4X4 *keypad);    // update LED Array if key is pressed


