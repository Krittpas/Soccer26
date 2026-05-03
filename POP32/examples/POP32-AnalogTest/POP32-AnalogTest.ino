#include <POP32.h>
void setup() 
{
}
void loop() 
{
  int val = analog(0);
  float volts = (float(val)*3.3)/4096;
  oled.text(0,0,"Adc: %d   ",val);
  oled.text(2,0,"Volts: %f   ",volts);
  oled.show();
  
}
