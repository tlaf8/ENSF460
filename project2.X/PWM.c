#include <xc.h>
#include "PWM.h"

void PWMinit(uint16_t target) {
    duty_cycle = 0;
    tick_count = 0;
    T2CON = 0;         
    TMR2 = 0;         
    PR2 = target;
    T2CONbits.TCKPS = 0b00;  
    T2CONbits.TCS = 0;         
    IPC1bits.T2IP = 4;     
    IFS0bits.T2IF = 0;    
    IEC0bits.T2IE = 1;  
    T2CONbits.TON = 1;
}

void set_duty_cycle(uint16_t duty) {
    if (duty <= PWM_MAX) {
        duty_cycle = (uint16_t) (duty * PWM_PERIOD / (PWM_MAX + 1));
    }
}
