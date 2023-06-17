#include <xc.h>
#include "stepper_motor.h"

void init_stepper(stepperMotor* stepper_motor, int current_coil, int step_counter, int direction, unsigned char hex_coil_register_values[COILS_NUMBER], volatile unsigned char *register_name){
    for (int i = 0; i < COILS_NUMBER; i++) {
       stepper_motor ->  hex_coil_register_values[i] = hex_coil_register_values[i];
    }
    stepper_motor -> current_coil = current_coil;
    stepper_motor -> step_counter = step_counter;
    stepper_motor -> direction = direction;
    stepper_motor -> register_name = register_name;
}

void turn_on_current_coil(stepperMotor* stepper_motor){
    *(stepper_motor -> register_name) =  stepper_motor -> hex_coil_register_values[stepper_motor -> current_coil];
    return;
}

void update_current_coil(stepperMotor* stepper_motor){
    stepper_motor -> current_coil = stepper_motor -> current_coil + stepper_motor -> direction;
    stepper_motor -> step_counter = stepper_motor -> step_counter + 1;
    if (stepper_motor -> current_coil == -1) {
        stepper_motor -> current_coil = 3;
    } else if(stepper_motor -> current_coil == 4) {
        stepper_motor -> current_coil = 0;
    } 
    return;
}

int reach_goal(stepperMotor* stepper_motor, int goal_to_reach){
    if(stepper_motor -> step_counter >= goal_to_reach){
        stepper_motor -> step_counter = 0;
        return 1;
    } else {
        turn_on_current_coil(stepper_motor);
        update_current_coil(stepper_motor);
        return 0;
    }
}

void change_direction(stepperMotor* stepper_motor){
    stepper_motor -> direction = stepper_motor -> direction*(-1);
    return;
}
