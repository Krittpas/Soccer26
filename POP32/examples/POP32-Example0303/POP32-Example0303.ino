#include <POP32.h>  
void setup() 
{
  oled.mode(1);             // หมุนหน้าจอจากปกติ 90 องศา
  oled.text(2,0,"Hello");   // แสดงข้อความ Hello ที่ตำแหน่งซ้ายสุดของบรรทัด 2 (บรรทัดที่ 3)
  oled.show();              // ปรับปรุงการแสดงผล
}

void loop() 
{}
