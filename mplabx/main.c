
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

void __interrupt() rx_char_usart(void);


// Usart puo triggerare interupt sia quando si manda sia quando si riceve info
// noi la usiamo qui solo per ricezione, usiamo un altro modo per capire quando
// e spedito

static _Bool new_char_rx = false;

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
        if (new_char_rx){
            rx_char = RCREG;
            RCREG = 0;
            const char* greet_str[80];
            snprintf(greet_str, sizeof(greet_str), "The current state is %c", rx_char);
            serial_tx_string(greet_str);
            serial_tx_char(rx_char);
            
            LATBbits.LATB0 =! LATBbits.LATB0; // toggle led
            new_char_rx = false;
            lcd_cmd(L_CLR);
            lcd_cmd(L_L1);
            lcd_str(greet_str);
        }
    }    
}
void __interrupt() rx_char_usart(void){
    if(PIE1bits.RCIE && PIR1bits.RCIF){// It has to be enabled and triggered
        PIR1bits.RCIF = 0;
        new_char_rx = true;
    }
}
