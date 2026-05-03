#include <POP32.h>  
void setup()
{}
void loop() 
{
  oled.text(0,0,"Knob: %d   ",knob());  // อ่านค่าข้อมูลจาก KNOB มาแสดงที่จอแสดงผล
  oled.show();  // ปรับปรุงการแสดงผล
}
