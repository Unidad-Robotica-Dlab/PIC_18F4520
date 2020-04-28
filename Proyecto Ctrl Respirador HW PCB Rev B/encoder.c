/*****************************************************
Project : Libray
Version : 1.0
Date    : 14/07/2020
Author  : Eduardo Illescas C
Company :
Comments:
Chip type               : PIC 18F4520
Program type            : Encoder Driver
*****************************************************/

#define ENCODER_INPUT_A PIN_B0
#define ENCODER_INPUT_B PIN_B2

int16 encoderCounter;


void EncoderInit()
{
  setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4|RTCC_8_BIT);		
  enable_interrupts(INT_TIMER0);
  encoderCounter++;	
}

#int_RTCC 
void ENCODER_ISR()
{
 int8 grayCode;
 static int8 grayOld = 0;

 grayCode = input_state(ENCODER_INPUT_A);
 grayCode <<= 1;
 grayCode |= input_state(ENCODER_INPUT_B);
 if(grayOld == 0x02 && grayCode == 0x03)
 {
   encoderCounter++;
 }
 if(grayOld == 0x03 && grayCode == 0x02)
 {
   encoderCounter--;
 }
 grayOld = grayCode;
}