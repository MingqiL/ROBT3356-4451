/*************************************************************************************************
 * ucsiUart.h
 * - - C interface file for MSP430 UCSI UART A1, A0
 *
 *  Author: Greg Scutt
 *  Created on: March 1, 2017
 *  Modified: Feb 19, 2018
 **************************************************************************************************/

#ifndef USCIA1UART_H_
#define USCIA1UART_H_

#define     TXD_A1          BIT4            //Transmit Data on P4.4
#define     RXD_A1          BIT5            //Recieve Data on P4.5
#define     _UART_A1PSEL    P4SEL |= PM_UCA1TXD | PM_UCA1RXD // use macros for short expressions.
#define     BUFF_SZ         100



void ucsiA1UartInit();

void ucsiA1UartTxChar(unsigned char txChar);

int ucsiA1UartTxString(unsigned char* txChar);

char * usciA1UartGets(char * rxString);

int usciA1UartTXBuffer(char * buffer, int bufLen);

#endif /* USCIA1UART_H_ */
