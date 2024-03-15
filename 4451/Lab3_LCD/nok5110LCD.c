/*************************************************************************************************
 * nok5110LCD.c
 * - C implementation or source file for NOKIA 5110 LCD.
 *
 *  Author: Greg Scutt
 *  Created on: Feb 20th, 2017
 **************************************************************************************************/



// nok5110LCD pin connectivity --> to MSP-EXP430F5529LP EVM.
//  8-LIGHT  	-->  	no connection necassary
//  7-SCLK  	-->  	MS430EVM  P4.3 or UCB1CLK
//  6-DN(MOSI)  -->  	MS430EVM  P4.1 or UCB1SIMO
//  5-D/C'  	-->  	MS430EVM  P4.2. 	Kept as I/O pin !!
//  4-RST'  	-->  	MS430EVM or supply VSS
//  3-SCE'  	-->  	MS430EVM  P4.0.  	Kept as I/O pin !!
//  2-GND  		-->  	MS430EVM or supply VSS
//  1-VCC  		-->  	MS430EVM or supply 3V3



#include <msp430.h>
#include <usciSpi.h>
#include "nok5110LCD.h" // remove _lab from file names


// 2-D 84x6 array that stores the current pixelated state of the display.
// remember a byte (8 bits) sets 8 vertical pixels in a column allowing 8x6=48 rows
// note that this array is GLOBAL to this file only. In that way it is protected from access from other functions in files.
// said another way - it is a private global array with local scope to the file in which the defining declaration exists.
// we don't want other functions messing with the shadow RAM. This is the reason for static and for its dec/defn in the .c file
static unsigned char currentPixelDisplay[LCD_MAX_COL][LCD_MAX_ROW / LCD_ROW_IN_BANK];

