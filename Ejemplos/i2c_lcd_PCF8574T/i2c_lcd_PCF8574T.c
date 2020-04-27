#include <config.h>
#include <i2c_LCD.c>



void main() {

lcd_init(0x4E,16,2);
lcd_backlight_led(1);
lcd_clear();


while(true){  
printf(lcd_putc,"\fLCD_i2c_0x4E"); 
delay_ms(1000); 
}

} 