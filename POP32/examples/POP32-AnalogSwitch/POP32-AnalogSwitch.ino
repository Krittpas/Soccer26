#include <POP32.h>
void setup() 
{
  oled.textSize(2);
}
void loop() 
{
  int val = analog(0);
  oled.text(1,0,"Adc: %d   ",val);
  oled.show();
  if(val>2048)
  {
    out(16,1);
  }
  else
  {
    out(16,0);
  }
  
}
