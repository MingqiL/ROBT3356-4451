/*************************************************************************************************
 * ucsiUart.h
 * - - C interface file for MSP430 UCSI UART A1, A0
 *
 *  Author: Greg Scutt
 *  Created on: March 1, 2017
 *  Modified: Feb 19, 2018
 **************************************************************************************************/

#ifndef I2C_H_
#define I2C_H_

#define     BUFF_SZ         100
#define SDA_B0  BIT0
#define SCL_B0  BIT1



void ucsiB0I2CInit(unsigned char sclkDiv);

int ucsiB0I2CMstTransmit(unsigned char nBytes, unsigned char *txBuffer, unsigned char slaveAddr);

int ucsiB0I2CMstReceive(unsigned char nBytes, unsigned char *txBuffer, unsigned char slaveAddr);

#endif /* I2C_H_ */
