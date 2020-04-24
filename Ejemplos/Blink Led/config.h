#include <18f4520.h>
#use delay(clock=20M)
#fuses HS,NOWDT,NOPROTECT,BROWNOUT,PUT,NOLVP,NOMCLR,
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, BRGH1OK)