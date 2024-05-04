/*************************************************************************************************
 * adc12.h
 * - C interface file for MSP430 ADC_12
 *
 *  Author: Greg Scutt
 *  Created on: May 1, 2018
 *  Modified: Andrew Friesen, 2024
 **************************************************************************************************/

#ifndef ADC12_H_
#define ADC12_H_

extern int adc12Result; // public global variable declarations

#define SAMPLE_ADC 1000   // delay between ADC12SC H-->L
#define EXT_MODE 1
#define PULSE_MODE 0

void adc12Cfg(const char * vref, char sampMode, char convTrigger, char adcChannel, char enInt );
void adc12SampSWConv(void);


#endif /* ADC12_H_ */
