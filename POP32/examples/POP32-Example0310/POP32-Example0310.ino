#include <POP32.h>  
void setup() 
{}
void loop() 
{
  oled.text(0,0,"Hello POP-32");
  oled.show();  // ปรับปรุงการแสดงผล
  delay(2000);  // หน่วงเวลา 2 วินาที
  oled.clear(); // เคลียร์หน้าจอแสดงผล
  oled.show();  // ปรับปรุงการแสดงผล
  delay(2000);  // หน่วงเวลา 2 วินาที
}
