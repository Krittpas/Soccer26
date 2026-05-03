void shoot() {
  motor(4, reloadSpd);
  delay(250);
  motor(4, 0);
  delay(50);
}

void reload() {
  motor(4, 65);
  int timer = 0;
  for (timer = 0; timer < 800; timer++) {
    if (analog(limPin) > 1000) break;
    delay(1);
  }
  if (timer >= 750) {      // ถ้าก้านยิงติด
    motor(4, 65);  // เลื่อนก้านยิงไปข้างหน้า
    delay(375);            //ก่อน 0.5 วินาที
    motor(4, 65);
    for (timer = 0; timer < 1000; timer++) {
      if (analog(limPin) > 1000) break;
      delay(1);
    }
  }
  motor(4, 0);
}

void kick() {
  reload();
  shoot();
  wheel(0, 0, 0);
  reload();
  reload();
  shoot();
  reload();
  reload();
  kickState = 1;
}
