#include <config.h>

//
//rotary encoder LPD3806-400BM-G5-24C
//Puerto RB4=A RB5=B
//Puerto Serial232 xmit=PIN_C6 rcv=PIN_C7
//
#define DELAY 10
signed int16 contador=0;

int8   enc=0; 
int8   aux=0;  


#INT_RB
void  RB_isr(void) 
{
aux=enc;
enc=input_b()&0b00110000;
if ((aux==0b00100000)&&(enc==0b00110000)){contador++;}
if ((aux==0b00110000)&&(enc==0b00100000)){contador--;}
}


void main()
{

enable_interrupts(INT_RB);
enable_interrupts(GLOBAL);
port_b_pullups(TRUE);



while(1){
delay_ms(DELAY);
printf("Pos: %Ld \n\r", contador);//print al puerto serial

} 
}