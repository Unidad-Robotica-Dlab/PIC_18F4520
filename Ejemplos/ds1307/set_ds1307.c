#include <config.h>

#include <ds1307.c>

void main()
{
  BYTE sec;
  BYTE min;
  BYTE hrs;
  BYTE day;
  BYTE month;
  BYTE yr;
  BYTE dow;
 
  ds1307_init();
 
  // Set date for -> 15 June 2005 Tuesday
  // Set time for -> 15:20:55

 // ds1307_set_date_time(27,4,20,1,15,06,00); //set time
 
  while(1)
  {
    delay_ms(1000);
   
    ds1307_get_date(day,month,yr,dow);
    ds1307_get_time(hrs,min,sec);
     
    printf("\f\%02d/\%02d/\%02d\r\n",day,month,yr);
    printf("\%02d:\%02d:\%02d", hrs,min,sec);
  }
}