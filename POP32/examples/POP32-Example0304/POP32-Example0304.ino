#include <POP32.h>  
void setup() 
{
  oled.mode(1);                // หมุนหน้าจอจากปกติ 90 องศา
  oled.dim(true);
  oled.text(2,0,"HelloWorld"); // แสดงข้อความ Hello World ที่ตำแหน่งซ้ายสุดของบรรทัด 2 (บรรทัดที่ 3)
  oled.show();                 // ปรับปรุงการแสดงผล
  delay(1000);
  oled.dim(false);
  oled.show();                 // ปรับปรุงการแสดงผล
}
void loop()
{}
