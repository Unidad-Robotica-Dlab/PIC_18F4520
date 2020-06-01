#include <18f4520.h>
#include <stdlib.h>

#use delay(clock=20M)
#fuses HS,NOWDT,NOPROTECT,BROWNOUT,PUT,NOLVP,NOMCLR,
#use rs232(baud=115200, xmit=PIN_C6, rcv=PIN_C7, BRGH1OK)
#include <math.h>

unsigned long tmr0=0;

int1 control_pid=0;
unsigned int32 ciclo=0;
#define BUFF_LEN 6
char string[BUFF_LEN];
int1 have_string=FALSE;

int8   enc=0; 
int8   aux=0;  


// Variables globales para el cálculo del controlador PID.
int1 q=0;
signed int16 Inp=0, Setpoint=0, out=0.0;
signed int16    ITerm=0, lastInput=0, dInput=0;
signed int16    kp=20, ki=30, kd=0;
signed int16 outMin=-255, outMax=255.0;
signed int16 error=0.0;
// Resto de variables.
unsigned int8   pwm=0;                                                
signed int16 contador=0;                                              
unsigned int16 tmr0_speed=195;
int32 ciclos=0;
int16 Ierror=0; 
int16 pos_con_maquina=100;
float pos_control=0;
float dt=0;
float bmp=40;            ////60 a 1 bmp
float volumen=100;
float recorrido=0;
float I=1,E=1,total=0;
int16  value[3];

int8 a0=0,a1=0,a2=0,a3=0;

void pot(){
set_adc_channel(0);a0=read_adc();printf("%u ",a0); 
set_adc_channel(1);a1=read_adc();printf("%u ",a1);
set_adc_channel(2);a2=read_adc();printf("%u ",a2);
set_adc_channel(3);a3=read_adc();printf("%u\n\r",a3);
}

void ciclo(int1 i,int1 p,int1 r){//i=ciclo+1 p=print serial r=reset ciclo
if(i){ciclos=ciclos+1;write_eeprom( 0,make8(ciclos,0));write_eeprom( 1,make8(ciclos,1));write_eeprom( 2,make8(ciclos,2));write_eeprom( 3,make8(ciclos,3));}
if(p){printf("%Lu \n\r",make32(read_eeprom(3),read_eeprom(2),read_eeprom(1),read_eeprom(0)) );}
if(r){ciclos=0;}
}

