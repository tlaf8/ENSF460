/*
 * File:   project2_main.c
 * Author: Thierry Laforge, Alex Tong, Michael Tapang
 *
 * Created on: November, 2024
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
#include "PWM.h"
#include "Clock.h"

// \033[2K clears line
// \r\n carriage return, line feed
// \033[2K clears line
// \r carriage return
// \033[A moves cursor back up one line. Opposite of \n (can use \033[D instead of \n too)
#define CLEAR "\033[2K\r\n\033[2K\r\033[A"

typedef enum {
    ON,
    OFF,
} state_t;

void u_atoi(uint16_t num, char *str);

state_t state = OFF;
state_t blink = OFF;
state_t led = OFF;
state_t transmit = OFF;

uint16_t PB1_event;
uint16_t PB2_event;
uint16_t PB3_event;
uint16_t PB_event;
uint16_t clear_transmission;
uint16_t adc;

volatile uint16_t duty_cycle = 0; 
volatile uint16_t tick_count = 0; 

int main(void) {
        
    newClk(8);
    
    InitClock();
    
    IOinit();
    
    InitUART2();
        
    PWMinit(39);
    
    Disp2String("\n\n\rBegin execution...\n\n\r");

    while (1) {
        switch (state) {
            
            // OFF state
            case OFF:
                // Clear screen, turn PWM off and ensure LED is off
                Disp2String(CLEAR);
                T2CONbits.TON = 0;
                LATBbits.LATB8 = 0;
                while(state == OFF) {
                    // Display system info
                    Disp2String("Mode: OFF | Blink: ");
                    Disp2String((blink == ON) ? "ON\r" : "OFF\r");
                    transmit = OFF;
                    
                    // Put CPU in low power mode
                    Idle();
                    if (PB_event) {
                        PB_event = 0;
                        IOcheck(&PB1_event, &PB2_event, &PB3_event);
                    }
                    
                    // Handle button presses
                    if (PB1_event) {
                        PB1_event = 0;
                        
                        state = ON;
                        
                        break;
                    } else if (PB2_event) {
                        PB2_event = 0;
                        
                        blink = (blink == ON) ? OFF : ON;
                        
                        break;
                    } else if (PB3_event) {
                        PB3_event = 0;
                        // Do nothing
                    }
                                   
                    // Update blink state
                    if (blink == OFF) {
                        LATBbits.LATB8 = 0;
                    } else {
                        LATBbits.LATB8 = ((led == ON) ? 1 : 0);
                    }
                    
                    T3CONbits.TON = (blink == ON) ? 1 : 0;
                }
                break;
                
            // ON state
            case ON:
                // Turn PWM back on
                Disp2String(CLEAR);
                T2CONbits.TON = 1;
                while(state == ON) {
                    // Read ADC value and adjust accordingly
                    adc = do_ADC();
                    adc = (adc <= 3) ? 0 : adc;
                    adc = (adc >= 1020) ? 1023 : adc;
                    
                    // Display system information
                    Disp2String("Mode: ON | Blink: ");
                    Disp2String((blink == ON) ? "ON | ADC: " : "OFF | ADC: ");
                    Disp2Hex(adc);
                    Disp2String("| Transmit: ");
                    Disp2String((transmit == ON) ? "ON \r": "OFF\r");
                    
                    // Avoiding polling buttons
                    // This will only run when a PB is pressed
                    if (PB_event) {
                        PB_event = 0;
                        IOcheck(&PB1_event, &PB2_event, &PB3_event);
                        
                        // Handle button presses
                        if (PB1_event) {
                            PB1_event = 0;

                            state = OFF;

                            break;
                        } else if (PB2_event) {
                            PB2_event = 0;

                            blink = (blink == ON) ? OFF : ON;
                            if (blink == OFF) {
                                set_duty_cycle(0);
                            }

                            break;
                        } else if (PB3_event) {
                            PB3_event = 0;

                            transmit = (transmit == ON) ? OFF : ON;

                            break;
                        }
                    }

                    // Potentiometer hangs around 2-3 when fully cranked left
                    // Conditional is used to turn off led when fully cranked left
                    if (blink == OFF) {
                        set_duty_cycle(adc);
                    } else {
                        if (led == OFF) {
                            adc = 0;
                        }
                        set_duty_cycle(adc);
                    }
                    
                    // Add transmission line for Python
                    if (transmit == ON) {
                        char str_num[21];
                        u_atoi(adc, str_num);
                        Disp2String("\nTransmitting...");
                        Disp2String(str_num);
                        Disp2String("\033[A\r");
                    }

                    T3CONbits.TON = (blink == ON) ? 1 : 0;
                }
                break;
        }
    }
}


void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    IFS0bits.T2IF = 0;  // Clear Timer2 interrupt flag
    
    // Increment tick and rollover
    tick_count++;
    if (tick_count >= PWM_PERIOD) {
        tick_count = 0;
    }
    
    // Output high if within duty cycle, low otherwise
    if (tick_count < duty_cycle) {
        LATBbits.LATB8 = 1; 
    } else {
        LATBbits.LATB8 = 0; 
    }
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    // Flip LED state from ON/OFF
    led = (led == ON) ? OFF : ON;
    IFS0bits.T3IF = 0;
    
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    PB_event = 1;
    IFS1bits.CNIF = 0;
}

void u_atoi(uint16_t num, char *str) {
    int i = 0;

    if (num == 0) {
        while (i < 3) {
            str[i++] = ' ';
        }
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num != 0) {
        int digit = num % 10;
        str[i++] = digit + '0';
        num /= 10;
    }

    while (i < 4) {
        str[i++] = ' ';
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}