/*
 * LS7366R.c
 *
 *  Created on: 2024Äê3ÔÂ15ÈÕ
 *      Author: leeqi
 */

#include <msp430.h>
#include <usciB1Spi.h>
#include <usciA1Uart.h>
#include <LS7366R.h>
#include <stdint.h>

void initLS7366R()
{
    clearLS7366R(CNTR);

    uint8_t configMDR0 = 0x03;
    writeLS7366R(MDR0, &configMDR0);

    uint8_t configMDR1 = 0x00;
    writeLS7366R(MDR1, &configMDR1);

}

void clearLS7366R(unsigned char reg)
{
    P2OUT &= ~LS_SS; // set SS:P6.5 to low
    usciB1SpiPutChar((CLR | reg)); // clear  register
    while(! (UCTXIFG&UCB1IFG) );
    P2OUT |= LS_SS;  // bring SS high
}


void readLS7366R(unsigned char reg, unsigned char* rxLS7366R)
{
    unsigned char byteSize,i;
    unsigned const char txDummy = 0x00;

    P2OUT |= LS_SS; // set SS to high
    P2OUT &= ~LS_SS; // set SS to low
    usciB1SpiPutChar((RD | reg));  // read reg
    while(! (UCTXIFG & UCB1IFG)); // wait for finish sending
    while(! (UCRXIFG & UCB1IFG)); // wait for finish receiving
    rxLS7366R[0] = UCB1RXBUF; // read garbage byte

    switch (reg){
    case STR:
        // same functionality of MDR1 thus nothing here and pass below
    case MDR0:
        // same functionality of MDR1 thus nothing here and pass below
    case MDR1:
        usciB1SpiPutChar(txDummy);
        while(! (UCRXIFG & UCB1IFG));
        rxLS7366R[0] = UCB1RXBUF;
        ucsiA1UartTxChar(rxLS7366R[0]);
        break;

    case OTR:
        // same functionality of CNTR thus nothing here and pass below
    case CNTR:
        byteSize = 4;
        for (i=0;i<byteSize;i++)
        {
            usciB1SpiPutChar(txDummy);
            while(! (UCTXIFG & UCB1IFG)); // wait for finish sending
            while(! (UCRXIFG & UCB1IFG)); // wait for finish receiving
            rxLS7366R[i] = UCB1RXBUF;
        }
        ucsiA1UartTxString(rxLS7366R);
        break;

    default:
        break;
    }

    __delay_cycles(30);
    P2OUT |= LS_SS;  // bring SS high

}

void writeLS7366R(unsigned char reg, unsigned char* txLS7366R)
{
    unsigned char i;
    P2OUT |= ~LS_SS;
    P2OUT &= ~LS_SS; // set SS to low

    usciB1SpiPutChar((WR | reg));

    switch (reg)
    {
    case MDR0:
        // same functionality of MDR1 thus nothing here and pass below
    case MDR1:
        usciB1SpiPutChar(txLS7366R[0]);
        break;

    case DTR:
        for (i=0; i<4; i++)
        {
            usciB1SpiPutChar(txLS7366R[i]);
        }
        break;


    default:
        break;
    }

    while(! (UCTXIFG & UCB1IFG)); // wait for finish sending
    __delay_cycles(30);
    P2OUT |= LS_SS;  // bring SS high
}



void loadLS7366R(unsigned char reg)
{
    P2OUT &= ~LS_SS; // set SS:P6.0 to low
    usciB1SpiPutChar((LOAD | reg)); // load  register
    while(! (UCTXIFG&UCB1IFG) );
    __delay_cycles(30);
    P2OUT |= LS_SS;  // bring SS high

}

