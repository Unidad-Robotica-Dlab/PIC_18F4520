#include <config.h>



#define DELAY 50
#define E_PWM pin_c0
#define power 125


void control_motor(int1 dir,int8 pot){
if(dir){set_pwm1_duty(0);set_pwm2_duty(pot);}
else{set_pwm1_duty(pot);set_pwm2_duty(0);}
}

void main()
{
output_high(E_PWM);

setup_ccp1(CCP_PWM);
setup_ccp2(CCP_PWM);
setup_timer_2(T2_DIV_BY_1, 255, 1);


while(1){
for(int i=0;i<power;i++){
control_motor(0,i);
delay_ms(DELAY);
printf("value: %u \n\r", i);
}
delay_ms(1000);
for(int i=0;i<power;i++){
control_motor(1,i);
delay_ms(DELAY);
printf("value: %u \n\r", i);
}
delay_ms(1000);

} 
}


