#include "timer.h"



void init_timer_0(void){
    // Reset
    T0CON = 0;
    // Config
    TMR0L = 0x96; // Set starting value of the timer so that it counts back from 5s
    TMR0H = 0x98;
    INTCONbits.TMR0IE = 1;
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS0 = 1; // Set prescaler to 128 so that the max count is 8388608 
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS2 = 1;
    T0CONbits.TMR0ON = 1;
}
