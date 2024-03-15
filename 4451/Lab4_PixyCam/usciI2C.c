/*************************************************************************************************
 * ucsiUart.c
 * - C implementation or source file for MSP430 UCSI UART A1
 *
 *  Author: Greg Scutt
 *  Created on: March 1, 2017
 *  Modified: February 26th, 2018
 *
 *  Modified By: Andrew Friesen
 *  Date: December 30, 2021
 **************************************************************************************************/

#include <msp430.h>
#include <Uart_lab.h>
#include <string.h>
#include <stdint.h>
#include <CmdInterpreter.h>
#include <stdio.h>
#include <usciI2C.h>

char rxI2CBuffer[BUFF_SZ];


void ucsiB0I2CInit(unsigned char sclkDiv){

    // READ THIS --> You must use UCSI Control BITS defined in msp430.h !!
    // In fact you must use pre-defined control bits whenever possible. no obscure HEX codes allowed anymore.

    P3SEL |= SDA_B0 | SCL_B0;   // P3.0:SDA, P3.1:SCL

    UCB0CTL1 |= UCSWRST;            // USCI B0  Software Reset Enabled
    //********************

    UCB0CTL1    |=  UCSSEL_2;        // select SMCLK

    UCB0CTL0     =  0;              // RESET UCB0CTL0 before new configuration
    UCB0CTL0    |=  UCMST           // Master mode
                |   UCMODE_3        // I2C
                &  ~UCA10           // 7-bit address
                |   UCSYNC;         // synchronized mode

    // configre CLK
    UCB0BR0 = (sclkDiv&0xFF);
    UCB0BR1 = (sclkDiv>>8) & 0xFF;

    UCB0CTL1    &= ~UCSWRST;        //  configured. take state machine out of reset.
    }



int ucsiB0I2CMstTransmit(unsigned char nBytes, unsigned char *txBuffer, unsigned char slaveAddr) {
    unsigned char counter, garbage;
    UCB0CTL1 |= UCTR;           // config as transmitter
    UCB0I2CSA = slaveAddr;      // assign slave addr
    UCB0CTL1 |= UCTXSTT;        // generate START

    for(counter=0;counter<=nBytes;counter++)
    {
        if(!(UCB0IFG & UCNACKIFG))  // if not NACK
        {
            while(!(UCB0IFG & UCTXIFG)) // polling UCTXIFG
            {
                if((UCB0IFG & UCNACKIFG) || (UCB0IFG & UCALIFG))
                {
                    return 1;  // if NACK or Arbitration happened
                }
            }
            UCB0TXBUF = txBuffer[counter];
        }
        else
        {
            return 1; // if NACK
        }

    }

    UCB0CTL1 |= UCTXSTP;    // send STOP
    while (UCB0IFG & UCSTPIFG); // polling stop flag


    garbage = UCB0RXBUF;    // read and clear RXIFG


    return 0;  // transmitted succesfully

}

int ucsiB0I2CMstReceive(unsigned char nBytes, unsigned char *rxBuffer, unsigned char slaveAddr)
{
    unsigned char counter;

    UCB0CTL1 &= ~UCTR;           // config as receiver
    UCB0I2CSA = slaveAddr;      // assign slave addr
    UCB0CTL1 |= UCTXSTT;        // generate START

    for(counter=0;counter<=nBytes;counter++)
    {
        while(!(UCB0IFG & UCRXIFG)) // polling UCRXIFG
        {
            if((UCB0IFG & UCNACKIFG) || (UCB0IFG & UCALIFG))
                return 1;  // if NACK or Arbitration happened

        }
        rxBuffer[counter] = UCB0RXBUF;  // read the RXBUF
    }

    UCB0CTL1 |= UCTXNACK;   // nack the slave
    UCB0CTL1 |= UCTXSTP;    // send STOP
    while (UCB0IFG & UCSTPIFG); // polling stop flag

    return 0;  // received successfully
}


