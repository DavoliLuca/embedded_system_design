#include <dc_motor.h>
// PARAMETRI e COSTANTI
static double const f_osc = 1000000; // in Hz
static double const t_osc = 1/f_osc;
static double pwm_period = 2e-5; // Period in s
static double tmr2_prescaler = 1;
static uint8_t active_duty_cycle = 0;
static uint8_t new_duty_cycle = 0;

void init_ccp1_dc(void){
    CCP1CON = 0x00; // Clean register
    // We have to assign PR2 as defined in datasheet
    // Datasheet formula --> PWM period = (PR2+1)*4*Tosc*(TMR2 prescaler))
    PR2 = (uint8_t) ((pwm_period / (4*1e-6)) - 1);
    start_dc(50); // Set initial pwm duty cycle
    
    // SET PWM MODE
    CCP1CONbits.CCP1M3 = 1;
    CCP1CONbits.CCP1M2 = 1;    
    return;
}

void start_dc(uint8_t duty){
    // Formula da datasheet --> converto in bin e dopo solo 8 piu significativi
    // i 2 meno significativi in ccp1con
    uint16_t new_duty; // 16 bit perche il risultato e a 10 bit
    uint8_t lsbs_duty;
    
    double req_duty = duty;
    new_duty = ((req_duty/100) * pwm_period) / (t_osc * tmr2_prescaler);
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