//////////////////////TMR0=51.2uS////////////////////////
#INT_TIMER0
void timer0_isr(void)
{
tmr0++;
if(tmr0==tmr0_speed){ //20ms:390//10ms:195// 976=50ms
tmr0=0;
output_toggle(PIN_b2);
recorrido=((60/bmp)*85);
total=I+E;
E=recorrido/total*E;
I=recorrido/total*I;

if(dt<=1.57){dt+=1.57/I;}else{dt+=1.57/E; }
if(dt>=3.14){dt=0;
/////ciclo terminado//////
output_toggle(PIN_a4);
ciclo(1,0,0);
pot();


//////////////////////////
}
pos_control=(pos_con_maquina*sin(dt));



//printf("%f\n\r",pos_control);

///////////////////SERIAL RX COMMAND///////////////////
//////////////////////SET COMMAND//////////////////
if (have_string){
output_toggle(PIN_b0);
have_string=FALSE;
if(string[0]=='t'){tmr0_speed=atol(&string[1]);printf("\ftmr0_speed:%Lu\n\r",tmr0_speed); }
if(string[0]=='A'){printf("\fTMR0:%Lu Setpoint:%Lu kp:%Lu Ki:%Lu Kd:%Lu\n\r",tmr0_speed,Setpoint,kp,ki,kd); }
if(string[0]=='c'){contador=atol(&string[1]);printf("\fContador:%Lu\n\r",contador);}
if(string[0]=='s'){Setpoint=atol(&string[1]);printf("\fSet_point:%Lu\n\r",Setpoint);}
if(string[0]=='p'){kp=atol(&string[1]);printf("\fKp:%Lu\n\r",kp); }
if(string[0]=='i'){ki=atol(&string[1]);printf("\fKi:%Lu\n\r",ki); }
if(string[0]=='d'){kd=atol(&string[1]);printf("\fKd:%Lu\n\r",kd); }
if(string[0]=='q'){q=atol(&string[1]);printf("\fq:%u\n\r",q);}
if(string[0]=='R'){ciclo(0,0,1);printf("\fciclos:%Lu\n\r",ciclos);}
if(string[0]=='P'){control_pid=atol(&string[1]);printf("\fPID:%u\n\r",control_pid);}
if(string[0]=='B'){bmp=atol(&string[1]);printf("\fbmp :%f\n\r",bmp);}
if(string[0]=='I'){I=atol(&string[1]);printf("\fI :%f\n\r",I);}
if(string[0]=='E'){E=atol(&string[1]);printf("\fE :%f\n\r",E);}

}
////////////////////////////////////////////////////////
/////////////////////COMPUTE PID//////////////////////////
if(control_pid){
output_toggle(PIN_b3);
Inp =contador;                                              
error = (Setpoint - Inp);
if ((dInput == 0.0) || (error == 0.0)) ITerm += (error / ki); else  ITerm -= (error / ki); 
//if (ITerm > outMax) ITerm = outMax; else if (ITerm < outMin) ITerm = outMin;   
out =  (error *Kp + ITerm*Ki) ;                                 
if (out > outMax) out = outMax; else if (out < outMin) out = outMin;
lastInput = Inp;
/////////////////////////////Integracion de error por ciclo///////////////////////
if(Setpoint==0){
//printf("%Ld \n\r",Ierror);
Ierror=0;
}
else{if(error>0){Ierror+=error;}else{Ierror+=error*-1;}}
//////////////////////////////////////////////////////////////////////////////////

}
}
}


#INT_RB
void  RB_isr(void){
aux=enc;enc=input_b()&0b00110000;
if ((aux==0b00100000)&&(enc==0b00110000)){contador=contador+1;}
if ((aux==0b00110000)&&(enc==0b00100000)){contador=contador-1;}
}



//////////////////////INT_SERIAL_Byte/////////////////////////
#INT_RDA
void  RDA_isr(void){
static int ctr=0;
char ch;
ch=getc();
if (ch=='\n'){
string[ctr]='\0';
ctr=0;
have_string=TRUE;
return;
}
string[ctr++]=ch;
if (ctr==BUFF_LEN)
ctr--; 
}
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
               
void main(void)
{  
setup_adc(ADC_CLOCK_DIV_32);
setup_adc_ports(AN0_TO_AN3|vss_vdd ); 

setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1|RTCC_8_bit);                         
setup_timer_2(T2_DIV_BY_1, 255, 1);                                
setup_ccp1(CCP_PWM);    
setup_ccp2(CCP_PWM);                                                   
enable_interrupts(INT_TIMER0);  
enable_interrupts(INT_RB);
enable_interrupts(INT_RDA);
enable_interrupts(GLOBAL);
port_b_pullups(TRUE);

control_pid=1;

while(true)
{
output_toggle(PIN_b1);
delay_ms(100);
Setpoint=(int16)pos_control;
//printf("%Ld \n\r",Ierror);
//printf("%Ld %Ld\n\r",(int16)pos_control,contador);

      if (error == 0.0)                                                
      {
         set_pwm1_duty(0);
         set_pwm2_duty(0);
         output_low(pin_c0); 
        
                                                    
      }
      else
      {
        
         output_high(pin_c0);                                               
         
         pwm=(int8)abs(out);                                               
         
       
         
         if (out > 0.0)                                     
         {
            set_pwm1_duty(0);
            set_pwm2_duty(pwm);
         }
         else                                                               
         {
            set_pwm1_duty(pwm);
            set_pwm2_duty(0);
         }
      }
                        

}
}


