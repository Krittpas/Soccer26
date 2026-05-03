#include <POP32.h>  
void setup() 
{
    oled.textBackgroundColor(WHITE);  // กำหนดให้พื้นหลังของตัวอักษรเป็นสีขาว
    oled.textColor(BLACK);            // กำหนดให้ข้อความเป็นสีดำ
    oled.text(2,0,"Hello World");     // แสดงข้อความ Hello World
    oled.show();                      // ปรับปรุงการแสดงผล
}
void loop()
{}
