
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
static _Bool msg_sent = false;
static _Bool read_new_char = false;
static _Bool timer_done = false;
const char* state_msgs[8] = {
    "IDLE: waiting for  vial to be placed in init pos",
    "INIT_POS: the vial is at the init position, process is starting",
    "MOVEMENT",
    "OVEN: reaching the correct temperature",
    "GRASPING",
    "MIXING",
    "DILUTING",
    "PICK_UP: the vial has reached the pick up station"
};
int state;
int mix_current_step;
int mix_direction;
int mix_step_counter;
int mix_counter;
int hex_joint_values[COILS_NUMBER] = {0x01, 0x02, 0x04, 0x08};
int hex_end_effector_values[COILS_NUMBER] = {0x01*16, 0x02*16, 0x04*16, 0x08*16};
int joint_homed = 0;
int end_effector_homed = 0;
int trash_counter = 0;
int move_to_trash = 0;

stepperMotor joint_stepper;

stepperMotor end_effector_stepper;


void main(void){
    unsigned char rx_char = ' ';
    init_PORTS();
    init_USART();
    init_timer_2();
    init_ccp1();
    init_interrupts();
    configure_analog_digital_conversion();
    init_stepper(&joint_stepper, 0, 0, 1, hex_joint_values);
    init_stepper(&end_effector_stepper, 0, 0, 1, hex_end_effector_values);
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
                if (rx_char == 'u'){
                    serial_tx_char(rx_char);
                }
                serial_tx_char(rx_char);
                state_translator_fpga_to_micro(rx_char, &state);
                read_new_char = false;  
            }
            snprintf(greet_str, sizeof(greet_str), "%s", state_msgs[state]);
            lcd_cmd(L_CLR);
            lcd_cmd(L_L1);
            lcd_str(greet_str);
            
            state_changed = false;
        }
        
        
        if (state == 2){
            LATAbits.LATA1 = 1; // Belt Movement
            if (move_to_trash && trash_counter < 100){
                trash_counter++;
            } else if (move_to_trash && trash_counter >= 100){
                state = 0;
            }
        } else {
            LATAbits.LATA1 = 0; // Stop Belt Movement
            if (state == 0) {
                if (!msg_sent){
                    serial_tx_char(state_translator_micro_to_fpga(&state));
                    msg_sent = true;
                }
            } else if (state == 1){
                state = 2;
                state_changed = true;
                serial_tx_char(state_translator_micro_to_fpga(&state));
            } else if (state == 3){
                if (timer_done){
                    if(check_temperature(get_temperature())){
                        state = 2;
                        state_changed = true;
                        timer_done = false;
                        serial_tx_char(state_translator_micro_to_fpga(&state)); // Send state update to fpga
                    } else{
                        //Stop or move to trash
                    }
                    
                } else {
                    if (T0CONbits.TMR0ON == 0){ // The first time init the timer
                        wait_for_zero();
                        init_timer_0(); // Initializes and starts a timer
                    }
                    int current_temp = get_temperature();
                }
                
            } else if (state == 4){
                __delay_ms(3);
                if (joint_homed && reach_goal(&joint_stepper, 50)){
                    joint_homed = 0;
                }
                if (!joint_homed && end_effector_homed && reach_goal(&end_effector_stepper, 100)){
                    end_effector_homed = 0;
                }
                if (!end_effector_homed * !joint_homed){
                    state = 5;
                    state_changed = true;
                }
            } else if (state == 5){
                __delay_ms(3);
                if (reach_goal(&joint_stepper, 100)) {
                    change_direction(&joint_stepper);
                    mix_counter++;
                }
                
                if (mix_counter >= 20){
                    state = 6;
                    mix_counter = 0;
                    init_stepper(&joint_stepper, 0, 0, 1, hex_joint_values);
                    change_direction(&joint_stepper);
                    change_direction(&end_effector_stepper);
                }
            } else if (state == 6){
                __delay_ms(3);
                
                if (!end_effector_homed && reach_goal(&end_effector_stepper, 100)){
                    end_effector_homed = 1;
                }
                
                if (end_effector_homed && !joint_homed && reach_goal(&joint_stepper, 50)){
                    joint_homed = 1;
                }
                
                if (end_effector_homed * joint_homed){
                    state = 2;
                }
            } else if (state == 7){
                if (timer_done){
                    state_changed = true;
                    serial_tx_char(state_translator_micro_to_fpga(&state)); // Send state update to fpga
                    state = 2; // Here it should go either to trash Move [2] because the vial hasn't been picked up on time
                    move_to_trash = 1;
                    trash_counter = 0;
                } else {
                    if (T0CONbits.TMR0ON == 0){ // Init if it has not been turned on yet
                        init_timer_0();
                    }
                    // Here we should check if the vial is still there
                }
            }
        }
    }    
}

void mix_exit_condition(int counter, int* current_step){
    if (counter >= 20){
        *current_step = 10;
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
    }
}
