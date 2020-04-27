#include <config.h>

void i2cScanner() {
   int8 count = 0;
   printf("\n\rStart I2C Scanner..\n\r");
   for(int8 i=0x10; i < 0xF0; i+=2) {
      i2c_start();
      if(i2c_write(i) == 0) { printf("ACK addr: 0x%X\n\r", i); count++;  }
      i2c_stop();
   }
   if(count == 0) {printf("Nothing Found\n\r"); }
   else { printf("Number of i2c chips found: %u\n\r", count); }

}

void main() {

i2cScanner();

while(1);

} 
