/*****************************************************
Project : Libray
Version : 1.0
Date    : 14/07/2020
Author  : MDP
Adapter : Eduardo Illescas C
Company :
Comments:
Chip type               : PIC 18F4520
Program type            : i2c Lcd driver chip PCF8574P
*****************************************************/
#include<string.h>
#include "i2c_soft.h"


// the Hitachi HD44780U controller chip
#define LCD_LINE_1_ADDRESS 0x00
#define LCD_LINE_2_ADDRESS 0x40
#define LCD_LINE_3_ADDRESS 0x14
#define LCD_LINE_4_ADDRESS 0x54
#define LCD_LINE1 1
#define LCD_LINE2 2
//
int I2C_BYTE;
#define  I2C_ADDR 0x4E
#bit     E   =    I2C_BYTE.2
#bit     RW  =    I2C_BYTE.1
#bit     RS  =    I2C_BYTE.0
#bit     D4  =    I2C_BYTE.4
#bit     D5  =    I2C_BYTE.5
#bit     D6  =    I2C_BYTE.6
#bit     D7  =    I2C_BYTE.7
#bit     BL  =    I2C_BYTE.3
#define  BL_ON    1     // 1 for POSITIVE control, 0 for NEGATIVE control

#define LCD_I2C_ADDR (I2C_ADDR<<1)

#separate
void LCD_I2C_Write(char text[]); 

#separate void LCD_I2C_Init();
#separate void LCD_I2C_GotoXY(int8 x, int8 y);
#separate void LCD_I2C_Putc(char c);
#separate void LCD_I2C_Send_Byte(int8 address, int8 n);
#separate void LCD_I2C_Write_Upper_Nibble(int u);
#separate void LCD_I2C_Write_Lower_Nibble(int l);
#separate void LCD_I2C_ClearLine(char line);
#separate void LCD_I2C_ClearChr(char line,char posX,char size);

int lcd_j,lcd_max,lcd_tempbyte,n,u,l;
int lcd_fixed_delay_us=50;  // Fixed delay to allow LCD to process I2C instruction
int LCD_line;


void LCD_I2C_ClearLine(char line)
{
 char index;
 
 LCD_I2C_GotoXY(1,line);
 for(index = 0;index < 16;index++)
 {
   LCD_I2C_Putc(' ');
 }
 LCD_I2C_GotoXY(1,line);
}

void LCD_I2C_ClearChr(char line,char posX,char size)
{
 char index;
 
 LCD_I2C_GotoXY(posX,line);
 for(index = 1;index <= size;index++)
 {
   LCD_I2C_Putc(' ');
 }
}


int1 LCD_I2C_GetAdd(int8 add)
{
 int8 status;
 int1 rValue = FALSE;
 add = add << 1;
 i2c_start();
 status = i2c_write(add);  // Status = 0 if got an ACK
 i2c_stop();
 if(status == 0)
   rValue = TRUE;
 return rValue;  
}

void LCD_I2C_Write(char text[]) 
{
// Writes a string text[] to LCD via I2C
   RS  = 1;
   RW  = 0;
   E   = 0;
   BL  = BL_ON;
   
   i2c_start(SOFT_I2C);
   i2c_write(SOFT_I2C,LCD_I2C_ADDR);

   lcd_max=strlen(text);
   for(lcd_j=0; lcd_j<lcd_max; ++lcd_j)
   {
      lcd_tempbyte=text[lcd_j];
      
// Send upper nibble   
      LCD_I2C_Write_Upper_Nibble(lcd_tempbyte);
      
// Send lower nibble
      LCD_I2C_Write_Lower_Nibble(lcd_tempbyte);
   }
   i2c_stop(SOFT_I2C);
}

void LCD_I2C_GotoXY(int8 x, int8 y)
{
int8 address;

   switch(y)
     {
      case 1:
        address = LCD_LINE_1_ADDRESS;
        break;
   
      case 2:
        address = LCD_LINE_2_ADDRESS;
        break;
   
      case 3:
        address = LCD_LINE_3_ADDRESS;
        break;
   
      case 4:
        address = LCD_LINE_4_ADDRESS;
        break;
   
      default:
        address = LCD_LINE_1_ADDRESS;
        break;     
     }

   address += x-1;
   LCD_I2C_Send_Byte(0, 0x80 | address);
}

