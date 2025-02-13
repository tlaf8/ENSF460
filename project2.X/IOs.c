#include <xc.h>
#include "IOs.h"

void IOinit() {
    TRISBbits.TRISB8 = 0;
    LATBbits.LATB8 = 0;

    TRISAbits.TRISA4 = 1;
    CNPU1bits.CN0PUE = 1;
    CNPD1bits.CN0PDE = 0;
    CNEN1bits.CN0IE = 1;

    TRISBbits.TRISB4 = 1;
    CNPU1bits.CN1PUE = 1;
    CNPD1bits.CN1PDE = 0;
    CNEN1bits.CN1IE = 1;

    TRISBbits.TRISB2 = 1;
    CNPU1bits.CN6PUE = 1;
    CNPD1bits.CN6PDE = 0;
    CNEN1bits.CN6IE = 1;

    IPC4bits.CNIP = 6;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

void IOcheck(uint16_t *PB1_event, uint16_t *PB2_event, uint16_t *PB3_event) {
    static uint16_t PB1_prev_state = 1;
    static uint16_t PB2_prev_state = 1;
    static uint16_t PB3_prev_state = 1;

    uint16_t PB1_curr_state = PORTBbits.RB2; 
    uint16_t PB2_curr_state = PORTBbits.RB4;
    uint16_t PB3_curr_state = PORTAbits.RA4;

    if (PB1_curr_state == 0 && PB1_prev_state == 1) {
        *PB1_event = 1;
    }
    if (PB2_curr_state == 0 && PB2_prev_state == 1) {
        *PB2_event = 1;
    }
    if (PB3_curr_state == 0 && PB3_prev_state == 1) {
        *PB3_event = 1;
    }

    PB1_prev_state = PB1_curr_state;
    PB2_prev_state = PB2_curr_state;
    PB3_prev_state = PB3_curr_state;
}
