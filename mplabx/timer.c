#include "timer.h"
#include <stdint.h>

// PARAMETRI e COSTANTI
static double const f_osc = 1000000; // in Hz
static double const t_osc = 1/f_osc;
static double pwm_period = 20e-3; // Period in s
static double tmr2_prescaler = 16;


void init_timer_0(void){
    // Reset
    T0CON = 0;
    // Config
    TMR0L = 0x96; // Set starting value of the timer so that it counts back from 5s
    TMR0H = 0x98;
    INTCONbits.TMR0IE = 1;
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS0 = 1; // Set prescaler to 128 so that the max count is 8388608 
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS2 = 1;
    T0CONbits.TMR0ON = 1;
}

void init_timer_2(void){
    // Reset
    T2CON = 0;
    TMR2 = 0;

    T2CONbits.TMR2ON = 1;
    T2CONbits.T2CKPS0 = 1;
    T2CONbits.T2CKPS1 = 1; // Set prescaler to 16

    PR2 = (uint8_t) (((pwm_period / (4*t_osc*tmr2_prescaler)) - 1)*4);
    
    return;
}

void init_ccp1(void){
    CCP1CON = 0x00; // Clean register
    // We have to assign PR2 as defined in datasheet
    // Datasheet formula --> PWM period = (PR2+1)*4*Tosc*(TMR2 prescaler))
    update_pwm_duty_ccp1(5e-3); // Set initial pwm duty cycle
    
    // SET PWM MODE
    CCP1CONbits.CCP1M3 = 1;
    CCP1CONbits.CCP1M2 = 1;    
    return;
}

void update_pwm_duty_ccp1(double time_up){
    // Formula da datasheet --> converto in bin e dopo solo 8 piu significativi
    // i 2 meno significativi in ccp1con
    uint16_t new_duty; // 16 bit perche il risultato e a 10 bit
    uint8_t lsbs_duty;
    
    new_duty = (0.001) / (t_osc * tmr2_prescaler);
    lsbs_duty = (uint8_t) new_duty; // cast a 8 bit tengo solo 8 bit meno significativi
    
    // Update CCP1CON<4>
    // BITWISE OPERATIONS: & is a bitwise AND | is a bitwise OR
    if (lsbs_duty & 0x01) CCP1CON |= (1u << 4);
    else CCP1CON &= ~(1u << 4);
    // Update CCP1CON<5>
    if (lsbs_duty & 0x02) CCP1CON |= (1u << 5);
    else CCP1CON &= ~(1u << 5);
    
    // Carico 8 bit piu significativi in CCPR1L
    CCPR1L = (uint8_t) (new_duty >> 2);
}
