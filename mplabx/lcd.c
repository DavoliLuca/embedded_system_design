// Credits to https://github.com/lcgamboa/picsimlab_examples/blob/master/docs/board_Breadboard/PIC18F4550/test_b0/src/teste_b0.X/lcd.c reworked
#include <xc.h>
#include "lcd.h"

const char const_msgs[2][8][20] = {
    {"IDLE: waiting for\0","vial to be placed\0","in init pos\0","\0"},
    {"debug\0","ri\0","","jeep\0"},
};

void lcd_wr(unsigned char val)
{
    LPORT=val;
}

void lcd_cmd(unsigned char val)
{
	LENA=1;
    lcd_wr(val);
    LDAT=0;
    __delay_ms(1);
    LENA=0;
    __delay_ms(1);
	LENA=1;
}
 
void lcd_dat(unsigned char val)
{
	LENA=1;
    lcd_wr(val);
    LDAT=1;
    __delay_ms(1);
    LENA=0;
    __delay_ms(1);
	LENA=1;
}

void lcd_init(void)
{
    
    asm("CLRF PORTD");
    asm("MOVLW 0x00");
    asm("MOVWF TRISD");
    
    asm("CLRF PORTE");
    asm("MOVLW 0x00");
    asm("MOVWF TRISE");
    
    
    
	LENA=0;
	LDAT=0;
	__delay_ms(30);
	LENA=1;
	
	lcd_cmd(L_CFG);
	__delay_ms(30);
	lcd_cmd(L_CFG);
    __delay_ms(30);
	lcd_cmd(L_CFG); //Set
	lcd_cmd(L_OFF);
	lcd_cmd(L_ON); //Turn on
	lcd_cmd(L_CLR); //Clear
	lcd_cmd(L_CFG); //Set
    lcd_cmd(L_L1);
}

void lcd_str(const char* str)
{
    unsigned char i=0;

    while(str[i] != 0 )
    {
      lcd_dat(str[i]);
      i++;
    }  
}

void lcd_manager_init(lcdManager* lcd_manager, unsigned char* new_msg, unsigned char* current_msg){
    lcd_manager -> new_msg = new_msg;
    lcd_manager -> current_msg = new_msg;
}

/*
void lcd_update(lcdManager* lcd_manager){
    if (lcd_manager -> new_msg == "" || lcd_manager -> new_msg == lcd_manager -> current_msg){
        asm("nop");
    } else {
        lcd_cmd(L_CLR);
        lcd_cmd(L_L1);
        lcd_str(lcd_manager -> new_msg[0:20]);
    }
}*/


void lcd_update(int state){
    lcd_cmd(L_CLR);
    const char line_select[4] = {L_L1, L_L2, L_L3, L_L4};
    for (int i=0;i<4;i++ ){
        lcd_cmd(line_select[i]);
        lcd_str(const_msgs[state][i]);          
    }
}