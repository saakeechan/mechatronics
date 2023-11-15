#include "msp.h"
#include "driverlib.h"
#include <stdio.h>

volatile int PWM = -0;             // Start with a duty cycle of 0.0 %


// Configure Timer A for UpMode operation
const Timer_A_UpModeConfig upConfig_0 =
{    TIMER_A_CLOCKSOURCE_SMCLK,
     TIMER_A_CLOCKSOURCE_DIVIDER_1,
     60000,
     TIMER_A_TAIE_INTERRUPT_DISABLE,
     TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
     TIMER_A_DO_CLEAR
};


/**
 * main.c
 */
void main(void)
{
    CS_setDCOFrequency(3E+6);                       // Sets clock speed
    WDT_A_holdTimer();              // Stop watchdog timer
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Configure GPIO ports 2.4,2.6,2.7 for motor A driving
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN4|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4|GPIO_PIN7);

    ///// *** Configure Timer A and its interrupt *** /////
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig_0);
    Interrupt_enableInterrupt(INT_TA0_0);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    ///// *** Setup Pins 2.4-7 as PWM output signals *** /////
    P2->SEL0 |= 0x10 ;         // Set bit 4 of P2SEL0 to enable TA0.1 functionality on P2.4
    P2->SEL1 &= ~0x10 ;        // Clear bit 4 of P2SEL1 to enable TA0.1 functionality on P2.4
    P2->DIR |= 0x10 ;          // Set pin 2.4 as an output pin
    TIMER_A0->CCR[1] = 0 ;     // Set duty cycle on pin 2.4 to 0.0 % to start (it defaults to 0%... this is just a demo)
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7   ;    // Macro which is equal to 0x00e0, defined in msp432p4111.h

    // Infinite loop
    while(1){
        // Nothing needed here, as everything is done in the Timer A ISR
    }

}

//////////////// *** Timer A Interrupt Service Routine *** ///////////////////
void TA0_0_IRQHandler(void){

    if (PWM < 255) {
        PWM = PWM + 1;                      // Update PWM Value
    }
    // Save new PWM value to Capture Compare register
    TIMER_A0->CCR[1] = PWM;

    // Verify PWM values
    printf("PWM Value: %i \r\n\n", PWM) ;

    // Clear Timer A Interrupt Flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

