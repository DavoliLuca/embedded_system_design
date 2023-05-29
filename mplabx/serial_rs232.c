#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h> // --> so that we get the round function 

#include "serial_rs232.h"

static double const fosc = 4000000;
static double tosc = 1/fosc;
static double const desired_baudrate = 9600;

void init_USART(void){
    // Async mode
    // Fullduplex riceve e manda messaggi
    // Desired baud rate = Fosc/ 64*(X+1)) if <BRGH> = 0\
    // Desired baud rate = Fosc/ 16*(X+1)) if <BRGH> = 1
    // X is the nearest integer value for the SPBRG register needed to obtain the desired baud rate
    
    // The choice is up to fosc and error (difference between desired and computed baudrate)
    
    // In this case fosc = 4MHz and baudrate desired 9600bps --> Fosc/16/desired_baud - 1 --> 25
    
    SPBRG = round(((fosc / desired_baudrate) / 16) - 1);
    
    // VALORI DA DATASHEET TABELLA per diversi BRGH noi usiamo = a 1 perche con 0 sarebbe errore troppo grande 7%
    // Trasmissione
    TXSTAbits.TX9 = 0; // Tx 8 bit data
    TXSTAbits.TXEN = 1; // Tx enable
    TXSTAbits.SYNC =0; // async mode
    TXSTAbits.BRGH = 1; // high baud rate bit
    // Ricezione
    RCSTAbits.SPEN = 1; //serial port enable
    RCSTAbits.RX9 = 0;  //RX 8bits data
    RCSTAbits.CREN = 1; //RX enable
    return;
}

void serial_tx_char(unsigned char val){
    TXREG = val;
    while(!TXSTAbits.TRMT); // controllo dello stato della trasmissione, quando bit vale uno buffer di trasmissione vuoto, quindi di default e a 1
    // ci permette di aspettare fino a quando il dato e stato mandato
}

void serial_tx_string(const char* val){
    for (int i=0; val[i] != 0; i++){
        serial_tx_char(val[i]);        
    }
}

unsigned char get_reg_value(){
    unsigned char rx_char = RCREG;
    RCREG = 0;
    return rx_char;
}