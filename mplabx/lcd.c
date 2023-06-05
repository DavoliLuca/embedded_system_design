// Credits to https://github.com/lcgamboa/picsimlab_examples/blob/master/docs/board_Breadboard/PIC18F4550/test_b0/src/teste_b0.X/lcd.c reworked
#include <xc.h>
#include "lcd.h"

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