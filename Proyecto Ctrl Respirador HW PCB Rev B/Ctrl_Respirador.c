/*****************************************************
Project : Ctrl Respirador HW PCB Rev B
Version : 1.0
Date    : 14/07/2020
Author  : Eduardo Illescas
Company :
Comments:
Chip type               : PIC 18F4520
Program type            : Application
*****************************************************/
#include <18F4520.h>
#device ADC=10  
#device HIGH_INTS=TRUE
#device *=16
#use delay(clock=20000000) 
#use standard_io(A)
#use standard_io(B)
#use standard_io(C)
#use standard_io(D)
#use standard_io(E) 
#priority rtcc,timer1

#fuses HS,NOWDT,NOPROTECT,MCLR,PUT 

#include "lcd_i2c.c"
#include "ds1307.c"
#include "encoder.c"

#ZERO_RAM
#CASE

enum FUNTIONS
{
 ADC_TEST = 1,
 ENCODER_TEST,
 MOTOR_RIGHT_TEST,
 MOTOR_LEFT_TEST, 
 RTC_TEST,
 BUZZER_TEST,
 LEDS_TEST,
 HOME_SW_TEST,
 END_FUNTION
};

// Digital Output
#define PWM_ENABLE PIN_C0
#define TEST_OUT PIN_C5
// Digital Input
#define HOME_SW_MOTOR PIN_B3
// Analog Input
#define VR1 PIN_A0 // AUX5
#define VR2 PIN_A1 // AUX6
#define VR3 PIN_A2 // AUX7
#define VR4 PIN_A3 // AUX8
#define VR5 PIN_A5 // AUX2
//Others 
#define HW_LED_ROJO PIN_B1
#define HW_LED_VERDE PIN_B4
#define HW_LED_NARANJA PIN_B5
#define HW_KEY_DOWN PIN_E1
#define HW_KEY_ENTER PIN_E0
#define HW_KEY_UP PIN_A4
#define HW_LCD_BLACKLIGHT PIN_E2
#define HW_BUZZER PIN_D3
#define TOTAL_ADC_CHANNEL 5
#define TIMER_1MS_TMR 64911
#define DELAY_KEY_TMR 10
#define SOFT_KEY_DOWN 0
#define SOFT_KEY_ENTER 1
#define SOFT_KEY_UP 2
#define RIGHT_MOTOR FALSE
#define LEFT_MOTOR TRUE

unsigned int16 keys[] = {HW_KEY_DOWN,HW_KEY_ENTER,HW_KEY_UP};
unsigned int16 leds[] = {HW_LED_ROJO,HW_LED_VERDE,HW_LED_NARANJA};
unsigned int32 tick;
char validKey;
char delayKey;
unsigned int16 dutyPwm_1;
unsigned int16 dutyPwm_2;
char hours;
char minutes;
char seconds;

#int_TIMER1 
void GENERAL_ISR()
{
 static char repiteKey = 255;
 char indexKey;
 char tempKey;
 char inputKey;
 char levelSw;
 
 if(--delayKey == 0)
 {
 	delayKey = DELAY_KEY_TMR;
 	for(indexKey = 0;indexKey < 3;indexKey++)
 	{      
  		if(!input(keys[indexKey]))
  		{
     		bit_set(inputKey, indexKey);
		}
		else
		{
 	 		bit_clear(inputKey, indexKey);
		}   
 	}
 	tempKey = inputKey;
 	inputKey &= repiteKey;
 	validKey = (inputKey ^ tempKey) | validKey;
 	repiteKey = tempKey;
 }
 tick++;
 set_timer1(TIMER_1MS_TMR);
}

void MotorOff()
{
 output_low(PWM_ENABLE);
 delay_ms(10);
 set_pwm1_duty(0);
 set_pwm2_duty(0);
}

void MotorOn(unsigned int16 duty,int1 direction)
{
  if(duty <= 1000)
  {
  	if(direction == RIGHT_MOTOR)
  	{
		set_pwm1_duty(duty);
  	}
  	else
  	{
        set_pwm2_duty(duty); 
  	}
  	output_high(PWM_ENABLE);
  }
}

#separate int1 EscapeKey()
{
 int1 success = FALSE;
 if(bit_test(validKey,SOFT_KEY_ENTER))
 {
  bit_clear(validKey,SOFT_KEY_ENTER);
  success = TRUE;
 }
 return success;
}

