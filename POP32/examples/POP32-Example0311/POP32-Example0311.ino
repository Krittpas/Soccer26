#include <POP32.h>
void setup() 
{
  for(int i=0;i<128;i+=4)
  {
    oled.drawPixel(i,32,WHITE); // พล็อตจุดทุกๆ 4 พิกเซลในแนวแกน x กลางจอภาพ
  }
  for(int i=0;i<64;i+=4)
  {
    oled.drawPixel(64,i,WHITE); // พล็อตจุดทุกๆ 4พิกเซลในแนวแกน y กลางจอภาพ
  }
  oled.show();                  // ปรับปรุงการแสดงผล
}
void loop() 
{}
