# 1 "utils.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Program Files (x86)/Microchip/MPLABX/v5.40/packs/Microchip/PIC18Fxxxx_DFP/1.2.26/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "utils.c" 2
const char* state_translator_fpga_to_micro(unsigned char state_machine_code, int* state){
    const char* state_msg;
    if (state_machine_code == 'A'){
        state_msg = "init_pos";
        *state = 1;
    } else if (state_machine_code == 'B'){
        state_msg = "oven";
        *state = 3;
    } else if (state_machine_code == 'D'){
        state_msg = "mixing_pos";
        *state = 4;
    } else if (state_machine_code == '@'){
        state_msg = "pick_up";
        *state = 5;
    } else if (state_machine_code == 'H'){
        state_msg = "movement";
        *state = 2;
    } else {
        state_msg = "NOTHING";

    }
    return state_msg;
}
