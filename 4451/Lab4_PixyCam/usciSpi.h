/*
 * interface.h
 *
 *  Created on: 2024Äê1ÔÂ22ÈÕ
 *      Author: leeqi
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_
#define B1CLK  BIT3
#define B1SOMI BIT2
#define B1SIMO BIT1
#define SS BIT0
#define MASTER 1
#define SLAVE 0



#endif /* INTERFACE_H_ */

void usciB1SpiInit(unsigned char spiMST, unsigned int sclkDiv, unsigned char* sclkMode, unsigned char spiLoopBack);
void usciB1SpiPutChar(unsigned char txByte);
int usciB1SpiTxBuffer(char * buffer, int bufLen);
int usciB1SpiTxBufferSS(char * buffer, int bufLen);




