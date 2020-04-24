#include <config.h>
#include <lcd.c>


#define E_PWM pin_d3






void main()
{
setup_timer_1(T2_DIV_BY_1, 255, 1);
setup_ccp1(CCP_PWM);  

set_pwm1_duty(125);

while(1){




} 
}