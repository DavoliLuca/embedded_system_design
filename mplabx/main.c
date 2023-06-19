
// Toggles a led and answers with hello world every time a char is received

#include <xc.h>
#define _XTAL_FREQ 4000000 // Freq of the oscillator

#include <stdio.h> 
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "serial_rs232.h"
#include "init_PIC.h"
#include "lcd.h"
#include "utils.h"
#include "oven.h"
#include "timer.h"
#include "stepper_motor.h"

void __interrupt() rx_char_usart(void);

static _Bool state_changed = false;
static _Bool idle_msg_sent = false;
static _Bool read_new_char = false;
static _Bool timer_done = false;

int state;
int mix_current_step;
int mix_direction;
int mix_step_counter;
int mix_counter;
char hex_joint_values[COILS_NUMBER] = {0x01, 0x02, 0x04, 0x08};
char hex_end_effector_values[COILS_NUMBER] = {0x10, 0x20, 0x40, 0x80};

int dilution_done = 0;
int trash_counter = 0;

int grasping_joint_position_reached = 0;
int grasping_ee_position_reached = 0;
int diluting_position_reached = 0;

int joint_homed = 0;
int end_effector_homed = 0;
int joint_dilutor_homed = 0;

int state_before_error = 8;

stepperMotor joint_stepper;

stepperMotor end_effector_stepper;

stepperMotor joint_dilutor_stepper;


