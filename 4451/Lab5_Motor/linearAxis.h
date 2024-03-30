/*
 * linearAxis.h
 *
 *  Created on: 2024Äê3ÔÂ15ÈÕ
 *      Author: leeqi
 */

#ifndef LINEARAXIS_H_
#define LINEARAXIS_H_

// constant for linear axis
#define LA_EN BIT3
#define LA_RELAY BIT4

// constant for PWM setup
#define TA01 BIT2
#define CPUCLK 1048000
#define desiredFreq 1000
#define PWM CPUCLK/desiredFreq
#define DutyC_10 PWM/10
#define TIMERB_PWM 1048


// constant for motor
#define ONE_REV_PULSE 4600
#define CHA BIT0
#define CHB BIT2

void linearAxisInit();

#endif /* LINEARAXIS_H_ */
