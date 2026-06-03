/* ══════════════════════════════════════════════════════
   C_kicker.ino — Motor 4 kicker + limit switch
   ══════════════════════════════════════════════════════ */

/* ── Single fire stroke ───────────────────────────────── */
void shoot() {
  if (!kickReady) return;  // prevent firing when rod is not loaded
  kickReady = 0;           // rod is now extended — must reload before next shot
  motor(4, 100);
  delay(225);
  motor(4, 0);
}

/* ── Pull rod back to home (limit switch) ─────────────── */
// void reload() {
//   for (uint8_t attempt = 0; attempt < 3; attempt++) {
//     motor(4, RELOAD_SPD);
//     uint32_t t = millis();
//     while (millis() - t < (uint32_t)RELOAD_MS) {
//       if (analog(LIM_PIN) > LIM_TRIG) {
//         delay(5);
//         if (analog(LIM_PIN) > LIM_TRIG) {
//           motor(4, 0);
//           kickReady = 1;
//           return;
//         }
//       }
//       delay(1);
//     }
//     motor(4, -RELOAD_SPD);
//     delay(250);
//   }
//   motor(4, 0);
//   kickReady = 0;
// }
void reload() {
  motor(4, 65);
  for (int timer = 0; timer < 800; timer++) {
    if (analog(LIM_PIN) > 1000) {
      motor(4, 0);
      kickReady = 1;
      return;
    }
    delay(1);
  }
  // ก้านยิงติด — nudge ถอยหลังก่อนแล้ว retry
  motor(4, -65);
  delay(200);
  motor(4, 65);
  for (int timer = 0; timer < 1000; timer++) {
    if (analog(LIM_PIN) > 1000) {
      motor(4, 0);
      kickReady = 1;
      return;
    }
    delay(1);
  }
  motor(4, 0);
  kickReady = 0;  // reload ไม่สำเร็จ
}

/* ── Full kick sequence: 2 shots + reload ─────────────── */
void kick() {
  shoot();  // kickReady must be 1 before calling kick()
  stopAll();
  reload();  // reload() sets kickReady = 1 when done
  shoot();
  reload();
}
