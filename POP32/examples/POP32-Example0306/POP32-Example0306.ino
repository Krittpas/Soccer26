#include <POP32.h>  
void setup() 
{
    oled.textColor(BLACK,WHITE);  // กำหนดข้อความเป็นสีดำ พื้นหลังเป็นสีขาว
    oled.text(2,0,"Hello World"); // แสดงข้อความ Hello World
    oled.show();                  // ปรับปรุงการแสดงผล
}
void loop()
{}
