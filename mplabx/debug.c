#include <xc.h>
#define _XTAL_FREQ 4000000 // Freq of the oscillator

#include <stdio.h> 
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "serial_rs232.h"
#include "init_PIC.h"
#include "stepper_motor.h"
#include "lcd.h"

void set_register(volatile unsigned char *reg_name, unsigned char hex_value);

void set_register(volatile unsigned char *reg_name, unsigned char hex_value){
    *reg_name = 0x00;
}
char hex_joint_values[COILS_NUMBER] = {0x01, 0x02, 0x04, 0x08};
char hex_end_effector_values[COILS_NUMBER] = {0x10, 0x20, 0x40, 0x80};

stepperMotor joint_stepper;


void main (void){
    init_PORTS();
    // init_stepper(&joint_stepper, 0, 0, 1, hex_end_effector_values, &LATB);
    lcd_init();
    lcd_cmd(L_NCR);
    
    lcd_cmd(L_CLR);
    lcd_cmd(L_L1);
    lcd_str("Device has been reset");
    
    while(1){
        lcd_update(0);
        __delay_ms(2000);
        lcd_update(1);
        __delay_ms(2000);
        /*turn_on_current_coil(&joint_stepper);
        update_current_coil(&joint_stepper);
        __delay_ms(100);
        for (int i = 0; i<4; i++){
            LATB = hex_joint_values[i];
            __delay_ms(10);
        }*/
        /*set_register(&LATB, 0x00);
        __delay_ms(1000);
        LATB = 4;
        __delay_ms(1000);*/
        
        
    }
}
