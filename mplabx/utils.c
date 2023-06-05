#include "lcd.h"
void state_translator_fpga_to_micro(unsigned char state_machine_code, int* state){
    lcd_dat(state_machine_code);
    if (state_machine_code == 'A'){
        *state = 1;
    } else if (state_machine_code == 'B'){
        *state = 3;
    } else if (state_machine_code == 'D'){
        *state = 4;
    } else if (state_machine_code == '@'){
        *state = 5;
    } else if (state_machine_code == 'H'){
        *state = 2;
    } else {
        *state = 0;
    }
}

unsigned char state_translator_micro_to_fpga(int* state){
    unsigned char state_machine_code;
    if (*state == 0){
        state_machine_code = 0;
    } else if (*state == 1){
        state_machine_code = 'A';
    } else if (*state == 3){
        state_machine_code = 'B';
    } else if (*state == 4){
        state_machine_code = 'D';
    } else if (*state == 5){
        state_machine_code = '@';
    } else if (*state == 2){
        state_machine_code = 'H';
    }
    return state_machine_code;
}
