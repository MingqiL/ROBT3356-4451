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



/**
 * main.c
 */



#define CPU_CLK 1048000
#define SAMP_CLK 10000
#define ADC_SZ 1000

unsigned int ADC_BUF[ADC_SZ]={0};

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer


    char UartString[50]={0};
    const char convTrigger = 1;
    const char sampMode = PULSE_MODE;
    const char adcChannel = 1;
    const char enInt = 0;
    volatile int M_PWM = 0;
    volatile const int M_samp = CPU_CLK/SAMP_CLK;
    volatile unsigned int i=0;
    volatile unsigned int count = 0;
    volatile int max_idx=0, min_idx=0;
    volatile unsigned int v_mid = 0;
    volatile int max_read = 0, min_read = 0;
    volatile float f,dc;
    volatile int check = 1;
    volatile double max_bit = 4095;



    adc12Cfg("2V5",sampMode,convTrigger,adcChannel,enInt); // cha1
    configDMAx_ADC12(&ADC12MEM0, ADC_BUF, ADC_SZ);
    P6SEL |= BIT1;

	ucsiA1UartInit();


	// Sample timer
    TA0CTL |= TASSEL_2 + ID_0 + MC_1;            // initiate TB0.1
    TA0CTL &= ~TAIFG;                            // clear TA Flag
    TA0CCTL1 |= OUTMOD_7;                        // use reset/set mode for cctl
    TA0CCR0 = M_samp;
    TA0CCR1 = 1;

    //  PWM output
    TA1CTL |= TASSEL_2 + ID_0 + MC_1;           // initiate TA0.1
    TA1CTL &= ~TAIFG;                           // clear TA Flag
    TA1CCTL1 |= OUTMOD_7;                       // use reset/set mode for cctl
    TA1CCR0 = 1047;
    TA1CCR1 = M_PWM;

    P2SEL |= BIT0;  // output PWM pin
    P2DIR |= BIT0;


//	UCA1IE |= UCRXIE;


//	for (i=0;i<ADC_SZ;i++)
//	{
//
//            ucsiA1UartTxChar(ADC_BUFx[send_idx]);
//            ucsiA1UartTxChar(ADC_BUFx[send_idx]>>8);
//            ucsiA1UartTxChar(ADC_BUFy[send_idx]);
//            ucsiA1UartTxChar(ADC_BUFy[send_idx]>>8);
//
//        __delay_cycles(SLEEP);
//	}


    while((DMA0CTL&DMAEN));

    // get max and min index
    for(i=check;count<1;i++)
    {
        if (ADC_BUF[i-check]<=ADC_BUF[i])
        {
            if(ADC_BUF[i]>ADC_BUF[i+check])
            {
                max_idx = i;
                count++;
                max_read = ADC_BUF[i];
            }
        }
    }

    count = 0;
    for(i=check;count<1;i++)
    {
        if(ADC_BUF[i-check]>=ADC_BUF[i])
        {
            if(ADC_BUF[i]<ADC_BUF[i+check])
            {
                min_idx = i;
                count++;
                min_read = ADC_BUF[i];
            }
        }

    }

    f = SAMP_CLK / (2*abs(max_idx-min_idx));
    sprintf(UartString,"Freq is: %f \r\n", f);
    ucsiA1UartTxString(UartString);

    v_mid = (min_read+max_read)/2;
    dc = (max_read - min_read) / max_bit;
    TA1CCR0 = (int)(CPU_CLK / f);
    TA1CCR1 = (int)(dc * TA1CCR0);

    while(1);

    __bis_SR_register(GIE);
	return 0;
}



#pragma vector = TIMER0_B1_VECTOR
__interrupt void Timer0_B1_ISR (void){
        TB0CTL &= ~TBIFG;
    }

