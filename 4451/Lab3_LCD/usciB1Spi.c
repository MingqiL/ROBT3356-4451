/*
 * usciB1Spi.c
 *
 *  Created on: 2024Äê1ÔÂ22ÈÕ
 *      Author: leeqi
 */

#include <msp430.h>
#include <usciSpi.h>

void usciB1SpiInit(unsigned char spiMST, unsigned int sclkDiv, unsigned char* sclkMode, unsigned char spiLoopBack)
{
    P4SEL |= B1CLK + B1SIMO;   //P4.3:SMCLK, P4.1:SIMO
    P4DIR |= B1CLK + B1SIMO;
    P6DIR |= SS;
    P6OUT |= SS;

    UCB1CTL1 |= UCSWRST; // disable the spi
    UCB1CTL0 = 0;       // reset CTL0 configuration

    // configure master mode select
    if (spiMST) UCB1CTL0 |= UCMST;
    else        UCB1CTL0 &= ~UCMST;

    // divide the clock
    UCB1BR0 = (sclkDiv&0xFF);
    UCB1BR1 = (sclkDiv>>8) & 0xFF;

    // configure UCCKPL and UCCKPH
    if (sclkMode[1]) UCB1CTL0 |= UCCKPH;
    else             UCB1CTL0 &= ~UCCKPH;

    if (sclkMode[2]) UCB1CTL0 |= UCCKPL;
    else             UCB1CTL0 &= ~UCCKPL;

    // configure loopBack
    if (spiLoopBack) UCB1STAT |= UCLISTEN;
    else             UCB1STAT &= ~UCLISTEN;

    // select SMCLK
    UCB1CTL1 |= UCSSEL_2;

    // configure others
    UCB1CTL0 |= UCSYNC
             |  UCMSB
             & ~UC7BIT
             | UCMODE_0;


    // enable SPI
    UCB1CTL1 &= ~UCSWRST;

}

void usciB1SpiPutChar(unsigned char txByte)
{
    while (!(UCB1IFG & UCTXIFG)); // is this efficient ?
    //__delay_cycles(1000);
            UCB1IFG &= ~UCRXIFG;
            P6OUT &= ~SS;   // enable SS
            UCB1TXBUF = txByte;
            while (!(UCB1IFG & UCRXIFG)); // wait for Tx finish transmission
            P6OUT |= SS;    // disable SS
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
int usciB1SpiTxBuffer(char * buffer, int bufLen)
{
    // complete this function
    unsigned char i;
    // while not a null character
    for (i=0;(buffer[i]!='\0')&&(i<bufLen);i++)
        usciB1SpiPutChar(buffer[i]);
    return (int)i;
}
int usciB1SpiTxBufferSS(char * buffer, int bufLen)
{
    // complete this function
    unsigned char i;
    // while not a null character
    for (i=0;(buffer[i]!='\0')&&(i<bufLen);i++)
        usciB1SpiPutChar(buffer[i]);

    return (int)i;
}
