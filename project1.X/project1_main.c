/*
 * File:   main.c
 * Author: Thierry Laforge, Alex Tong, Michael Tapang
 *
 * Finished: October 21st, 2024
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

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p24F16KA101.h>
#include "UART2.h"
#include "clkChange.h"
#include "TimeDelay.h"
#include "IOs.h"

// Define states
#define SET 0
#define CNT 1
#define CLR 2
#define FIN 3

// Global variables for button presses
int PB1_event = 0;
int PB2_event = 0;
int PB3_event = 0;

// Trackers for holding down buttons
int PB1_hold_count = 0;  
int PB2_hold_count = 0;  

int min = 0;
int sec = 0;

// Increment to add to minutes and seconds, can be 1 or 5
int min_increment = 1;
int sec_increment = 1;

// Display flag to print out time and state
int disp = 0;

// Used in order to flip from counting down to not counting
// Can think of it as play/pause flag
int counting = 0;

// Current state
int state = SET;
char *state_str = "SET";


void intToStr(int N, char *str) {
    int i = 0;

    if (N == 0) {
        str[i++] = '0';
    } else {
        while (N > 0) {
            str[i++] = N % 10 + '0';
            N /= 10;
        }
    }
    
    while (i < 2) {
        str[i++] = '0'; 
    }
    
    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}


int main(void) {
    AD1PCFG = 0xFFFF;
    
    // Enable button interrupts on all buttons
    CNEN1bits.CN0IE = 1;
    CNEN1bits.CN1IE = 1;    
    CNEN2bits.CN30IE = 1;

    // Set up timer 2 with PR2 at 2930, prescaler 1:256 for 3 seconds
    T2CONbits.TCKPS = 3;
    T2CONbits.TCS = 0;
    T2CONbits.TSIDL = 0;
    IPC1bits.T2IP = 2;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;
    PR2 = 2930; 
    TMR2 = 0;
    
    // Set up timer 3 with PR3 at 10417, prescaler 1:8 for 1 second
    T3CONbits.TCKPS = 1;
    T3CONbits.TCS = 0;
    T3CONbits.TSIDL = 0;
    IPC2bits.T3IP = 2;
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1;
    PR3 = 10417;
    TMR3 = 0;

    // Enable LED as output and ensure LED is off 
    TRISBbits.TRISB8 = 0;
    LATBbits.LATB8 = 0;

    // Set clock to 500MHz and initialize IO/UART
    newClk(500);
    InitUART2();
    IOinit();    
    
    // Create buffers to hold string-ified time values
    char min_buf[3];
    char sec_buf[3];
    
    Disp2String("\n\nBegin...\n\n\r");
    
    while(1) {
        // State machine
        switch(state) {
            case SET:
                // Set LED to low
                LATBbits.LATB8 = 0;
                
                // Handle PB1_event and PB2_event
                if (PB1_event) {
                    // Increment minute by 1 and start TMR3 to check for holding
                    min += min_increment;
                    if (min > 59) min = 59;
                    PB1_hold_count++;
                    T3CONbits.TON = 1;
                } else if (PB2_event) {
                    // Increment second by 1 and start TMR3 to check for holding
                    sec += sec_increment;
                    if (sec > 59) sec = 59;
                    PB2_hold_count++;
                    T3CONbits.TON = 1;
                
                // If PB1_event or PB2_event are not pressed, reset everything and disable timer
                } else {
                    // PBs are not being held at this point
                    sec_increment = 1;
                    min_increment = 1;
                    PB1_hold_count = 0;
                    PB2_hold_count = 0;
                    T3CONbits.TON = 0;
                }

                // Time change, need to print out new time
                if (PB1_event || PB2_event) {
                    disp = 1;
                }
                break;
            
            case CLR:
                // Pause timer 3 and set LED to off
                counting = 0;
                LATBbits.LATB8 = 0;
                
                // Handle PB1_event or PB2_event
                // This will change the state from CLR to SET
                if (PB1_event) {
                    state = SET;
                    state_str = "SET";
                    disp = 1;
                    PB1_hold_count++;
                    TMR3 = 0;
                    PR3 = 10417;
                    T3CONbits.TON = 1;
                }
                
                if (PB2_event) {
                    state = SET;
                    state_str = "SET";
                    disp = 1;
                    PB2_hold_count++;
                    TMR3 = 0;
                    PR3 = 10417;
                    T3CONbits.TON = 1;
                }
                
            // Any of PB1 or PB2 changes state from FIN to SET
            case FIN:
                if (PB1_event) {
                    state = SET;
                    state_str = "SET";
                    min++;
                    disp = 1;
                    PB1_hold_count++;
                    TMR3 = 0;
                    PR3 = 10417;
                    T3CONbits.TON = 1;
                }
                
                if (PB2_event) {
                    state = SET;
                    state_str = "SET";
                    sec++;
                    disp = 1;
                    PB2_hold_count++;
                    TMR3 = 0;
                    PR3 = 10417;
                    T3CONbits.TON = 1;
                }
                
            default:
                ;
                                                
        }
        
        // If display flag has been set (time change, new state, etc.), display said changes
        if (disp) {
            intToStr(min, min_buf);
            intToStr(sec, sec_buf);
            
            Disp2String(state_str);
            Disp2String(" ");
            Disp2String(min_buf);
            Disp2String("m : ");
            Disp2String(sec_buf);
            Disp2String(" s");
            
            // If state is in FIN (timer ran to 0), add the alarm part to the message)
            if (state == FIN) {
                Disp2String(" -- ALARM\n\r");
            } else {
                Disp2String("\n\r");
            }
            disp = 0;
        }
        
        // zzz
        Idle();
    }
    return 0;
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    // Long press occurred since timer 2 ran to PR2 while PB3 was still being pressed
    // If PB3 had been let go before timer interrupt, CNInterrupt would have disabled timer there
    IFS0bits.T2IF = 0;
    LATBbits.LATB8 = 0;
    state = CLR;
    state_str = "CLR";
    disp = 1;
    min = 0;
    sec = 0;

    // In CLR mode, make sure no timer is active
    TMR2 = 0;
    TMR3 = 0;
    T2CONbits.TON = 0;
    T3CONbits.TON = 0;
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    IFS0bits.T3IF = 0;
    
    // Used to check if PB1 or PB2 is being held down, and to change increment accordingly
    if (PB1_event) {
        // Set min to 5 if hold count more than 5 and reset back to 1 otherwise
        min_increment = (PB1_hold_count >= 5) ? 5 : 1;
    } else if (PB2_event) {
        // Set sec to 5 if hold count more than 5 and reset back to 1 otherwise
        sec_increment = (PB2_hold_count >= 5) ? 5 : 1;
    } else if (!PB1_event && !PB2_event && counting == 0) {
        // None of PB1 or PB2 is pressed and timer is not currently counting
        // Must have let go of all buttons so turn timer off and reset increments
        sec_increment = 1;
        min_increment = 1;
        TMR3 = 0;
        T3CONbits.TON = 0;
    }
    
    // In the counting stage, being counting down
    if (state == CNT) {
        // Flip the LED on and off
        LATBbits.LATB8 = !LATBbits.LATB8;
        
        // Counting logic
        if (--sec < 0) {
            if (min > 0) {
                min--;
                sec = 59;
            }
        }
        
        // Time change so make sure to reflect it in UART
        disp = 1;
        
        // Check if timer has run out, if so keep LED on, turn off timers, and set state to FIN
        if (sec <= 0 && min <= 0) {
            LATBbits.LATB8 = 1;
            counting = 0;
            T3CONbits.TON = 0;
            state = FIN;
            state_str = "FIN";
        }
    }
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    IFS1bits.CNIF = 0;
    
    // Some PB has been pressed, read all and update global flags
    PB1_event = !PORTAbits.RA2;
    PB2_event = !PORTBbits.RB4;
    PB3_event = !PORTAbits.RA4;
       
    // Only PB3_event, begin timer to count up to 3 seconds to determine long or short press
    if (!PB1_event && !PB2_event && PB3_event) {
        TMR2 = 0;
        T2CONbits.TON = 1;
    
    // No PBs (including PB3) is pressed)
    } else if (!PB1_event && !PB2_event && !PB3_event) {
        // Check to see if TMR2 was previously started
        if (0 < TMR2 && TMR2 < PR2) {
            // If so flip from counting to paused or vice versa
            counting = !counting;
            
            // If counting, update state and turn timer 3 on
            // Using PR3 at 31250, prescaler remains the same for 1 second delay
            if (counting) {
                state = CNT;
                state_str = "CNT";
                disp = 1;
                TMR3 = 0;
                PR3 = 31250;
                T3CONbits.TON = 1;
            } else {
                // Timer has been counting so pause it
                T3CONbits.TON = 0;
            }
        }
        
        // Since no PBs are being pressed, ensure timer 2 is off
        T2CONbits.TON = 0;
    } else {
        // Idk if this is needed but keeping it in case
        TMR2 = 0;
    }
}
