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

char rxBuffer[BUFF_SZ];




/************************************************************************************
* Function: ucsiA1UartInit
* - configures UCA1 UART to use SMCLK, no parity, 8 bit data, LSB first, one stop bit
*  BAUD rate = 19.2Kbps with 16xoversampling.
*  assumes SMCLK = 2^20 Hz.
* argument:
* Arguments: none, but baud rate would be useful
*
* return: none
* Author: Greg Scutt
* Date: March 1st, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void ucsiA1UartInit(){

    // READ THIS --> You must use UCSI Control BITS defined in msp430.h !!
    // In fact you must use pre-defined control bits whenever possible. no obscure HEX codes allowed anymore.

    P4SEL |= TXD_A1 | RXD_A1;

    UCA1CTL1 |= UCSWRST;            // USCI A1  Software Reset Enabled
    //********************

    UCA1CTL1    |=  UCSSEL_2;       // select SMCLK. User is responsible for setting this rate.

    UCA1CTL0     =  0;              // RESET UCA1CTL0 before new configuration
    UCA1CTL0    &=  ~UCPEN          // No Parity
                &   ~UCMSB          // LSB First
                &   ~UC7BIT         // 8 bits of data
                &   ~UCSPB          // 1 stop bit
                &   ~UCSYNC;        // UART Mode


    UCA1BR1 = 0x0;// calculate and confirm with table
    UCA1BR0 = 0x3;// calculate and confirm with table
    UCA1MCTL = UCBRS_1 | UCBRF_6 | UCOS16;  // calculate and confirm with table.

    UCA1CTL1    &= ~UCSWRST;        //  configured. take state machine out of reset.
    }


/************************************************************************************
* Function: ucsiA1UartTxChar
* - writes a single character to UCA1TXBUF if it is ready
* argument:
* Arguments: txChar - byte to be transmitted
*
* return: none
* Author: Greg Scutt
* Date: March 1st, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void ucsiA1UartTxChar(unsigned char txChar) {

    while (!(UCA1IFG & UCTXIFG)); // is this efficient ?
        UCA1TXBUF = txChar;  // if TXBUFF ready then transmit a byte by writing to it
}


/************************************************************************************
* Function: ucsiA1UartTxString
* - writes a C string of characters, one char at a time to UCA1TXBUF by calling
*   ucsiA1UartTxChar. Stops when it encounters  the NULL character in the string
*   does NOT transmit the NULL character
* argument:
* Arguments: txChar - pointer to char (string) to be transmitted
*
* return: number of characters transmitted
* Author: Greg Scutt
* Date: March 1st, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int ucsiA1UartTxString(unsigned char *txChar){

    // complete this function
    unsigned char i;

    // while not a null character
    for (i=0;txChar[i]!='\0';i++)
        ucsiA1UartTxChar(txChar[i]);
    // transmit a character with ucsiA1UartTxChar and increment the pointer

    ucsiA1UartTxChar('\r');
    ucsiA1UartTxChar('\n');
    return i;
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
int usciA1UartTXBuffer(char * buffer, int bufLen)
{
    // complete this function
    unsigned char i;
    unsigned char newLine[] = "\r\n";
    // while not a null character
    for (i=0;(buffer[i]!='\0')&&(i<bufLen);i++)
        ucsiA1UartTxChar(buffer[i]);
    ucsiA1UartTxString(newLine);
    return (int)i;
}

/*
 * Name: usciA1UartGets
 *
 * Function:
 * Gets a string from the USCIA1 UART. Copy from the receive buffer until a return character is sent then return the string.
 *
 * Inputs: [char *] string pointer
 * Returns: [char *] string
 */
char * usciA1UartGets(char * rxString)
{
    unsigned char i = 0;

    do{
        while (!(UCRXIFG&UCA1IFG)); // polling the UCRXIFG
        rxBuffer[i] = UCA1RXBUF;    // copy the received char into rxBuffer
        i++;
        ucsiA1UartTxChar(UCA1RXBUF); // echo the received char
    }while ((UCA1RXBUF != '\n')&&(i<BUFF_SZ));// while user hasn't press enter

    rxBuffer[i]='\0';           // insert null character at end of the string
    strcpy(rxString,rxBuffer);  // copy the buffer string to rx string

    return rxString;
}




#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
  switch(__even_in_range(UCA1IV,4))
  {
  case 0:break;
  case 2: //RX int
      while (!(UCA1IFG & UCTXIFG)); // is this efficient ?
              UCA1TXBUF = UCA1RXBUF;  // if TXBUFF ready then transmit a byte by writing to it
    break;
  case 4:break; //TX int
  default: break;
  }
}


