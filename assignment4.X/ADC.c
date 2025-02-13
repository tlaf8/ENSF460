#include <xc.h>

uint16_t do_ADC(void) {
    uint16_t ADCvalue; // Register for holding ADC output

    // Disable interrupts during ADC sampling
    __builtin_disi(0x3FFF); // Disable all interrupts

    // ADC Configuration
    AD1CON1 = 0;
    AD1CON1bits.SSRC = 0b111;    // Internal counter ends sampling and starts conversion
    AD1CON1bits.ASAM = 0;        // Manual sampling start

    AD1CON2 = 0;
    AD1CON3 = 0;
    AD1CON3bits.SAMC = 0b11111;  // Auto-sample time bits (max sample time)
    AD1CON3bits.ADRC = 0;        // Use system clock

    AD1CHSbits.CH0SA = 5;        // Select AN5 as input for ADC

    AD1PCFG = 0xFFFF;            // Set all pins to digital
    AD1PCFGbits.PCFG5 = 0;       // Set AN5 as analog

    // Start ADC Sampling and Conversion
    AD1CON1bits.ADON = 1;        // Turn on ADC module
    AD1CON1bits.SAMP = 1;        // Start sampling

    while (!AD1CON1bits.DONE);   // Wait for conversion to complete
    
    ADCvalue = ADC1BUF0;         // Read ADC result

    AD1CON1bits.ADON = 0;        // Turn off ADC module

    // Re-enable interrupts after ADC sampling
    __builtin_disi(0x0000);      // Re-enable all interrupts

    return ADCvalue;             // Return ADC result
}
