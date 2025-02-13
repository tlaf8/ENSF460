#ifndef PWM_H
#define	PWM_H

#include <xc.h>
#include <stdint.h>

// Configuration
#define PWM_PERIOD  40    // PR2 + 1
#define PWM_MAX     1023  // Maximum input value for duty cycle

extern volatile uint16_t duty_cycle;    
extern volatile uint16_t tick_count;   

// Function declarations
void pwm_init(void);
void set_duty_cycle(uint16_t duty);

#endif

