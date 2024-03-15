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
#include <pixyCam.h>
#include <usciI2C.h>


char pixyGetVersion(unsigned char slvAddr, unsigned char * rxBuffer)
{
    unsigned char sendPack[4] = {0xae,0xc1,0x0e,0x00};
    unsigned static char numByte  = 18;

    if(!(ucsiB0I2CMstTransmit(sizeof(sendPack), sendPack, slvAddr))) // if transmit return 0
    {
        if(!(ucsiB0I2CMstReceive(numByte, rxBuffer, slvAddr)))     // if receive return 0
            return 0;   // return 0
    }
    return 1;
}

char pixyGetVector(unsigned char slvAddr, unsigned char * rxBuffer)
{
    unsigned char sendPack[] = {0xae,0xc1,48,2,MAIN_FEATURES, VECTORS};
    unsigned char numByteRx = 12;

    if(!(ucsiB0I2CMstTransmit(sizeof(sendPack), sendPack, slvAddr))) // if transmit return counter
    {
        if(!(ucsiB0I2CMstReceive(numByteRx, rxBuffer, slvAddr)))     // if receive return counter
            return 0;   // return 0
    }
    return 1;

}






