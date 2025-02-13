#include <xc.h>

void InitClock() {
    T3CONbits.TCKPS = 0b10; 
    T3CONbits.TCS = 0;
    T3CONbits.TSIDL = 0; 
    IPC2bits.T3IP = 2; 
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1; 
    PR3 = 65535 / 2;
    TMR3 = 0;
    T3CONbits.TON = 0;
}