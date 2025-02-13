#include <xc.h>
#include "IOs.h"

void IOinit() {
    TRISBbits.TRISB8 = 0;
    
    // PB and LED config
    TRISBbits.TRISB8 = 0;       // LED
    TRISAbits.TRISA2 = 1;       // PB1
    CNPU2bits.CN30PUE = 1;      // PB1 Pullup Enable
    TRISBbits.TRISB4 = 1;       // PB2 
    CNPU1bits.CN1PUE = 1;       // PB2 Pullup Enable
    TRISAbits.TRISA4 = 1;       // PB3
    CNPU1bits.CN0PUE = 1;       // PB3 Pullup Enable
}

void IOcheck() {
    if (PORTAbits.RA2 == 0 && PORTBbits.RB4 == 0) {
        LATBbits.LATB8 = 1;
        delay_ms(1);
        LATBbits.LATB8 = 0;
        delay_ms(1);
    } else if(PORTAbits.RA2 == 0) {
        LATBbits.LATB8 = 1;
        delay_ms(500);
        LATBbits.LATB8 = 0;
        delay_ms(500);
    } else if(PORTBbits.RB4 == 0) {
        LATBbits.LATB8 = 1;
        delay_ms(1000);
        LATBbits.LATB8 = 0;
        delay_ms(1000);   
    } else if(PORTAbits.RA4 == 0) {
        LATBbits.LATB8 = 1;
        delay_ms(4000);
        LATBbits.LATB8 = 0;
        delay_ms(4000);
    } else {
        LATBbits.LATB8 = 0;
    }
}