/*
 * hBridgeCtr.c
 *
 *  Created on: 2023Äê11ÔÂ30ÈÕ
 *      Author: leeqi
 */

#include <msp430.h>
#include "keypad.h"
#include "hBridgeCtr.h"

void hBridgeInit(HBRIDGE_CTRL* myBridge){
    P1DIR |= TA01 + DRIVEDIR + BRAKE;
    P1SEL |= TA01;

    TA0CTL |= TASSEL_2 + ID_0 + MC_1 + TACLR;    // initiate TA0.1
    TA0CTL &= ~TAIFG;                           // clear TA Flag

    TA0CCTL1 |= OUTMOD_7;                      // use reset/set mode for cctl

    TA0CCR0 = PWM-1;                             // 10^6/10^3

    myBridge[0].dutyCycle = 0;
    myBridge[0].pwmCCR = 0;
    myBridge[0].systemCtrlStat[0]=0;
    myBridge[0].systemCtrlStat[1]=0;
    myBridge[0].systemCtrlStat[2]=0;



}
void updateBridge(HBRIDGE_CTRL* myBridge, KEYPAD_4X4* keypad){

    static int prevDuty;
    static unsigned int curDuty;

    switch(keypad[0].currentKey){
    case '1':       // key 1        ---->> case 2,3,4,5,6,7,8,9,0 have same function as case 1
        myBridge[0].dutyCycle = 10;             // update duty cycle
        myBridge[0].systemCtrlStat[0] = 1;      // update system stat
        updatePWM(myBridge,prevDuty,DutyC_10);  // update PWM
        curDuty = DutyC_10;                     // record current duty cycle
        break;
    case '2':       // key 2
        myBridge[0].dutyCycle = 20;
        myBridge[0].systemCtrlStat[0] = 1;
        updatePWM(myBridge,prevDuty,DutyC_20);
        curDuty = DutyC_20;
        break;
    case '3':       // key 3
        myBridge[0].dutyCycle = 30;
        myBridge[0].systemCtrlStat[0] = 1;
        updatePWM(myBridge,prevDuty,DutyC_30);
        curDuty = DutyC_30;
        break;
    case '4':       // key 4
        myBridge[0].dutyCycle = 40;
        myBridge[0].systemCtrlStat[0] = 1;
        updatePWM(myBridge,prevDuty,DutyC_40);
        curDuty = DutyC_40;
        break;
    case '5':       // key 5
        myBridge[0].dutyCycle = 50;
        myBridge[0].systemCtrlStat[0] = 1;
        updatePWM(myBridge,prevDuty,DutyC_50);
        curDuty = DutyC_50;
        break;
    case '6':       // key 6
        myBridge[0].dutyCycle = 60;
        myBridge[0].systemCtrlStat[0] = 1;
        updatePWM(myBridge,prevDuty,DutyC_60);
        curDuty = DutyC_60;
        break;
    case '7':       // key 7
        myBridge[0].dutyCycle = 70;
        myBridge[0].systemCtrlStat[0] = 1;
        updatePWM(myBridge,prevDuty,DutyC_70);
        curDuty = DutyC_70;
        break;
    case '8':       // key 8
        myBridge[0].dutyCycle = 80;
        myBridge[0].systemCtrlStat[0] = 1;
        updatePWM(myBridge,prevDuty,DutyC_80);
        curDuty = DutyC_80;
        break;
    case '9':       // key 9
        myBridge[0].dutyCycle = 90;
        myBridge[0].systemCtrlStat[0] = 1;
        updatePWM(myBridge,prevDuty,DutyC_90);
        curDuty = DutyC_90;
        break;
    case '0':       // key 0
        myBridge[0].dutyCycle = 100;
        myBridge[0].systemCtrlStat[0] = 1;
        updatePWM(myBridge,prevDuty,DutyC_100);
        curDuty = DutyC_100;
        break;
    case 'A':       // key A
        if(myBridge[0].systemCtrlStat[2]) break;
        if(!myBridge[0].systemCtrlStat[1]){  // forward key
//            updatePWM(myBridge,prevDuty,DutyC_0);
            P1OUT |= BRAKE;                     // apply dynamic break
            __delay_cycles(50000);              // give enough time for break
            TA0CCR1 = 0;                        // reset PWM as 0%
            P1OUT |= DRIVEDIR;                  // set direction to forward
            updatePWM(myBridge,0,curDuty);      // update PWM and ramp up the motor
            myBridge[0].systemCtrlStat[1] = DRIVEDIR;   // update system stat
    }    break;
    case 'B':       // key B    ------>>> case B has function as case A
        if(myBridge[0].systemCtrlStat[2]) break;
        if(myBridge[0].systemCtrlStat[1]){
            P1OUT |= BRAKE;
            __delay_cycles(50000);
            TA0CCR1 = 0;
//            updatePWM(myBridge,prevDuty,DutyC_0);
            P1OUT &= ~DRIVEDIR;
            updatePWM(myBridge,0,curDuty);
            myBridge[0].systemCtrlStat[1] = !DRIVEDIR;
    }    break;
    case 'C':       // key C
        P1OUT |= BRAKE;     // apply dynamic break
        TA0CCR1 = 0;        // reset to 0% PWM
        myBridge[0].systemCtrlStat[2] = STOP;   // update system stat
        myBridge[0].dutyCycle = 0;              // update duty cycle
        break;
    default:
        break;
    }
//    if (myBridge[0].systemCtrlStat[2]) {
//        prevDuty = 0;
//        TA0CCR1 = 0;
//    }

    // update TIMERA CCR1
    myBridge[0].pwmCCR = TA0CCR1;

    // update current dutycycle
    prevDuty = myBridge[0].dutyCycle;
    //updatePWM(myBridge);

}
void updatePWM(HBRIDGE_CTRL* myBridge, volatile unsigned int prevDuty, unsigned int targetPWM){
    volatile char dutyDiff,count;
    unsigned const int mDiff = DutyC_10; // increment of M for 10% duty cycle difference
    P1OUT &= ~BRAKE;                    // release the brake

    if (prevDuty==myBridge[0].dutyCycle) dutyDiff = prevDuty;       // if same prevDuty = current duty cycle, means change direction
    else dutyDiff = abs((int)myBridge[0].dutyCycle-(int)prevDuty);  // calculate the duty cycle difference to determine necessary for ramp
    if ((dutyDiff>=30)){    // if >=30% difference in duty cycle, ramp
        if((myBridge[0].dutyCycle>prevDuty) || (myBridge[0].dutyCycle==0)){
                   for (count=dutyDiff/10;count>1;count--){
                       TA0CCR1 += mDiff;
                       __delay_cycles(CPUCLK/2);
                   }
               }
        else{
           for (count=dutyDiff/10;count>1;count--){
               TA0CCR1 -= mDiff;
               __delay_cycles(CPUCLK/2);
           }
        }
    }
    TA0CCR1 = targetPWM;    // update CCR1 to target PWM

    myBridge[0].systemCtrlStat[2] = !STOP;  // update system stat
}


