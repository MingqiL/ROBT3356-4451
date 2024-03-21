#define TA01 BIT2
#define DRIVEDIR BIT3
#define BRAKE BIT4
#define CPUCLK 1048000
#define desiredFreq 1000
#define PWM CPUCLK/desiredFreq
#define DutyC_0 0
#define DutyC_10 PWM*1/10
#define DutyC_20 PWM*2/10
#define DutyC_30 PWM*3/10
#define DutyC_40 PWM*4/10
#define DutyC_50 PWM*5/10
#define DutyC_60 PWM*6/10
#define DutyC_70 PWM*7/10
#define DutyC_80 PWM*8/10
#define DutyC_90 PWM*9/10
#define DutyC_100 PWM


typedef struct HBRIDGE_CTRL{
    unsigned char systemCtrlStat[3]; // pwmGenerator system status <B2:B0>
    /*  B0 bridge power (S).    1 = PWM ON at dutyCycle, 0 = OFF
     *  B1 bridge DIR (D).      1 = CW, 0 = CCW
     *  B2 brake ON/OFF (B).    1 = ON, 0 = OFF
     */
    unsigned char dutyCycle; // current pwm duty cycle in %
    unsigned char pwmCCR;    // copy of TIMERA CCR register value
}HBRIDGE_CTRL;

void hBridgeInit(HBRIDGE_CTRL* myBridge);
void updateBridge(HBRIDGE_CTRL* myBridge, KEYPAD_4X4* keypad);
void updatePWM(HBRIDGE_CTRL* myBridge, volatile unsigned int prevDuty, unsigned int targetPWM);
