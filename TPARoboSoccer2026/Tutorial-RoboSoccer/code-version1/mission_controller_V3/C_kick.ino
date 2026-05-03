void shoot() {
  motor(4, reloadSpd);
  delay(125);
  motor(4, 0);
  delay(50);
}
int timer = 0;
void reload() {
  // while(1) {
  //   oled.text(1,1,"%d     " ,analogRead(limPin));
  //   oled.show();
  // }
  motor(4, reloadSpd);
  timer = 0;
  for (int i = 0; i < 1000; i++) {
    timer++;
    if (analogRead(limPin) > 500) break;
    delay(1);
  }
  if (timer == 1500) {     // ถ้าก้านยิงติด
    motor(4, -reloadSpd);  // เลื่อนก้านยิงไปข้างหน้า
    delay(250);            //ก่อน 0.5 วินาที
    motor(4, reloadSpd);
    timer = 0;
    for (int i = 0; i < 1000; i++) {
      timer++;
      if ((analogRead(limPin) > 500)) break;
      delay(1);
    }
  }
  motor(4, 0);
}