/************************************************************************************
* Function: nokLcdInit
* -
* argument:
*   none
* return: none
* Author: Greg Scutt
* Date: Feb 20th, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void nokLcdInit(void) {

    P2DIR |= RST;   // P2.2: RST
    P4DIR |= SCE + DAT_CMD;     // port 4.0: SCE, port 4.2:D/C
    // gScutt.  do an SPI init with ucsiB1SpiInit  from ucsiSpi.h before this function call !!
    _NOK_LCD_RST;   // reset impulse

    P4OUT   &=  ~(SCE | DAT_CMD);   // Set DC and CE Low

    // send initialization sequence to LCD module
    nokLcdWrite(LCD_EXT_INSTR, DC_CMD);
    nokLcdWrite(LCD_SET_OPVOLT, DC_CMD);
    nokLcdWrite(LCD_SET_TEMPCTRL, DC_CMD);
    nokLcdWrite(LCD_SET_SYSBIAS, DC_CMD);
    nokLcdWrite(LCD_BASIC_INSTR, DC_CMD);
    nokLcdWrite(LCD_NORMAL_DISP, DC_CMD);

    nokLcdClear(); // clear the display
}

// return -1 if over sized line, return 0 if draw successfully
int nokLcdDrawLine(unsigned char x_start, unsigned char y_start, unsigned char x_end, unsigned char y_end)
{
    unsigned char x0,y0;
    signed char sx,sy,dx,dy;
    signed int p;
    x0 = x_start;
    y0 = y_start;

    dx = (x_end - x_start);
    sx = x_start < x_end ? 1 : (x_start=x_end)? 0:-1;
    dy = (y_end - y_start);
    sy = y_start < y_end ? 1 : (y_start=y_end)? 0:-1;

    if (dx>dy){
        p = 2 * dy - dx;

        while ( (x0<=x_end)) {
            nokLcdSetPixel(x0,y0);
            x0 += sx;
            if (p < 0) {
                p += 2 * dy;
            } else {
                p += 2 * (dy - dx);
                y0 += sy;
            }
        }
    }
    else{
        p = 2 * dx - dy;
        while ((y0<=y_end)) {
            nokLcdSetPixel(x0,y0);
            y0 += sy;
            if (p < 0) {
                p += 2 * dx;
            } else {
                p += 2 * (dx - dy);
                x0 += sx;
        }
    }
    }

    return 0;
}


// return -1 if over sized line, return 0 if draw successfully
int nokLcdDrawScrnLine(unsigned char x_start, unsigned char y_start, unsigned char mode)
{
    unsigned char i;
    unsigned char length;

    if ( (x_start)>LCD_MAX_COL || (y_start)>LCD_MAX_ROW ) return -1;

    switch (mode){
        case HORIZONTAL:
            length = LCD_MAX_COL-x_start;
            nokLcdWrite(LCD_SET_HORIZONTAL, DC_CMD);    // configure drawing direction
            for (i=0;i<length;i++){
                nokLcdSetPixel(x_start+i, y_start);     // draw line
            }
            break;
        case VERTICAL:
            length = LCD_MAX_ROW-y_start;
            nokLcdWrite(LCD_SET_VERTICAL, DC_CMD);      // configure drawing direction
            for (i=0;i<length;i++){
                nokLcdSetPixel(x_start, y_start+i);     // draw line
            }

    }


    return 0;
}

/************************************************************************************
* Function: nokLcdWrite
* - performs write sequence to send data or command to nokLCD. Calls spiTxByte to transmit serially to nokLCD
* argument:
* Arguments: lcdByte - the 8 bit char (data or command) that is written to nokLCD.
* 			 cmdType - 0 - lcdByte is a cmd,   1 - lcdByte is data.
* return: none
* Author: Greg Scutt
* Date: Feb 20th, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void nokLcdWrite(char lcdByte, char cmdType) {
	
    lcdByte = (unsigned char)lcdByte;
	// check cmdType and output correct DAT_CMD signal to PORT4 based on it.
    if(cmdType == DC_DAT)   P4OUT |= DAT_CMD;   // data mode
    else                    P4OUT &= ~DAT_CMD;  // command mode

    P4OUT &= ~SCE;          // activate the SCE. the chip select

    spiTxByte(lcdByte);// transmit lcdByte with spiTxByte

	// wait for transmission  complete ?  If so, disable the SCE
    P4OUT |= SCE;

}

/************************************************************************************
* Function: nokLcdSetPixel
* -
* argument:
*	xPos - The horizontal pixel location in the domain (0 to 83)
*	yPos - The vertical pixel location in the domain (0 to 47)
*
* return: 0 - pixel was valid and written.  1 - pixel not valid
* Author: Greg Scutt
* Date: Feb 20th, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
unsigned char nokLcdSetPixel(unsigned char xPos, unsigned char yPos) {
	unsigned char bank; // a bank is a group of 8 rows, selected by 8 bits in a byte

	// verify pixel position is valid
	if ((xPos < LCD_MAX_COL) && (yPos < LCD_MAX_ROW)) {

		// if-else statement avoids costly division
		if (yPos<8) bank = 0;
		else if (yPos<16) bank = 1;
		else if (yPos<24) bank = 2;
		else if (yPos<32) bank = 3;
		else if (yPos<40) bank = 4;
		else if (yPos<48) bank = 5;

		// set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
		nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
		nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);

		// update the pixel being set in currentPixelDisplay array
		currentPixelDisplay[xPos][bank] |= BIT0 << (yPos % LCD_ROW_IN_BANK); // i.e if yPos = 7 then BIT0 is left shifted 7 positions to be 0x80. nice
		nokLcdWrite(currentPixelDisplay[xPos][bank], DC_DAT); // write the data. this is DATA DC_DAT
		return 0;
	}
	return 1;
}


void nokLcdClear(){
    unsigned char i,j;
    for ( j=0 ; j<(LCD_MAX_ROW) ; j++ ){
        for ( i=0 ; i<LCD_MAX_COL ; i++ ){
            nokLcdWrite(LCD_SET_XRAM | i, DC_CMD);
            nokLcdWrite(LCD_SET_YRAM | j, DC_CMD);
            // update the pixel being set in currentPixelDisplay array
            currentPixelDisplay[i][j] = 0; // reset byte
            nokLcdWrite(currentPixelDisplay[i][j], DC_DAT); // write the data. this is DATA DC_DAT
        }
    }
}


/************************************************************************************
* Function: spiTxByte
* - if TXBUFFER is ready!!  then a byte is written to buffer and serialized on SPI UCB1. nothing else happens.
* argument:
*	txData - character to be serialized over SPI
*
* return: none
* Author: Greg Scutt
* Date: Feb 20th, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void spiTxByte(char txData)
{
	// transmit a byte on SPI if it is ready !!
    while (!(UCB1IFG & UCTXIFG));
    UCB1IFG &= ~UCRXIFG;
    // P6OUT &= ~SS_NOT;   // enable SS
    UCB1TXBUF = txData;
    while (!(UCB1IFG & UCRXIFG)); // wait for Tx finish transmission
    //P6OUT |= SS_NOT;    // disable SS
}

void forFun(){
    unsigned char i,j,hi,numWord,tot_hi;
    unsigned int k=0;
    numWord=4;
    hi = 20;
    tot_hi = hi*numWord;

    static unsigned char yuedui[] = {
                                     0x00, 0x38, 0x00, 0x03, 0x07, 0x00,
                                     0x00, 0x3c, 0x00, 0x07, 0x87, 0x80,
                                     0x02, 0x7c, 0xe0, 0x07, 0x87, 0x80,
                                     0x03, 0xff, 0xf0, 0x0f, 0x07, 0x00,
                                     0x03, 0xff, 0xf0, 0x0f, 0x07, 0x1c,
                                     0x03, 0xff, 0xe0, 0x1f, 0xff, 0xfe,
                                     0x03, 0xff, 0xfc, 0x1f, 0xcf, 0xc0,
                                     0x03, 0x80, 0xfe, 0x3f, 0x1f, 0xc0,
                                     0x03, 0xff, 0xfe, 0x3e, 0x1f, 0xc0,
                                     0x03, 0xff, 0xf8, 0x7e, 0x3f, 0xe0,
                                     0x03, 0x81, 0xf0, 0xee, 0x7f, 0x70,
                                     0x7f, 0xff, 0xe0, 0xce, 0x77, 0x78,
                                     0x7f, 0xff, 0xe0, 0x0e, 0xe7, 0x3c,
                                     0x30, 0x0f, 0xe0, 0x0f, 0xe7, 0x7f,
                                     0x00, 0x3e, 0xe0, 0x0f, 0xff, 0xff,
                                     0x01, 0xf8, 0xe0, 0x0f, 0x77, 0x0c,
                                     0x07, 0xe7, 0xe0, 0x0e, 0x07, 0x00,
                                     0x3f, 0x8f, 0xe0, 0x0e, 0x07, 0x00,
                                     0x7e, 0x07, 0xe0, 0x0e, 0x07, 0x00,
                                     0x70, 0x01, 0xc0, 0x0e, 0x07, 0x00,

                                     0x03, 0x00, 0x80, 0x00, 0x1c, 0x00,
                                     0x07, 0x81, 0xc0, 0x00, 0x1e, 0x00,
                                     0x07, 0xb1, 0xf8, 0x18, 0x0e, 0x1c,
                                     0x0f, 0xff, 0xfc, 0x1f, 0xff, 0xfe,
                                     0x0f, 0xff, 0xfc, 0x1f, 0xff, 0xfe,
                                     0x1e, 0xff, 0xfe, 0x1f, 0xff, 0xf0,
                                     0x1e, 0xff, 0xff, 0x1f, 0xff, 0xf8,
                                     0x3f, 0xfd, 0xd8, 0x1d, 0x8c, 0x7c,
                                     0x3d, 0xff, 0xf8, 0x1f, 0xff, 0xfe,
                                     0x7d, 0xff, 0xf8, 0x1f, 0xff, 0xfe,
                                     0xfd, 0x3f, 0xfc, 0x1f, 0xff, 0xf8,
                                     0x1f, 0xbf, 0xfc, 0x1d, 0xcf, 0x7c,
                                     0x1f, 0xf7, 0xcc, 0x1d, 0xef, 0x7c,
                                     0x1d, 0xff, 0xfe, 0x1c, 0xff, 0xf8,
                                     0x1d, 0xff, 0xfe, 0x38, 0x7d, 0xe0,
                                     0x1c, 0xf9, 0xc0, 0x3f, 0xed, 0xf0,
                                     0x1d, 0xff, 0xff, 0x7f, 0xfc, 0xfe,
                                     0x1f, 0xff, 0xff, 0x7f, 0x7c, 0x7e,
                                     0x1f, 0x8f, 0xfc, 0xe6, 0x3c, 0x1c,
                                     0x1e, 0x00, 0x00, 0xc0, 0x18, 0x00,

                                     0x00, 0x00, 0x00, 0x03, 0x80, 0x00,
                                     0x07, 0x00, 0x08, 0x07, 0x80, 0x00,
                                     0x07, 0x82, 0x7c, 0x07, 0x80, 0x1c,
                                     0x03, 0xb7, 0xfe, 0x0f, 0xff, 0xfe,
                                     0x7f, 0xff, 0xfc, 0x0f, 0xff, 0xfc,
                                     0x3c, 0xf7, 0x00, 0x1c, 0x1c, 0x00,
                                     0x1e, 0xf7, 0x00, 0x38, 0x1c, 0x00,
                                     0x0e, 0xff, 0x0c, 0x7c, 0x1c, 0x70,
                                     0xff, 0xff, 0xff, 0x6f, 0xff, 0xf8,
                                     0x7f, 0xff, 0xfe, 0x0f, 0xff, 0xf8,
                                     0x7f, 0xff, 0x30, 0x0e, 0x1c, 0x00,
                                     0x7f, 0xff, 0x30, 0x0e, 0x1c, 0x0c,
                                     0x3f, 0xe7, 0x30, 0xff, 0xff, 0xfe,
                                     0x1f, 0xf7, 0x30, 0xff, 0xff, 0xfe,
                                     0x3f, 0xff, 0x30, 0x60, 0x1c, 0x00,
                                     0x3b, 0xbe, 0x30, 0x00, 0x1c, 0x00,
                                     0x7f, 0xbe, 0x30, 0x00, 0x1c, 0x00,
                                     0xff, 0xbc, 0x30, 0x00, 0x1c, 0x00,
                                     0xcf, 0xf8, 0x30, 0x00, 0x1c, 0x00,
                                     0x03, 0x70, 0x30, 0x00, 0x1c, 0x00,

                                     0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00,
                                     0x0e, 0x0f, 0x00, 0x00, 0x01, 0xf0,
                                     0x0e, 0x0f, 0x00, 0x06, 0x3f, 0xf0,
                                     0x0e, 0x0e, 0x30, 0x0f, 0xff, 0xf0,
                                     0x0e, 0xff, 0xfc, 0x0f, 0xff, 0x00,
                                     0x3f, 0xce, 0x38, 0x0e, 0x0e, 0x00,
                                     0x3f, 0xce, 0x38, 0x0e, 0x0e, 0x00,
                                     0x7f, 0xce, 0x38, 0x0e, 0x0c, 0x00,
                                     0x7f, 0xce, 0x38, 0x0e, 0x0c, 0x1c,
                                     0xfe, 0x8e, 0x3e, 0x1f, 0xff, 0xfe,
                                     0x0f, 0xff, 0xff, 0x1c, 0x8c, 0x00,
                                     0x0f, 0xcf, 0x80, 0x01, 0xcd, 0xc0,
                                     0x0e, 0x1f, 0x80, 0x03, 0xed, 0xe0,
                                     0x0e, 0x1d, 0xc0, 0x07, 0xec, 0xf8,
                                     0x0e, 0x3d, 0xe0, 0x0f, 0x8c, 0x7c,
                                     0x0e, 0x78, 0xf0, 0x1e, 0x0c, 0x3c,
                                     0x0e, 0xf0, 0xfc, 0x7d, 0xfc, 0x1c,
                                     0x0f, 0xe0, 0x7f, 0x71, 0xfc, 0x1c,
                                     0x0f, 0xc0, 0x3f, 0x60, 0x7c, 0x00,
                                     0x0f, 0x80, 0x1c, 0x00, 0x18, 0x00

    };


        // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
    for (j=0;j<tot_hi;j++){
        for(i=0;i<6;i++){
        nokLcdWrite(LCD_SET_XRAM | (tot_hi-j), DC_CMD);
        nokLcdWrite(LCD_SET_YRAM | (i), DC_CMD);

           yuedui[k] = (yuedui[k] & 0xF0) >> 4 | (yuedui[k] & 0x0F) << 4;
           yuedui[k] = (yuedui[k] & 0xCC) >> 2 | (yuedui[k] & 0x33) << 2;
           yuedui[k] = (yuedui[k] & 0xAA) >> 1 | (yuedui[k] & 0x55) << 1;

        nokLcdWrite(yuedui[k++], DC_DAT); // write the data. this is DATA DC_DAT
        }
    }

    __delay_cycles(99999999);



}