//===================================
void LCD_I2C_Putc(char c)
{
// Writes a char 'c' to LCD via I2C to include some common formats
   switch(c)
   {
      case '\f':
         LCD_I2C_Send_Byte(0x00,0x01);
         LCD_line = 1;
         delay_ms(lcd_fixed_delay_us);
         break;
      
      case '\n':
          LCD_I2C_Send_Byte(0x01, ++LCD_line);
          break;
      
      case '\b':
          LCD_I2C_Send_Byte(0x00,0x10);
          break;
      
      default:
          LCD_I2C_Send_Byte(0x01,c);
          break;
   }
}

//===================================
void LCD_I2C_Send_Byte(int8 address, int8 n)
{
// HD44780U LCD Address Type (RS): Command=0, Data=1
//
   if(address)
   {
      RS=1;   // Data
   }
   else
   {
      RS=0;   // Command
   }
 
   RW  = 0;
   E   = 0;
   BL  = BL_ON;

   i2c_start(SOFT_I2C);
   i2c_write(SOFT_I2C,LCD_I2C_ADDR);

// Send upper nibble   
   LCD_I2C_Write_Upper_Nibble(n);
      
// Send lower nibble
   LCD_I2C_Write_Lower_Nibble(n);
   
   i2c_stop(SOFT_I2C);
   delay_ms(5);
}

//===================================
void LCD_I2C_Init()
{  
// Initialization commands for Hitachi HD44780U LCD Display
//
   delay_ms(300);                               // Let LCD power up

   i2c_start(SOFT_I2C);                           // Begin transfer, claim I2C BUS
   i2c_write(SOFT_I2C,LCD_I2C_ADDR);          // Tell all I2C devices you are talking to LCD_I2C_ADDR

// Following bytes are all Command bytes, i.e. address = 0x00
   LCD_I2C_Send_Byte(0x00, 0x03);   // Write Nibble 0x03 three times (per HD44780U initialization spec)
   delay_us(lcd_fixed_delay_us);                // (per HD44780U initialization spec)
   LCD_I2C_Send_Byte(0x00, 0x03);   //
   delay_us(lcd_fixed_delay_us);                // (per HD44780U initialization spec)
   LCD_I2C_Send_Byte(0x00, 0x03);   //
   delay_us(lcd_fixed_delay_us);                // (per HD44780U initialization spec)
   LCD_I2C_Send_Byte(0x00, 0x02);   // Write Nibble 0x02 once (per HD44780U initialization spec)
   delay_us(lcd_fixed_delay_us);                // (per HD44780U initialization spec)
   LCD_I2C_Send_Byte(0x00, 0x28);   // Set mode: 4-bit, 2+lines, 5x8 dots
   delay_us(lcd_fixed_delay_us);                // (per HD44780U initialization spec) 
   LCD_I2C_Send_Byte(0x00, 0x0C);   // Display ON
   delay_us(lcd_fixed_delay_us);                // (per HD44780U initialization spec) 
   LCD_I2C_Send_Byte(0x00, 0x01);   // Clear display
   delay_ms(lcd_fixed_delay_us);                // (per HD44780U initialization spec) 
   LCD_I2C_Send_Byte(0x00, 0x06);   // Set cursor to increment
   delay_us(lcd_fixed_delay_us);                // (per HD44780U initialization spec) 
   i2c_stop(SOFT_I2C);
   delay_us(lcd_fixed_delay_us);                            // Terminate I2C transfer
}

//===================================
void LCD_I2C_Write_Upper_Nibble(int u) 
{
// Send upper nibble
   if(bit_test(u,7))
      D7=1;
   else
      D7=0;
      
   if(bit_test(u,6))
      D6=1;
   else
      D6=0;

   if(bit_test(u,5))
      D5=1;
   else
      D5=0;
      
   if(bit_test(u,4))
      D4=1;
   else
      D4=0;
             
   E = 0;    
   i2c_write(SOFT_I2C,I2C_BYTE);
   E = 1;    
   i2c_write(SOFT_I2C,I2C_BYTE);
   E = 0;    
   i2c_write(SOFT_I2C,I2C_BYTE);
}

//===================================
void LCD_I2C_Write_Lower_Nibble(int l) 
{
// Send lower nibble
   if(bit_test(l,3))
      D7=1;
   else
      D7=0;
      
   if(bit_test(l,2))
      D6=1;
   else
      D6=0;

   if(bit_test(l,1))
      D5=1;
   else
      D5=0;
      
   if(bit_test(l,0))
      D4=1;
   else
      D4=0;

   E = 0;    
   i2c_write(SOFT_I2C,I2C_BYTE);
   E = 1;    
   i2c_write(SOFT_I2C,I2C_BYTE);
   E = 0;    
   i2c_write(SOFT_I2C,I2C_BYTE);  
}
