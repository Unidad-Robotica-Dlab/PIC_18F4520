# Driver for Real Time Clock DS1307.C
                                           
ds1307_init() 
- Enable oscillator without clearing the seconds register 
-used when PIC loses power and DS1307 run from 3V BAT
 - Disable squarewave output 

ds1307_set_date_time(day,mth,year,dow,hour,min,sec)  Set the date/time  

ds1307_get_date(day,mth,year,dow)               Get the date 

ds1307_get_time(hr,min,sec)                     Get the time    
