// Credits to https://github.com/lcgamboa/picsimlab_examples/blob/master/docs/board_Breadboard/PIC18F4550/test_b0/src/teste_b0.X/lcd.c reworked
#include <xc.h>
#include "lcd.h"

// All the state msgs, formatted so they could easily fit in the lcd
const char const_msgs[15][4][20] = {
    {"IDLE: waiting for","vial to be placed","in init pos",""},
    {"INIT_POS: the vial","is at the init pos", "process is starting", ""},
    {"MOVEMENT", "", "", ""},
    {"OVEN:", "vial has reached", "the oven", ""},
    {"OVEN: reaching the", "correct temperature", "", ""},
    {"GRASPING: the vial","has reached the","mixing station",""},
    {"GRASPING:","grasping has","started",""},
    {"MIXING: the vial","will now be","tilted 10 times",""},
    {"DILUTING: the","vaccine will now","be diluted",""},
    {"RELEASING:","releasing has","started",""},
    {"PICK_UP: the vial","has reached the", "pick up station", ""},
    {"PICK_UP: please","pick up the", "vial before", "timeout"},
    {"PICK_UP: the vial","has been picked", "going back to", "idle"},
    {"TRASH: timeout","exceeded the", "vial will be", "trashed"},
    {"ERROR","", "", ""},
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

void lcd_update(int state){
    lcd_cmd(L_CLR);
    const char line_select[4] = {L_L1, L_L2, L_L3, L_L4};
    for (int i=0;i<4;i++ ){
        lcd_cmd(line_select[i]);
        lcd_str(const_msgs[state][i]);          
    }
}