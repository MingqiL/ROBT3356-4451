#include <msp430.h> 
#include <interface.h>
#include <Uart_lab.h>
#include <CmdInterpreter.h>
#include <string.h>

/**
 * main.c
 */

char rxString[BUFF_SZ];
char rxSpiBuff[BUFF_SZ];
unsigned char i=0;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	unsigned char sclkMode[2] = {0,0};  // sclkMode<1> = UCCKPH, sclkMode<2> = UCCKPL
	int buffLen;

	usciB1SpiInit(MASTER, 3, sclkMode, 1);  // initiate SPI
	ucsiA1UartInit();     // initiate UART

	__bis_SR_register(GIE);
	UCB1IE |= UCRXIE;
	//       | UCTXIE;
	//UCA1IE |= UCRXIE;
	//       | UCTXIE;

	//P1DIR |= BIT0; // config LED1
	do{
//	    P1OUT |= BIT0;
//	    __delay_cycles(50000);
//	    P1OUT &= ~BIT0;
//	    __delay_cycles(50000);

	    usciA1UartGets(rxString);
	    buffLen = strlen(rxString);
	    //usciB1SpiTxBuffer(rxString,buffLen-1);
	    usciB1SpiTxBufferSS(rxString,buffLen-1);

	}while(1);


	return 0;
}


#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
  switch(__even_in_range(UCA1IV,4))
  {
  case 0:break;
  case 2: //RX int
      while (!(UCA1IFG & UCTXIFG)); // is this efficient ?
              //UCA1TXBUF = UCA1RXBUF;  // if TXBUFF ready then transmit a byte by writing to it
              usciB1SpiPutChar(UCA1RXBUF);   // write UART RXBUF to SPI TXBUF
              //UCB1TXBUF = UCA1RXBUF;

    break;
  case 4:break; //TX int
  default: break;
  }
}


#pragma vector=USCI_B1_VECTOR
__interrupt void usciB1SpiIsr(void){
    switch(__even_in_range(UCB1IV,4))
    {
    case 0:break;
    case 2: //RX int
        rxSpiBuff[i++] = UCB1RXBUF;
        if (i == BUFF_SZ) i=0;
      break;
    case 4:break; //TX int
    default: break;
    }
}
