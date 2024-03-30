/*
 * linearAxis.c
 *
 *  Created on: 2024Äê3ÔÂ15ÈÕ
 *      Author: leeqi
 */
#include <msp430.h>
#include <CmdInterpreter.h>
#include <linearAxis.h>

void linearAxisInit()
{
    P1DIR |= TA01 + LA_EN + LA_RELAY; // BIT2: PWM, BIT3:EN, BIT4:Relay
    P1OUT |= LA_RELAY   // bring relay high and not changed
          |  LA_EN;// disable the linear axis


    P1SEL |= TA01;

    TA0CTL |= TASSEL_2 + ID_0 + MC_1;    // initiate TA0.1
    TA0CTL &= ~TAIFG;                           // clear TA Flag

    TA0CCTL1 |= OUTMOD_3;                      // use reset/set mode for cctl

    TA0CCR0 = PWM-1;                             // 10^6/10^3

    /*************************************************************************/

    TB0CTL |= TASSEL_2 + ID_0 + MC_1 + TBIE;    // initiate TA0.1
    TB0CTL &= ~TBIFG;                           // clear TA Flag

    TB0CCTL1 |= OUTMOD_3;                      // use reset/set mode for cctl

    TB0CCR0 = TIMERB_PWM;                             // 10^6/10^3
}


