#include <config.h>


#define LED PIN_B0
#define DELAY 10

#define HW_LCD_BLACKLIGHT PIN_E2
#include <lcd.c>

signed int16 contador;

int8   enc=0; 
int8   aux=0;  


#INT_RB
void  RB_isr(void) 
{
aux=enc;
enc=input_b()&0b00110000;
if ((aux==0b00100000)&&(enc==0b00110000)){contador++;               
}
if ((aux==0b00110000)&&(enc==0b00100000)){contador--;               
}
}


void main()
{
//ext_int_edge(H_TO_L);
enable_interrupts(INT_RB);
enable_interrupts(GLOBAL);
port_b_pullups(TRUE);

output_low(HW_LCD_BLACKLIGHT);

lcd_init();

lcd_putc("\fReady...\n");

while(1){
output_low(LED);
delay_ms(DELAY);
output_high(LED);
delay_ms(DELAY);

printf("value: %Ld \n\r", contador);
} 
}