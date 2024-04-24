#include <msp430.h>
#include <string.h>
#include <stdlib.h>  // standard functions and constant
#include <stdio.h>   // i/o functions
#include <stdint.h>
#include <math.h>    // math functions
#include <ctype.h>   // character functions
#include <stdbool.h> // bool definitions

#include <usciA1Uart.h>
#include <dma.h>
#include <adc12.h>
#include <comp.h>



/**
 * main.c
 */
/*
 * LAB6 PIN LAYOUT
 *
 * |   USCIB1SPI   |   USCIB0SPI  |  INTERRUPT  | LINEAR AXIS |      ADC12      |   COMP     |
 * |  4.1: B1SIMO  |  3.0: B0SIMO |  2.0: CHA   |  1.2: PWM   |  6.0: A0(broke) | 6.3: CB3   |
 * |  4.2: B1SOMI  |  3.1: B0SOMI |  2.2: CHB   |  1.3: EN    |  5.0: VREF+     | 6.4: CB4   |
 * |  4.3: B1SCLK  |  3.2: B0SCLK |  1.4: RELAY |             |  5.1: VREF-     | 1.6: CBOUT |
 * |  2.7: B1SS\   |  2.6: B0SS\  |             |             |  6.1: A1
 * |               |              |             |             |  6.2: A2
 */

#define ADC_SZ 600
#define SAMP_FREQ 60
#define CPU_FREQ 1048000
#define SLEEP (CPU_FREQ/SAMP_FREQ-SAMPLE_ADC)
unsigned int ADC_BUFx[ADC_SZ]={0};
unsigned int ADC_BUFy[ADC_SZ]={0};

int main(void)
{
    unsigned int i=0;
//    unsigned char UartString[100];
    unsigned char convert_flag, send_flag,send_idx=0;

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    adc12Cfg("3V3",PULSE_MODE,0,1,0); // cha1
    adc12Cfg("3V3",PULSE_MODE,0,2,0); // cha2
	configDMAx_ADC12(&ADC12MEM1, ADC_BUFx, ADC_SZ);
    configDMAy_ADC12(&ADC12MEM2, ADC_BUFy, ADC_SZ);

	ucsiA1UartInit();
	compInit();
//	UCA1IE |= UCRXIE;
//    __bis_SR_register(GIE);

	convert_flag = 1;
	send_flag = 0;
	for (i=0;i<ADC_SZ;i++)
	{
	    adc12SampSWConv();
	    if (convert_flag)
	    {
	        convert_flag = 0;
            send_flag = 1;
	    }
	    else if (send_flag)
        {
	        __delay_cycles(20);
            ucsiA1UartTxChar(ADC_BUFx[send_idx]);
            ucsiA1UartTxChar(ADC_BUFx[send_idx]>>8);
            ucsiA1UartTxChar(ADC_BUFy[send_idx]);
            ucsiA1UartTxChar(ADC_BUFy[send_idx]>>8);
            convert_flag = 1;
            send_flag = 0;
            send_idx++;
        }

        __delay_cycles(20);


        __delay_cycles(SLEEP);
	}
	while(1);

	return 0;
}



