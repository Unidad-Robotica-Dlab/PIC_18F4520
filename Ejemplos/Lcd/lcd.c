#include <config.h>
#include <lcd.c>
//
//lcd 
//
//#define LCD_ENABLE_PIN  PIN_E0                                    ////
////     #define LCD_RS_PIN      PIN_E1                                    ////
////     #define LCD_RW_PIN      PIN_E2                                    ////
////     #define LCD_DATA4       PIN_D4                                    ////
////     #define LCD_DATA5       PIN_D5                                    ////
////     #define LCD_DATA6       PIN_D6                                    ////
////     #define LCD_DATA7       PIN_D7 
//


#define HW_LCD_BLACKLIGHT PIN_E2


void main()
{


lcd_init();
lcd_putc("\fReady...\n");
output_low(HW_LCD_BLACKLIGHT);


while(1){



} 
}