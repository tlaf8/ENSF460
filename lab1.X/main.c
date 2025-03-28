/*
 * File:   main.c
 * Author: UPDATE THIS WITH YOUR GROUP MEMBER NAMES OR POTENTIALLY LOSE POINTS
 *
 * Created on: USE THE INFORMATION FROM THE HEADER MPLAB X IDE GENERATES FOR YOU
 */

// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRCDIV           // Oscillator Select (8 MHz FRC oscillator with divide-by-N (FRCDIV))
#pragma config IESO = ON                // Internal External Switch Over bit (Internal External Switchover mode enabled (Two-Speed Start-up enabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = ON            // CLKO Enable Configuration bit (CLKO output disabled; pin functions as port I/O)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8 MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary oscillator configured for high-power operation)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor Selection (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (MCLR pin enabled; RA5 input pin disabled)

// FICD
#pragma config ICS = PGx2               // ICD Pin Placement Select bits (PGC2/PGD2 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = SOSC            // RTCC Reference Clock Select bit (RTCC uses SOSC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = ON             // Deep Sleep Watchdog Timer Enable bit (DSWDT enabled)

// #pragma config statements should precede project file includes.

#include <xc.h>
#include <libpic30.h>

#define SEC 74064
#define HALF 37032
#define FOUR 296256

void delay_c(unsigned long long cycles) {
    for(unsigned long long i = 0; i < cycles; i++) {}
}

int main(void) {  
    AD1PCFG = 0xFFFF; /* sets I/O pins that can also be analog to be digital */
    TRISBbits.TRISB8 = 0;       // LED
    TRISAbits.TRISA2 = 1;       // PB1
    CNPU2bits.CN30PUE = 1;      // PB1 Pullup Enable
    TRISBbits.TRISB4 = 1;       // PB2 
    CNPU1bits.CN1PUE = 1;       // PB2 Pullup Enable
    TRISAbits.TRISA4 = 1;       // PB3
    CNPU1bits.CN0PUE = 1;       // PB3 Pullup Enable
    
    while(1) {
        if (!PORTAbits.RA2 + !PORTBbits.RB4 + !PORTAbits.RA4 >= 2) {
            LATBbits.LATB8 = 1;
        } else if (PORTAbits.RA2 == 0) {
            LATBbits.LATB8 = 1;
            delay_c(HALF);
            LATBbits.LATB8 = 0;
            delay_c(HALF);
        } else if (PORTBbits.RB4 == 0) {
            LATBbits.LATB8 = 1;
            delay_c(SEC);
            LATBbits.LATB8 = 0;
            delay_c(SEC);
        } else if (PORTAbits.RA4 == 0) {
            LATBbits.LATB8 = 1;
            delay_c(FOUR);
            LATBbits.LATB8 = 0;
            delay_c(FOUR);
        } else {
            LATBbits.LATB8 = 0;
        }
    }
}