void main()
{
 char stateDemo = ADC_TEST;
 int1 funtionEnter;
 char indexTemp;
 unsigned long generalDelay;
 int16 voltageRaw;
 float voltage;
 char lcdCursor;
 

 port_b_pullups(TRUE); 
 setup_adc_ports(AN0_TO_AN4);// remover R11,R12,R28
 setup_adc(ADC_CLOCK_INTERNAL);
 setup_ccp1(CCP_PWM);
 setup_ccp2(CCP_PWM);
 MotorOff();
 output_low(HW_LED_ROJO);
 output_low(HW_LED_VERDE);
 output_low(HW_LED_NARANJA);
 output_low(HW_BUZZER);
 LCD_I2C_Init();
 ds1307_init();
 EncoderInit();
 delayKey = DELAY_KEY_TMR;
 tick = 0;	
 setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
 set_timer1(TIMER_1MS_TMR);
 setup_timer_2(T2_DIV_BY_16, 255, 1);
 enable_interrupts(INT_TIMER1);
 enable_interrupts(GLOBAL);
 LCD_I2C_Putc('\f');
 LCD_I2C_GotoXY(2,LCD_LINE1);
 printf(LCD_I2C_Putc,"%s","Test PCB ver 2"); 
 LCD_I2C_GotoXY(1,LCD_LINE2);
 printf(LCD_I2C_Putc,"%s","NewLeo X2 Rev B");
 delay_ms(3000);
 //ds1307_set_date_time(24,4,20,0,16,56,00);
 LCD_I2C_Putc('\f'); 
 do
 {
   LCD_I2C_GotoXY(1,LCD_LINE2);
   printf(LCD_I2C_Putc,"%s","Press Key - ");
   LCD_I2C_Putc(127); 
   printf(LCD_I2C_Putc,"%s"," +");
   switch(stateDemo)
   {
	case ADC_TEST:
		LCD_I2C_GotoXY(5,LCD_LINE1);
 		printf(LCD_I2C_Putc,"%s","ADC TEST"); 
        if(funtionEnter)
        {
            LCD_I2C_Putc('\f');
            while(!EscapeKey())
 			{	
				LCD_I2C_GotoXY(1,LCD_LINE1);
				for(indexTemp = 0;indexTemp < 3;indexTemp++)
				{
                  set_adc_channel(indexTemp);
                  voltageRaw = read_adc();
                  voltage = voltageRaw * (5.0/1023.0);
                  printf(LCD_I2C_Putc,"%u=%1.1f ",indexTemp+1,voltage);
                  delay_ms(100);
				}
				LCD_I2C_GotoXY(1,LCD_LINE2);
				for(indexTemp = 3;indexTemp < TOTAL_ADC_CHANNEL;indexTemp++)
				{
                  set_adc_channel(indexTemp);
                  voltageRaw = read_adc();
                  voltage = voltageRaw * (5.0/1023.0) ;
                  printf(LCD_I2C_Putc,"%u=%1.1f ",indexTemp+1,voltage);
                  delay_ms(100);
				}
			};
			funtionEnter = FALSE; 
            LCD_I2C_Putc('\f');
		}			      	
	break;
   	case ENCODER_TEST:
            LCD_I2C_GotoXY(3,LCD_LINE1);
 			printf(LCD_I2C_Putc,"%s","ENCODER TEST"); 
            if(funtionEnter)
            {
              encoderCounter = 0;  
			  LCD_I2C_ClearLine(LCD_LINE2);          
              while(!EscapeKey())
 			  {
            	LCD_I2C_GotoXY(1,LCD_LINE2);
            	printf(LCD_I2C_Putc,"%s","Account:");
            	LCD_I2C_GotoXY(12,LCD_LINE2);
            	printf(LCD_I2C_Putc,"%5Ld",encoderCounter);
			  };
			  funtionEnter = FALSE;
              LCD_I2C_Putc('\f'); 
            }
	break;
    case MOTOR_RIGHT_TEST:
			LCD_I2C_GotoXY(1,LCD_LINE1);
 			printf(LCD_I2C_Putc,"%s","RIGHT MOTOR TEST");
            if(funtionEnter)
			{
              LCD_I2C_ClearLine(LCD_LINE2);             
			  MotorOff();
              lcdCursor = 1;
			  dutyPwm_1 = 0;
              while(!EscapeKey())
              {
                LCD_I2C_GotoXY(lcdCursor,LCD_LINE2);
                LCD_I2C_Putc(255);
                MotorOn(dutyPwm_1,RIGHT_MOTOR);
                if(bit_test(validKey,SOFT_KEY_DOWN))
                {
                  if(lcdCursor > 1)
                  {                   
                   LCD_I2C_ClearChr(LCD_LINE2,lcdCursor,1);
                   dutyPwm_1 -= 62;
                   if(--lcdCursor == 1)
                   {
					dutyPwm_1 = 0;
				   }
				  }                
				  bit_clear(validKey,SOFT_KEY_DOWN);
				}
                else if(bit_test(validKey,SOFT_KEY_UP))
				{
					if(lcdCursor < 15)
                    {
                      lcdCursor++;                      
                      dutyPwm_1 += 62;	              
					}
 					else
                    { 
                      lcdCursor = 16;
					  dutyPwm_1 = 1000;
					} 
					bit_clear(validKey,SOFT_KEY_UP);
				} 
			  };        
              MotorOff();
              LCD_I2C_Putc('\f'); 
			  funtionEnter = FALSE;
			}			            
    break;
    case MOTOR_LEFT_TEST:
			LCD_I2C_GotoXY(1,LCD_LINE1);
 			printf(LCD_I2C_Putc,"%s","LEFT MOTOR TEST");
            if(funtionEnter)
			{
              LCD_I2C_ClearLine(LCD_LINE2);             
			  MotorOff();
              lcdCursor = 1;
			  dutyPwm_2 = 0;
              while(!EscapeKey())
              {
                LCD_I2C_GotoXY(lcdCursor,LCD_LINE2);
                LCD_I2C_Putc(255);
                MotorOn(dutyPwm_2,LEFT_MOTOR);
                if(bit_test(validKey,SOFT_KEY_DOWN))
                {
                  if(lcdCursor > 1)
                  {                   
                   LCD_I2C_ClearChr(LCD_LINE2,lcdCursor,1);
                   dutyPwm_2 -= 62;
                   if(--lcdCursor == 1)
                   {
					dutyPwm_2 = 0;
				   }
				  }                
				  bit_clear(validKey,SOFT_KEY_DOWN);
				}
                else if(bit_test(validKey,SOFT_KEY_UP))
				{
					if(lcdCursor < 15)
                    {
                      lcdCursor++;                      
                      dutyPwm_2 += 62;	              
					}
 					else
                    { 
                      lcdCursor = 16;
					  dutyPwm_2 = 1000;
					} 
					bit_clear(validKey,SOFT_KEY_UP);
				} 
			  };        
              MotorOff();
              LCD_I2C_Putc('\f'); 
			  funtionEnter = FALSE;
			}			            
    break;    
    case RTC_TEST:
		LCD_I2C_GotoXY(5,LCD_LINE1);
 		printf(LCD_I2C_Putc,"%s","RTC TEST");
        if(funtionEnter)
        {
 			LCD_I2C_ClearLine(LCD_LINE2);
            while(!EscapeKey())
            {
             LCD_I2C_GotoXY(5,LCD_LINE2);
             ds1307_get_time(hours,minutes,seconds);
             printf(LCD_I2C_Putc,"%02u:%02u:%02u",hours,minutes,seconds);
            };
            LCD_I2C_ClearLine(LCD_LINE2);
			funtionEnter = FALSE;
		}	 	
	break;
    case BUZZER_TEST:
		LCD_I2C_GotoXY(4,LCD_LINE1);
 		printf(LCD_I2C_Putc,"%s","BUZZER TEST");
        if(funtionEnter)
        {   
            LCD_I2C_ClearLine(LCD_LINE2);
			generalDelay = tick + 500;
            while(generalDelay > tick)
            {
              output_high(HW_BUZZER);
			};
            output_low(HW_BUZZER);
			funtionEnter = FALSE;
		}	 	
	break;
    case LEDS_TEST:
		LCD_I2C_GotoXY(4,LCD_LINE1);
 		printf(LCD_I2C_Putc,"%s","LEDS TEST");
        if(funtionEnter)
        {
			LCD_I2C_ClearLine(LCD_LINE2);
            while(!EscapeKey())
			{
              for(indexTemp = 0;indexTemp < 3;indexTemp++)
              {
                output_high(leds[indexTemp]);
                delay_ms(100);
                output_low(leds[indexTemp]);
                delay_ms(100);
			  }
			};
            output_low(HW_LED_ROJO);
 			output_low(HW_LED_VERDE);
 			output_low(HW_LED_NARANJA);
			funtionEnter = FALSE;
		}
    break;
    case HOME_SW_TEST:
		LCD_I2C_GotoXY(3,LCD_LINE1);
 		printf(LCD_I2C_Putc,"%s","HOME SW TEST");
        if(funtionEnter)
        {
            LCD_I2C_ClearLine(LCD_LINE2);
            while(!EscapeKey())
			{
               LCD_I2C_GotoXY(6,LCD_LINE2);
               if(input_state(HOME_SW_MOTOR))
               {
				printf(LCD_I2C_Putc,"%s","SW OFF");
			   }
               else
               {
				printf(LCD_I2C_Putc,"%s","SW ON ");
			   }
            };
            LCD_I2C_Putc('\f'); 
			funtionEnter = FALSE;
		}
	break;
	default:
	break;
   }
   if(bit_test(validKey,SOFT_KEY_UP))
   {
    if(++stateDemo == END_FUNTION)
    {
		stateDemo = ADC_TEST;
	}
	LCD_I2C_Putc('\f');
   	bit_clear(validKey,SOFT_KEY_UP);
   }
   else if(bit_test(validKey,SOFT_KEY_DOWN))
   {
	if(--stateDemo < ADC_TEST)
    {
		stateDemo = END_FUNTION - 1;
	}
	LCD_I2C_Putc('\f');
	bit_clear(validKey,SOFT_KEY_DOWN);
   }
   else if(bit_test(validKey,SOFT_KEY_ENTER))
   {
    funtionEnter = TRUE;
	LCD_I2C_Putc('\f');
	bit_clear(validKey,SOFT_KEY_ENTER);
   }
 }while(1);
}