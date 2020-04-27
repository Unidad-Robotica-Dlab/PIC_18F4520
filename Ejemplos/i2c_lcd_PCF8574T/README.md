# LCD_i2c
Description:   Driver for common LCD with 1/2/3 or 4 rows by 1...20 columns 
               using PCF8574T interface board with I2C protocol. 
               
lcd_init() Must be called before any other function. 

lcd_putc(c) Will display c on the next position of the LCD. 
  
\f Clear LCD display 

\n Set write position on next lcd line 

\b LCD backspace 

lcd_gotoxy(x,y) Set write position on LCD (upper left is 1,1)

lcd_backlight_led(ON)/lcd_backlight_led(OFF) = Turn ON/OFF LCD Backlight LED 

