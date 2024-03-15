#include <msp430.h> 
#include <CmdInterpreter.h>
#include <string.h>
#include <pixyCam.h>
#include <nok5110LCD.h>
#include <usciSpi.h>
#include <usciI2C.h>
#include <Uart_lab.h>
#include <CmdInterpreter.h>

/**
 * main.c
 */

char rxString[BUFF_SZ];
unsigned char i=0;

void main(void)
    {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	unsigned char sclkDiv = 12;  // sclk divisor
	unsigned char sclkMode[2] = {0,0};
	unsigned char cmdNum;


	ucsiB0I2CInit(sclkDiv);  // initiate I2C
	ucsiA1UartInit();     // initiate UART
	usciB1SpiInit(MASTER, 12, sclkMode, 0);  // initiate SPI
	cmdsinit();          // initiate cmds
	nokLcdInit();

	__bis_SR_register(GIE);
	UCB0IE |= UCNACKIE; // enable NACK

	do{
	        usciA1UartGets(rxString);
	        cmdNum = parseCmd(rxString);
	        executeCMD(cmdNum);

	    }while(1);

}

#pragma vector=USCI_B0_VECTOR
__interrupt void usciB0I2CIsr(void){
    switch(__even_in_range(UCB0IV,4))
    {
    case 0:break;
    case 2:break;   //ALIFG
    case 4:          //NACKIFG
        UCB0CTL1 |= UCTXSTP;    // send STOP if NACK
        break;
    default: break;
}
}
