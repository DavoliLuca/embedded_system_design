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
