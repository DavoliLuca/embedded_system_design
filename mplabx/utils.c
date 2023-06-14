#include "lcd.h"
void state_translator_fpga_to_micro(unsigned char state_machine_code, int* state){
    lcd_dat(state_machine_code);
    if (state_machine_code == 0x81){
        *state = 1; // Vial at the start position
    } else if (state_machine_code == 0x82){
        *state = 3; // Vial in the oven
    } else if (state_machine_code == 0x84){
        *state = 4; // Vial at the mixing station
    } else if (state_machine_code == 0x88){
        *state = 7; // Vial at the mixing station
    } else if (state_machine_code == 0x90){
        *state = 2; // Movement
    } else if (state_machine_code == 0xA0){
        *state = 8; // Error
    } else {
        *state = 0;
    }
    return;
}

unsigned char state_translator_micro_to_fpga(int* state){
    unsigned char state_machine_code;
    if (*state == 2){
        state_machine_code = 0x90; // Movement
    } else if (*state == 0) {
        state_machine_code = 0x89; // Idle
    } else if (*state == 8) {
        state_machine_code = 0xA0; // Error
    } else {
        state_machine_code = 0x93; // Process
    }
    return state_machine_code;
}
