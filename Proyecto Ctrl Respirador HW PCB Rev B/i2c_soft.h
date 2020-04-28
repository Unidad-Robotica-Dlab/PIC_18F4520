/*****************************************************
Project : Libray
Version : 1.0
Date    : 14/07/2020
Author  : 
Adapter : Eduardo Illescas C
Company :
Comments:
Chip type               : PIC 18F4520
Program type            : i2c Soft Driver
*****************************************************/

#define LCD_SDA  PIN_C6
#define LCD_SCL  PIN_C7
#use i2c(MASTER, SLOW, SCL=LCD_SCL, SDA=LCD_SDA, FORCE_SW, STREAM=SOFT_I2C)