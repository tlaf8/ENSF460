
/*
 * File:   main.c
 * Author: Thierry Laforge, Alex Tong, Michael Tapang
 *
 * Created on: October 28, 2024
 */

// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC oscillator (FRC))
#pragma config IESO = OFF               // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

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
#include <p24F16KA101.h>
#include "clkChange.h"
#include "UART2.h"
#include "ADC.h"
#include "IOs.h"


uint16_t PB1_event;
uint16_t PB2_event;
uint16_t PB3_event;
uint16_t streaming = 0;
uint16_t mode = 0;
uint16_t time = 0;


void display_ADC_as_bar(uint16_t adc_value) {
    uint8_t bar_length = (adc_value * 40) / 0x03FF;
    
    Disp2String("Mode 0: *");
    for (uint8_t i = 0; i < bar_length; i++) {
        Disp2String("*");
    }
    Disp2Hex(adc_value);
    Disp2String("\r");
}

void clear_line() {
    Disp2String("\r");
    for(int i = 0; i < 56; i++) {
        Disp2String(" ");
    }
    Disp2String("\r");
}

int main(void) {
        
    newClk(500);
    
    //T3CON config
    T2CONbits.T32 = 0; // operate timer 2 as 16 bit timer
    T3CONbits.TCKPS = 1; // set prescaler to 1:8
    T3CONbits.TCS = 0; // use internal clock
    T3CONbits.TSIDL = 0; //operate in idle mode
    IPC2bits.T3IP = 2; //7 is highest and 1 is lowest pri.
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1; //enable timer interrupt
    PR3 = 15625 * 2; // set the count value for 0.5 s (or 500 ms)
    TMR3 = 0;
    T3CONbits.TON = 1;
    
    IOinit();
    
    /* Let's set up our UART */    
    InitUART2();
    
    Disp2String("\n\n\rBegin execution...\n\n\r");
    
    while (1) {
        Idle();
        
        clear_line();
        
        if (mode == 0) {
            display_ADC_as_bar(do_ADC());
        } else {
            T3CONbits.TON = 0;
            
            while (streaming == 0 && PB2_event == 0) {
                Disp2String("Mode 1: Entered streaming mode. Start python script now\n\r");
                Idle();
            }
            
            PB2_event = 0;
            T3CONbits.TON = 1;
            Disp2String("python -- ");
            Disp2Dec(do_ADC());
            Disp2String("\r\n");
        }
    }
}


// Timer 2 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    //Don't forget to clear the timer 2 interrupt flag!
    IFS0bits.T2IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    //Don't forget to clear the timer 2 interrupt flag!
    IFS0bits.T3IF = 0;
    time++;
    if (time >= 12) {
        time = 0;
        streaming = 0;
        mode = 0;
    }
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    //Don't forget to clear the CN interrupt flag!
    IFS1bits.CNIF = 0;
    IOcheck(&PB1_event, &PB2_event, &PB3_event);
   
    if (PB1_event == 1 && streaming == 0) {
        mode = !mode;
    } else if (PB2_event == 1 && mode == 1) {
        streaming = 1;
        time = 0;
    }
}