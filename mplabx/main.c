
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


// Usart puo triggerare interupt sia quando si manda sia quando si riceve info
// noi la usiamo qui solo per ricezione, usiamo un altro modo per capire quando
// e spedito

static _Bool state_changed = false;
static _Bool timer_done = false;
int state;

void main(void){
    unsigned char rx_char = ' ';
    init_PORTS();
    init_USART();
    init_interrupts();
    lcd_init();
    lcd_cmd(L_NCR);
    
    ei();
    
    while(1){
        // toggle rb0 then bit 0 of lATB set as output
        if (state_changed){
            rx_char = get_reg_value();
            const char* greet_str[80];
            const char* state_msg = state_translator_fpga_to_micro(rx_char, &state);
            snprintf(greet_str, sizeof(greet_str), "The current state is %s", state_msg);
            serial_tx_string(greet_str);
            serial_tx_char(rx_char);

            lcd_cmd(L_CLR);
            lcd_cmd(L_L1);
            lcd_str(greet_str);
            
            state_changed = false;
        }
        if (state == 2){ // Movement
            LATBbits.LATB1 = 1;
        } else {
            LATBbits.LATB1 = 0; // Stop Movement
            if (state == 3){
                if (timer_done){
                    state = 2;
                } else {
                    if (T0CONbits.TMR0ON == 0){
                        init_timer_0();
                    }
                }
            } else if (state == 4){
                __delay_ms(2000);
                state = 2;
            } else if (state == 5){
                __delay_ms(2000);
                state = 2;
            } else if (state == 6){
                if (timer_done){
                    state = 2; // Here it should go either to trash Move [2] or to Idle [7]
                } else {
                    if (T0CONbits.TMR0ON == 0){
                        init_timer_0();
                    }
                }
            }
        }
    }    
}
void __interrupt() rx_char_usart(void){
    if(PIE1bits.RCIE && PIR1bits.RCIF){// It has to be enabled and triggered
        PIR1bits.RCIF = 0;
        state_changed = true;
    }
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF){
        T0CON = 0;
        INTCONbits.TMR0IF = 0;
        timer_done = true;
    }
}
