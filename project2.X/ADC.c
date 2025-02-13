#include <xc.h>
#include "ADC.h"

uint16_t do_ADC(void) {
    uint16_t ADCvalue; 

    // Disable interrupts during ADC sampling
    __builtin_disi(0x3FFF);

    // ADC Configuration
    AD1CON1 = 0;
    AD1CON1bits.SSRC = 0b111;    
    AD1CON1bits.ASAM = 0;       

    AD1CON2 = 0;
    AD1CON3 = 0;
    AD1CON3bits.SAMC = 0b11111;  
    AD1CON3bits.ADRC = 0;        

    AD1CHSbits.CH0SA = 5;        

    AD1PCFG = 0xFFFF;           
    AD1PCFGbits.PCFG5 = 0;       

    // Start ADC Sampling and Conversion
    AD1CON1bits.ADON = 1;        
    AD1CON1bits.SAMP = 1;        

    while (!AD1CON1bits.DONE);   
    
    ADCvalue = ADC1BUF0;         

    AD1CON1bits.ADON = 0;        

    // Re-enable interrupts after ADC sampling
    __builtin_disi(0x0000);      

    return ADCvalue;            
}
