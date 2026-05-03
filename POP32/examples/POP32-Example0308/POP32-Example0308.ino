#include <POP32.h>  
void setup() 
{
  oled.textSize(1);       // กำหนดขนาดตัวอักษรเป็น 1 เท่า
  oled.text(0,0,"Size1"); // แสดงข้อความ Size1
  oled.textSize(2);       // กำหนดขนาดตัวอักษรเป็น 2 เท่า
  oled.text(1,0,"Size2"); // แสดงข้อความ Size2
  oled.textSize(3);       // กำหนดขนาดตัวอักษรเป็น 3 เท่า
  oled.text(2,0,"Size3"); // แสดงข้อความ Size3
  oled.show();            // ปรับปรุงการแสดงผล
}
void loop()
{}
