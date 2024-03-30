/*
 * usciB0Spi.h
 *
 *  Created on: 2024Äê3ÔÂ15ÈÕ
 *      Author: leeqi
 */

#ifndef USCIB0SPI_H_
#define USCIB0SPI_H_

#define B0CLK  BIT2
#define B0SOMI BIT1
#define B0SIMO BIT0
#define B0SS BIT6
#define MASTER 1
#define SLAVE 0

void usciB0SpiInit(unsigned char spiMST, unsigned int sclkDiv, unsigned char* sclkMode, unsigned char spiLoopBack);
void usciB0SpiPutChar(unsigned char txByte);
int usciB0SpiTxBuffer(char * buffer, int bufLen);
int usciB0SpiTxBufferSS(char * buffer, int bufLen);



#endif /* USCIB0SPI_H_ */
