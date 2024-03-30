/*
 * usciB0Spi.c
 *
 *  Created on: 2024Äê3ÔÂ15ÈÕ
 *      Author: leeqi
 */


#include <msp430.h>
#include <usciB0Spi.h>

void usciB0SpiInit(unsigned char spiMST, unsigned int sclkDiv, unsigned char* sclkMode, unsigned char spiLoopBack)
{
    P3SEL |= B0CLK + B0SIMO;   //P3.2:SMCLK, P3.0:SIMO
//    P2DIR |= B0SS;
//    P2OUT |= B0SS;

    UCB0CTL1 |= UCSWRST; // disable the spi
    UCB0CTL0 = 0;       // reset CTL0 configuration

    // configure master mode select
    if (spiMST) UCB0CTL0 |= UCMST;
    else        UCB0CTL0 &= ~UCMST;

    // divide the clock
    UCB0BR0 = (sclkDiv&0xFF);
    UCB0BR1 = (sclkDiv>>8) & 0xFF;

    // configure UCCKPL and UCCKPH
    if (sclkMode[1]) UCB0CTL0 |= UCCKPH;
    else             UCB0CTL0 &= ~UCCKPH;

    if (sclkMode[2]) UCB0CTL0 |= UCCKPL;
    else             UCB0CTL0 &= ~UCCKPL;

    // configure loopBack
    if (spiLoopBack) UCB0STAT |= UCLISTEN;
    else             UCB0STAT &= ~UCLISTEN;

    // select SMCLK
    UCB0CTL1 |= UCSSEL_2;

    // configure others
    UCB0CTL0 |= UCSYNC;
    UCB0CTL0 |=  UCMSB;
    UCB0CTL0 &= ~UC7BIT;
    UCB0CTL0 |= UCMODE_0;


    // enable SPI
    UCB0CTL1 &= ~UCSWRST;

}

void usciB0SpiPutChar(unsigned char txByte)
{
    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = txByte;

    while (!(UCB0IFG & UCRXIFG));
    UCB0IFG &= ~UCRXIFG;

    // dont fking touch, works Mar26,2024, worked time: 6hrs+fking weekend.
}

/************************************************************************************
* Function: ucsiA1UartTxBuffer
* - writes an array of characters, one char at a time to UCA1TXBUF by calling
*   ucsiA1UartTxChar. Stops when it reaches bufLen
* argument:
* Arguments: buffer - pointer to char array to be transmitted
*            bufLen - length of the buffer
*
* return: number of characters transmitted
* Author: Andrew Friesen
* Date: December 30, 2021
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int usciB0SpiTxBuffer(char * buffer, int bufLen)
{
    // complete this function
    unsigned char i;
    // while not a null character
    for (i=0;(buffer[i]!='\0')&&(i<bufLen);i++)
        usciB0SpiPutChar(buffer[i]);
    return (int)i;
}
