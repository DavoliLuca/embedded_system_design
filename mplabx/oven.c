
#include <xc.h>
#include "oven.h"

int temperature_int;
int temperature_scaled;

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

int get_temperature(void){
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO == 1);
    return (int) (ADRESH * TEMP_CONST) + TEMP_OFFSET;
}

int get_liters(void){
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO == 1);
    return (int) (ADRESH * 0.01);
}

void wait_for_zero(void){
    while(1){
        if (get_temperature() == -55){
            break;
        }
    }
}

int check_temperature(int temp_to_be_checked){
    float grad = abs(temp_to_be_checked - (-63))/5;
    if (grad >= LOW_GRAD && grad <= HIGH_GRAD){
        return 1;
    } else {
        return 0;
    }
}
