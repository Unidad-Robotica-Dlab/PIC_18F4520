#include <config.h>

//
//Led Blink RB0
//
//
//

#define LED PIN_B0
#define DELAY 100



void main()
{

while(1){
output_low(LED);
delay_ms(DELAY);
output_high(LED);
delay_ms(DELAY);

} 
}