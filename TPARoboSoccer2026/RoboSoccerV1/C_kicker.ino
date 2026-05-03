/* ══════════════════════════════════════════════════════
   §8  KICKER — Motor 4 + limit switch
   ══════════════════════════════════════════════════════ */

void shoot() {
  motor(4, reloadSpd);
  delay(250);
  motor(4, 0);
  delay(50);
}

void reload() {
  motor(4, 60);
  uint32_t t = millis();
  int cnt = 0;
  while (millis() - t < RELOAD_MAX_MS) {
    cnt++;
    if (analog(LIM_PIN) > LIM_TRIG) break;
    delay(1);
  }
  if (cnt >= RELOAD_MAX_MS) {  // ก้านติด — ดันออกก่อน
    motor(4, -65);
    delay(250);
    motor(4, 60);
    t = millis();
    while (millis() - t < RELOAD_MAX_MS) {
      if (analog(LIM_PIN) > LIM_TRIG) break;
      delay(1);
    }
  }
  motor(4, 0);
}

// ยิงสองรอบ + reload ให้พร้อม
void kick() {
  reload();
  shoot();
  stopAll();
  reload();
  reload();
  shoot();
  reload();
  reload();
  kickState = 1;
}