void main(void){
    unsigned char rx_char = ' ';
    init_PORTS();
    init_USART();
    init_interrupts();
    
    init_stepper(&joint_stepper, 0, 0, 1, hex_joint_values, &LATB);
    init_stepper(&end_effector_stepper, 0, 0, 1, hex_end_effector_values, &LATB);
    init_stepper(&joint_dilutor_stepper, 0, 0, 1, hex_joint_values, &LATC);
    
    lcd_init();
    lcd_cmd(L_NCR);
    
    lcd_cmd(L_CLR);
    lcd_cmd(L_L1);
    lcd_str("Device has been reset");
    
    ei();
    
    mix_counter = 0;
    
    while(1){
        if (state_changed){
            const char* greet_str[80];
            if (read_new_char){
                rx_char = get_reg_value();
                state_translator_fpga_to_micro(rx_char, &state);  // Send state update to fpga
                read_new_char = false;
                if (state == 14){
                    LATCbits.LATC4 = 1; // Red LED for error from FPGA requires reset
                }
            } else {
                serial_tx_char(state_translator_micro_to_fpga(&state));
            }
            lcd_update(state);
            // __delay_ms(300); // Delay just to show clearly the message
            state_changed = false;
            idle_msg_sent = false;
        }
        
        if (state == 2){
            LATAbits.LATA1 = 1; // Belt Movement
        } else {
            LATAbits.LATA1 = 0; // Stop Belt Movement
            if (state == 0) {
                // Idle
                configure_ad_conversion_tank();
                if (get_liters() < 0.1){
                    LATCbits.LATC5 = 1;
                    LATCbits.LATC4 = 1; // Turning on yellow LED for low solution level can be recovered
                    state = 14;
                    state_changed = true;
                    state_before_error = 0;                  
                }
            } else if (state == 1){
                state = 2;
                state_changed = true;
            }  else if (state == 3){
                state = 4;
                state_changed = true;
            } else if (state == 4){
                if (timer_done){
                    if(check_temperature(get_temperature())){
                        state = 2;
                    } else{
                        state = 14;
                        LATCbits.LATC4 = 1; // Red LED for signaling temp error, not ACK
                    }
                    state_changed = true;
                    timer_done = false;
                } else {
                    if (T0CONbits.TMR0ON == 0){ // The first time init the timer
                        configure_ad_conversion_oven(); // So it is init only once
                        wait_for_zero();
                        init_timer_0(); // Initializes and starts a timer
                    }
                    int current_temp = get_temperature();
                }
                
            } else if (state == 5){ // Vial reached TOF 3 diluting station
                state = 6;
                state_changed = true;
            }else if (state == 6){ // Grasping
                __delay_ms(3);
                if (!grasping_joint_position_reached){ // If the motor hasn't reached the grasping position then move further
                    grasping_joint_position_reached = reach_goal(&joint_stepper, 50);
                }
                // The arm has reached its grasping position, now the End Effector moves to grasp the vial and centers it
                if (grasping_joint_position_reached && !grasping_ee_position_reached){
                    grasping_ee_position_reached = reach_goal(&end_effector_stepper, 100);
                } 
                if (grasping_joint_position_reached * grasping_ee_position_reached){
                    state = 7; // Mixing
                    state_changed = true;
                }
            } else if (state == 7){ // Mix 10 times and 10 times more after dilution
                __delay_ms(3);
                if (reach_goal(&joint_stepper, 100)) {
                    change_direction(&joint_stepper);
                    mix_counter++;
                }
                
                if (mix_counter == 20 && !dilution_done){
                    state = 8; // Dilution
                    state_changed = true;
                } else if (mix_counter == 40) {
                    dilution_done = 0;
                    state = 9; // Release
                    state_changed = true;
                    mix_counter = 0;
                    // Change direction so that vial released
                    change_direction(&joint_stepper);
                    change_direction(&end_effector_stepper);
                }
            } else if (state == 8){ // Dilution
                __delay_ms(3);
                if (!diluting_position_reached){
                    diluting_position_reached = reach_goal(&joint_dilutor_stepper, 200);
                } else if (!dilution_done && diluting_position_reached){
                    //
                    
                    __delay_ms(1000); // Time for the fluid to flow into the vial
                    dilution_done = 1;
                    change_direction(&joint_dilutor_stepper);
                } else if (dilution_done && !joint_dilutor_homed){
                    joint_dilutor_homed = reach_goal(&joint_dilutor_stepper, 200);
                } else if (dilution_done && joint_dilutor_homed){
                    state = 7; // Mixing
                    state_changed = true;
                    diluting_position_reached = 0;
                }
                
            } else if (state == 9){ // Releasing
                __delay_ms(3);
                
                if (!end_effector_homed){
                    end_effector_homed = reach_goal(&end_effector_stepper, 100);
                }
                
                if (end_effector_homed && !joint_homed){
                    joint_homed = reach_goal(&joint_stepper, 50);
                }
                
                if (end_effector_homed * joint_homed){
                    state = 2; // Move towards picking station once released and homed
                    state_changed = true;
                }
            } else if (state == 10){
                state = 11;
                state_changed = true;
            }else if (state == 11){
                if (timer_done){
                    state_changed = true;
                    state = 13; // Here it should go to trash Move [2] because the vial hasn't been picked up on time
                    trash_counter = 0;
                    timer_done = false;
                } else {
                    if (T0CONbits.TMR0ON == 0){ // Init if it has not been turned on yet
                        init_timer_0();
                    }
                    // The fpga takes care of the case in which the vial is collected before the timeout
                }
            } else if (state == 12){ // The vial has been picked before the timeout
                state = 0; // Back to idle once picked
                state_changed = true;
                T0CONbits.TMR0ON = 0;
            } else if (state == 13){ // Move to trash
                LATAbits.LATA1 = 1; // Belt Movement
                if ( trash_counter < 100){
                    __delay_ms(10);
                    trash_counter++;
                } else if (trash_counter >= 100){
                    state = 0; // Back to idle once trashed
                    state_changed = true;
                    T0CONbits.TMR0ON = 0;
                }
            } if (state == 14){
                // Error state
                if (LATCbits.LATC4 == 1 && LATCbits.LATC5 == 1 && get_liters() > 0.1){
                    LATCbits.LATC4 = 0;
                    LATCbits.LATC5 = 0;
                    state = state_before_error;
                    state_changed = true;
                }
                    
            }
        }
    }    
}


void __interrupt() rx_char_usart(void){
    if(PIE1bits.RCIE && PIR1bits.RCIF){// It has to be enabled and triggered
        PIR1bits.RCIF = 0;
        state_changed = true;
        read_new_char = true;
    }
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF){
        T0CON = 0; // Sets to zero also TMR0ON
        INTCONbits.TMR0IF = 0;
        timer_done = true;
        T0CONbits.TMR0ON = 0;
    }
}
