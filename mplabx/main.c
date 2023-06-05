
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
#include "timer.h"

void __interrupt() rx_char_usart(void);
void compute_next_step(int* current_step, int step_direction);
void update_direction_and_counters(int* counter_step, int* step_direction, int* counter);
void mix_exit_condition(int counter, int* current_step);

static _Bool state_changed = false;
static _Bool msg_sent = false;
static _Bool read_new_char = false;
static _Bool timer_done = false;
const char* state_msgs[6] = {
    "IDLE: waiting for jab to be placed in init pos",
    "INIT_POS: the jab is at the init position, process is starting",
    "MOVEMENT",
    "OVEN: reaching the correct temperature",
    "MIXING_STATION: the jab has reached the mixing station",
    "PICK_UP: the jab has reached the pick up station"
};
int state;
int mix_current_step;
int mix_direction;
int mix_step_counter;
int mix_counter;


void main(void){
    unsigned char rx_char = ' ';
    init_PORTS();
    init_USART();
    init_timer_2();
    init_ccp1();
    init_interrupts();
    lcd_init();
    lcd_cmd(L_NCR);
    
    lcd_cmd(L_CLR);
    lcd_cmd(L_L1);
    lcd_str("Device has been reset");
    
    ei();
    mix_current_step = 0;
    mix_direction = 1;
    mix_step_counter = 0;
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
        
        
        if (state == 2){ // Movement
            LATBbits.LATB1 = 1;
        } else {
            LATBbits.LATB1 = 0; // Stop Movement
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
                    state = 2;
                    state_changed = true;
                    serial_tx_char(state_translator_micro_to_fpga(&state)); // Send state update to fpga
                    
                } else {
                    if (T0CONbits.TMR0ON == 0){ // The first time init the timer
                        init_timer_0(); // Initializes and starts a timer
                    }
                }
            } else if (state == 4){
                switch(mix_current_step){
                    
                    case 0:
                        LATA = 0x01;
                        compute_next_step(&mix_current_step, mix_direction);
                        update_direction_and_counters(&mix_step_counter, &mix_direction, &mix_counter);
                        mix_exit_condition(mix_counter, &mix_current_step);
                        __delay_ms(10);
                        break;
                    case 1:
                        LATA = 0x02;
                        compute_next_step(&mix_current_step, mix_direction);
                        update_direction_and_counters(&mix_step_counter, &mix_direction, &mix_counter);
                        mix_exit_condition(mix_counter, &mix_current_step);
                        __delay_ms(1);
                        break;
                    case 2:
                        LATA = 0x04;
                        compute_next_step(&mix_current_step, mix_direction);
                        update_direction_and_counters(&mix_step_counter, &mix_direction, &mix_counter);
                        mix_exit_condition(mix_counter, &mix_current_step);
                        __delay_ms(1);
                        break;
                    case 3:
                        LATA = 0x08;
                        compute_next_step(&mix_current_step, mix_direction);
                        update_direction_and_counters(&mix_step_counter, &mix_direction, &mix_counter);
                        mix_exit_condition(mix_counter, &mix_current_step);
                        __delay_ms(1);
                        break;
                    case 10:
                        state = 2;
                        state_changed = true;
                        mix_direction = 1;
                        mix_step_counter = 0;
                        mix_counter = 0;
                    
                }
            } else if (state == 5){
                if (timer_done){
                    state = 2; // Here it should go either to trash Move [2] or to Idle [7]
                    state_changed = true;
                    serial_tx_char(state_translator_micro_to_fpga(&state)); // Send state update to fpga
                } else {
                    if (T0CONbits.TMR0ON == 0){ // init if it has not been turned on yet
                        init_timer_0();
                    }
                }
            }
        }
    }    
}

void compute_next_step(int* current_step, int step_direction){
    *current_step = *current_step + step_direction;
    if (*current_step == -1) {
        *current_step = 3;
    } else if(*current_step == 4) {
        *current_step = 0;
    }
    return;
}

void update_direction_and_counters(int* counter_step, int* step_direction, int* counter){
    *counter_step = *counter_step + 1;
    if (*counter_step >= 96){
        *counter_step = 0;
        *counter = *counter + 1;
        *step_direction = *step_direction *(-1);
    }
    return;
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
