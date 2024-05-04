/*************************************************************************************************
 * adc12.c
 * - C implementation file for MSP430 ADC12
 *
 *  Author: Greg Scutt
 *  Created on: May 1,2018
 *  Modified by: Andrew Friesen, 2024
 **************************************************************************************************/

#include <msp430.h>
#include "adc12.h"
#include <string.h>


/************************************************************************************
* Function: adc12Cfg
* - configures an ADC channel for  single channel conversion with selectable trigger and vref.
* - extended or pulse mode sampling.
* -  12 bit resolution. Assumes VR- = AVSS = 0V. Enables conversion ADC12ENC = 1 and interrupt ADC12IE.
* -  does not select ADC12CLK.  Should be added
* Arguments:
* vref - REF+ reference. Send string "2V5", "1V5", "2V5", "3V3"
* sampMode - sample/hold and convert control. Sets/Clears ADC12SHP for pulse or extended mode.
* convTrigger - selects timer source ADC12SHS_1 or ADC12SC register bit.
			1 - will select the timer.  0 - will select SW Register bit	
* adcChannel - selects adcChannel 0:15 as adc input. Not implemented.
* return: none
* Author: Greg Scutt
* Date: May 1, 2018
* Modified: April 4, 2024 Andrew Friesen
*
************************************************************************************/
void adc12Cfg(const char * vref, char sampMode, char convTrigger, char adcChannel, char enInt )	{


    /*configure REF MODULE. 
	see user manual 28.2.3  and the REF Module in section 26
	Since tte MSP430F5529 has a separate REF Module, we are using it directly
	and programming the ADC12A to accept its inputs.  Make sure you understand this.*/
    if (strcmp(vref, "3V3"))	{
		ADC12MCTL0 |= (ADC12INCH_1 + ADC12SREF_1);		// select Analog Channel Input A0.  select VR+ = VREF+
		REFCTL0 |= (REFMSTR + REFON);					//  enable reference control. Use REF module registers.
		if (!strcmp(vref, "1V5")){
			REFCTL0 |= REFVSEL_0;
		}
		else if(!strcmp(vref, "2V0")){
			REFCTL0 |= REFVSEL_1;
		}
		else if(!strcmp(vref, "2V5")){
			REFCTL0 |= REFVSEL_2;
		}
    }
    else if (!strcmp(vref, "3V3"))	{				//   VR+ = AVCC+ and not VREF+
		ADC12MCTL0 |= (ADC12INCH_1 + ADC12SREF_0);	// select Analog Channel Input A0.
		REFCTL0 &= ~(REFMSTR + REFON);				//  disable REF control. Use the ADC12A to provide 3V3
    }


	// Setup Trigger and sample mode
    if(convTrigger){
    	ADC12CTL1 |= ADC12SHS_1;					// select timer. assumes timer is setup
    }
    else{
    	ADC12CTL1 |= ADC12SHS_0;					// select SW bit
    }

    if (sampMode) {
       	ADC12CTL1  |= ADC12SHP;						// extended mode. SAMPCON follows the trigger signal width
       	// SET ADC12SHT1,0 IN ADC12CTL0
    } else
       	ADC12CTL1  &= ~ADC12SHP;					// pulse sampling sampling. SAMPCON will be controlled by ADC12SHT1x, ADC12SHT10x. Bits Not implemented here.

    ADC12CTL0 |= ADC12ON;         					// ADC12 on
    ADC12CTL1 |= ADC12CONSEQ_2;      				// Reapeated Single Channel
    ADC12CTL2 |= ADC12RES_2;						// 12-Bit Resolution

    if(enInt)
    {
        ADC12IE   |= BIT0;                              // Enable interrupt
    }
    else
    {
        ADC12IE   &= ~BIT0;                              // Disable interrupt
    }

    ADC12CTL0 |= ADC12ENC;							// Enable Conversion

}

/************************************************************************************
* Function: adc12SampSWConv
* - sets then clears the ADC12SC bit after SAMPLE_ADC cycles to start an ADC12 conversion.
* - make sure the delay (Tsample) created with delay_cycles and SAMPLE_ADC,MCLK is long enough. Errors arise if it is too fast. Can you explain why?
* Arguments:
* - none.
* Author: Greg Scutt
* Date: May 1, 2018
* Modified:
************************************************************************************/
void adc12SampSWConv(void){
	ADC12CTL0 |= ADC12SC;
	__delay_cycles(SAMPLE_ADC);
	ADC12CTL0 &= (~ADC12SC);  // note: the user manual indicates this bit clears automatically?  Is this true?  Verify.
}


#pragma vector = ADC12_VECTOR
interrupt void eocADC12ISR(void)	{

	// needs to be modified for channel selection with a proper interrupt handler.
	
	// currently just stores MEM0 to global variable adc12Result. Good enough for the DC Voltmeter ...
	//adc12Result = ADC12MEM0;

}
