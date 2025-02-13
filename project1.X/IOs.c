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
    
    // Enable button interrupts
    CNEN1bits.CN0IE = 1;
    CNEN1bits.CN1IE = 1;    
    CNEN2bits.CN30IE = 1;
    
    // Allow interrupts
    IPC4bits.CNIP = 6;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

void IOcheck() {
    uint16_t PB1 = !PORTAbits.RA2;
    uint16_t PB2 = !PORTBbits.RB4;
    uint16_t PB3 = !PORTAbits.RA4;
    
    if (PB1 + PB2 + PB3 > 1) {
        LATBbits.LATB8 = 1;
    } else if (PB1) {
        LATBbits.LATB8 = 1;
        delay_ms(500);
        LATBbits.LATB8 = 0;
        delay_ms(500);
    } else if (PB2) {
        LATBbits.LATB8 = 1;
        delay_ms(1000);
        LATBbits.LATB8 = 0;
        delay_ms(1000);
    } else if (PB3) {
        LATBbits.LATB8 = 1;
        delay_ms(4000);
        LATBbits.LATB8 = 0;
        delay_ms(4000);
    } else {
        LATBbits.LATB8 = 0;
    }
}