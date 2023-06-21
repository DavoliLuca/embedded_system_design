
#include <xc.h>
#include "oven.h"

int temperature_int;
int temperature_scaled;

// Configuring A/D conversion on AN4 for the oven temperature
void configure_ad_conversion_oven(void){
    ADCON0 = 0;
    ADCON0bits.CHS0 = 0; // CHS<0:2> to 100 so that AN4, RA5, is A/D
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 1;
    ADCON0bits.ADON = 1;
    ADRESH = 0;
    ADRESL = 0;
    ADCON1 = 0; 
}

// Tank fluid level for the dilutor, using AN0
void configure_ad_conversion_tank(void){
    ADCON0 = 0;
    ADCON0bits.CHS0 = 0; // CHS<0:2> to 100 so that AN0, RA0, is A/D
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.ADON = 1;
    ADRESH = 0;
    ADRESL = 0;
    ADCON1 = 0; 
}

// Reads the analog register converts and returns the temperature
int get_temperature(void){
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO == 1);
    return (int) (ADRESH * TEMP_CONST) + TEMP_OFFSET;
}

// Function for the dilutor, used to get the liters getting and scaling the
// value loaded by the linear powermeter and read through an analog pin
int get_liters(void){
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO == 1);
    return (int) (ADRESH * 0.01);
}

// So that we wait for the increasing front of the saw-tooth wave from the generator, -55 corresponds to 0
void wait_for_zero(void){
    while(1){
        if (get_temperature() == -55){
            break;
        }
    }
}

// Check if the gradient of the temperature is in the expected range
int check_temperature(int temp_to_be_checked){
    float grad = abs(temp_to_be_checked - (-55))/5; // 0 V mapped to -55 C, 5 is the time in s from the counter
    if (grad >= LOW_GRAD && grad <= HIGH_GRAD){
        return 1;
    } else {
        return 0;
    }
}
