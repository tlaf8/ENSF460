#include "IOs.h"
#include <xc.h>

void IOinit() {
    TRISBbits.TRISB8 = 0;
    LATBbits.LATB8 = 1;

    TRISAbits.TRISA4 = 1;
    CNPU1bits.CN0PUE = 1;
    CNEN1bits.CN0IE = 1;

    TRISBbits.TRISB4 = 1;
    CNPU1bits.CN1PUE = 1;
    CNEN1bits.CN1IE = 1;

    TRISAbits.TRISA2 = 1;
    CNPU2bits.CN30PUE = 1;
    CNEN2bits.CN30IE = 1;

    IPC4bits.CNIP = 6;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

void IOcheck(uint16_t *PB1, uint16_t *PB2, uint16_t *PB3) {
    *PB1 = !PORTAbits.RA2;
    *PB2 = !PORTBbits.RB4;
    *PB3 = !PORTAbits.RA4;
}