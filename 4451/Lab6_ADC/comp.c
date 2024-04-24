/*
 * comp.c
 *
 *  Created on: Apr. 4, 2024
 *      Author: andrew_friesen
 */

#include <msp430.h>
#include "comp.h"


void compInit(void)
{
    CBCTL0 |= CBIMEN | CBIPEN | CBIPSEL_3 | CBIMSEL_4; //Enable both the position and negative inputs of the comparator (CBIMEN and CBIPEN)
                                                        //Select CB3 as the positive terminal input and CB4 as the negative input
    CBCTL1 |= CBF | CBON; //Enable the filter for CBOUT (CBF) and turn on the comparator (CBON)

    //Configure pin 1.6 as CBOUT
    P1DIR |= BIT6;
    P1SEL |= BIT6;
}
