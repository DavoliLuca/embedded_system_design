#include "lcd.h"

// Translates the 8bit USART msg from fpga to a uC state machine state and updates it
void state_translator_fpga_to_micro(unsigned char state_machine_code, int* state){
    lcd_dat(state_machine_code);
    if (state_machine_code == 0x01){
        *state = 1; // Vial at the start position
    } else if (state_machine_code == 0x02){
        *state = 3; // Vial in the oven
    } else if (state_machine_code == 0x04){
        *state = 5; // Vial at the mixing station
    } else if (state_machine_code == 0x08){
        *state = 10; // Vial at the picking station
    } else if (state_machine_code == 0x10){
        *state = 2; // Movement
    } else if (state_machine_code == 0x40){
        *state = 14; // Error
    } else if (state_machine_code == 0x80){
        // No change in state
    } else if (state_machine_code == 0xA0){
        *state = 12;
    } else {
        *state = 0;
    }
    return;
}

// From a state of the uC to a 8bit msg to be sent to fpga
unsigned char state_translator_micro_to_fpga(int* state){
    unsigned char state_machine_code;
    if (*state == 2){
        state_machine_code = 0x10; // Movement
    } else if (*state == 0) {
        state_machine_code = 0x00; // Idle
    } else if (*state == 14) {
        state_machine_code = 0x40; // Error
    } else {
        state_machine_code = 0x80; // Process
    }
    return state_machine_code;
}
