#include <msp430.h> 
#include <CmdInterpreter.h>
#include <string.h>
#include <stdio.h>
#include <nok5110LCD.h>
#include <LS7366R.h>
#include <linearAxis.h>
#include <usciA1Uart.h>
#include <usciB1Spi.h>
#include <usciB0Spi.h>


/**
 * LAB5 PIN LAYOUT
 *
 * |   USCIB1SPI   |   USCIB0SPI  |  INTERRUPT  | LINEAR AXIS |   LCD SCREEN
 * |  4.1: B1SIMO  |  3.0: B0SIMO |  2.0: CHA   |  1.2: PWM   |   P6.0: POW
 * |  4.2: B1SOMI  |  3.1: B0SOMI |  2.2: CHB   |  1.3: EN    |   P6.1: LED
 * |  4.3: B1SCLK  |  3.2: B0SCLK |  1.4: RELAY |             |   P6.2: D/C
 * |  2.7: B1SS\   |  2.6: B0SS\  |             |             |   P6.3: RST
 * |               |              |             |             |   P6.4: SCE
 */


char rxString[BUFF_SZ];
int pwmScale=2;
unsigned char i=0, j=0;
unsigned char rxFromUart = 0;


void main(void)
    {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	unsigned const char sclkDiv = 1;  // sclk divisor
	unsigned char sclkMode[2] = {0,1};
	unsigned char cmdNum;
	float axisPos_float = 0;


	ucsiA1UartInit();     // initiate UART
    usciB0SpiInit(MASTER, sclkDiv, sclkMode, 0);  // initiate SPI
//	usciB1SpiInit(MASTER, sclkDiv, sclkMode, 0);  // initiate SPI
	cmdsinit();          // initiate cmds
	nokLcdInit();
//	initLS7366R();      // initiate LS7366R
	nokLcdDrawScrnLine(0,0,1);
    nokLcdDrawScrnLine(LCD_MAX_COL-1,0,1);
    nokLcdDrawScrnLine(0,0.5*LCD_MAX_ROW,0);

	P2DIR &= ~ (CHA + CHB);
	// configure CHA and CHB as input ports
	P2REN |= CHA + CHB;         // enable pulling resistor
	P2OUT &= ~ (CHA + CHB);     // pull-down resistor
    P2IE |= CHA + CHB;          // enable interrupt
    P2IES &= ~ (CHA + CHB);     // configure low-to-high trigger edge of A
    P2IFG &= ~ (CHA + CHB);     // clear flags in port 2

    UCA1IE |= UCRXIE;


    linearAxisInit();   // initiate the linear axis


	__bis_SR_register(GIE);

//	while(1){
//	    unsigned char encMsg[50];
//	    sprintf(encMsg, "ENCODER COUNT: %d", encoderCountMSP);
//	    ucsiA1UartTxString(encMsg);
//	}
//	TA0CCR1 = TA0CCR0/10;

	encoderCountMSP = 0;
	do{
	        //usciA1UartGets(rxString);



	    if (rxFromUart)
	    {
	        if (UCA1RXBUF == '\n'){
                cmdNum = parseCmd(rxString);
                executeCMD(cmdNum);
                if (cmdNum == 11) // rescale case
                    pwmScale = getPWMScale();
	        }
            rxFromUart = 0;
	    }

	    if (j>20){
	        axisPos_float = ((float)encoderCountMSP) /  (4600 * (float)pwmScale);
	        TA0CCR1 = (int)((PWM) * axisPos_float);
	        updateLCD(axisPos_float);
	        j = 0;
	    }

	    }while(1);

}


#pragma vector = PORT2_VECTOR
__interrupt void Port2_ISR1 (void){
    switch(__even_in_range(P2IV,4))
    {
    case 0:break;
    case 2: // A
        if ((P2IES&CHA))
        {
            if (P2IN&CHB)   encoderCountMSP++;
            else            encoderCountMSP--;
        }
        else
        {
            if (P2IN&CHB)   encoderCountMSP--;
            else            encoderCountMSP++;
        }
        P2IFG &= ~ CHA;
        P2IES ^= CHA;
        break;
    case 6: // B
        if ((P2IES&CHB))
        {
            if (P2IN&CHA)   encoderCountMSP--;
            else            encoderCountMSP++;
        }
        else
        {
            if (P2IN&CHA)   encoderCountMSP++;
            else            encoderCountMSP--;
        }
        P2IFG &= ~ CHB;
        P2IES ^= CHB;
        break;
    default: break;
}
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void Timer0_B1_ISR (void){
    switch(__even_in_range(TB0IV,14)){
    case 0:break;
    case 2:break;
    case 4:break;
    case 6:break;
    case 8:break;
    case 10:break;
    case 12:break;
    case 14:    //TAIFG
        j++;
        TB0CTL &= ~TBIFG;
        break;
    default:break;
    }

}

#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void){
    switch(__even_in_range(UCA1IV,4))
    {
    case 0: break;
    case 2:         // RX Interrupt
    {
        rxString[i] = UCA1RXBUF;// copy the received char into rxBuffer
        i++;
        rxFromUart = 1;
        if((UCA1RXBUF == '\n')) i = 0;
        break;
    }
    case 4: break;
}
}
