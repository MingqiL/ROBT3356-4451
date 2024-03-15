#include <msp430.h> 
#include <Uart_lab.h>
#include <string.h>
#include <stdint.h>
#include <CmdInterpreter.h>
#include <stdio.h>


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	//__bis_SR_register(GIE);     // enable the global enable
	//unsigned char testStr[] = "BCIT MECHATRONICS\r\n";
	//static char testArr[] = "ABCDEFG";
	//unsigned char diag_TX[50];
    //unsigned char newLine[] = "\r\n";
	//int numTX;
	char rxString[BUFF_SZ];
	unsigned char errMsg[BUFF_SZ] = "ERROR!";
	int cmdIndex, xqIndex;
	
	P1DIR |= BIT2 + BIT3 + BIT4 + BIT5; // BIT2:EN, BIT3+BIT4:absPos, BIT5:Relay

	ucsiA1UartInit();
	cmdsinit();
	while(1)
	{
//	    ucsiA1UartTxString(testStr);
	  //  numTX = usciA1UartTXBuffer(testArr, 70);
	  //  sprintf(diag_TX,"%d characters transmitted", numTX);
	  //  ucsiA1UartTxString(diag_TX);
	  //  ucsiA1UartTxString(newLine);

	// time spent on sending testStr, Ts = 1/fs = 1/(B/10bit/char) * 19char = 19/(19.2k/10) = 0.00989583333s = 9.9ms
	    if (!usciA1UartGets(rxString)){
	        ucsiA1UartTxString(errMsg);// call the get Uart function
	    }

	    //char testCmd[] = "RELAY,0";
	    cmdIndex=parseCmd(rxString);
	    if (cmdIndex==-1)   ucsiA1UartTxString(errMsg);
	    else
	        xqIndex = executeCMD(cmdIndex);

	}
	    return 0;
}
