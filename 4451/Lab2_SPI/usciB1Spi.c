/*
 * usciB1Spi.c
 *
 *  Created on: 2024Äê1ÔÂ22ÈÕ
 *      Author: leeqi
 */

#include <msp430.h>
#include 'interface.h'

void usciB1SpiInit(unsigned char spiMST, unsigned int sclkDiv, unsigned char sclkMode, unsigned char spiLoopBack)
{
    UCB1CTL1 |= UCSWRST; // disable the spi
    UCB1CTL0 = 0;       // reset CTL0 configuration

    // configure master mode select
    if (spiMST) UCB1CTL0 &= ~UCMST;
    else        UCB1CTL0 |=  UCMST;

    // divide the clock
    UCB1BR0 = (sclkDiv&0xFF);
    UCB1BR1 = (sclkDiv>>8);

    // configure UCCKPL and UCCKPH
    UCB1CTL1 |= UCCKPL
             &  ~UCCKPH;

    // configure loopBack
    if (spiLoopBack) UCB1STAT |= UCLISTEN;
    else             UCB1STAT &= ~UCLISTEN;

    // select SMCLK
    UCB1CTL1 |= UCSSEL_2;

    // enable SPI
    UCB1CTL1 &= ~UCSWRST;

}

void usciB1SpiPutChar(unsigned char txByte)
{
    while (!(UCB1IFG & UCTXIFG)); // is this efficient ?
            UCB1TXBUF = txByte;
}


#pragma vector=USCI_B1_VECTOR
__interrupt void usciB1SpiIsr(void){
    switch(__even_in_range(UCB1IV,4))
    {
    case 0:break;
    case 2: //RX int
        while (!(UCB1IFG & UCTXIFG)); // is this efficient ?
                UCB1TXBUF = UCB1RXBUF;  // if TXBUFF ready then transmit a byte by writing to it
      break;
    case 4:break; //TX int
    default: break;

}
