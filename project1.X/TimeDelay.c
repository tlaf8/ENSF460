#include <xc.h>
#include "TimeDelay.h"


void delay_ms(uint16_t time_ms) {
    PR2 = 3906.25 / 1000 * time_ms;                 // set the count value
    TMR2 = 0;
    
    //T2CON config
    T2CONbits.TCKPS = 2;        // set prescaler
    T2CONbits.TCS = 0;          // use internal clock
    T2CONbits.TSIDL = 0;        //operate in idle mode
    
    // Timer 2 interrupt config
    IPC1bits.T2IP = 2;          //7 is highest and 1 is lowest pri.
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;          //enable timer interrupt
    T2CONbits.TON = 1;
    Idle(); // Can this change to while loop while interrupt flag zero?
}