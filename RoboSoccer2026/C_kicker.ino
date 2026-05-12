/* ══════════════════════════════════════════════════════
   C_kicker.ino — Motor 4 kicker + limit switch
   ══════════════════════════════════════════════════════ */

/* ── Single fire stroke ───────────────────────────────── */
void shoot() {
  motor(4, RELOAD_SPD);
  delay(220);
  motor(4, 0);
  delay(60);
}

/* ── Pull rod back to home (limit switch) ─────────────── */
void reload() {
  motor(4, RELOAD_SPD);
  uint32_t t = millis();

  while (millis() - t < (uint32_t)RELOAD_MS) {
    if (analog(LIM_PIN) > LIM_TRIG) break;
    delay(1);
  }

  // If stuck: push forward briefly then retry
  if (analog(LIM_PIN) <= LIM_TRIG) {
    motor(4, -RELOAD_SPD);
    delay(300);
    motor(4, RELOAD_SPD);
    t = millis();
    while (millis() - t < (uint32_t)RELOAD_MS) {
      if (analog(LIM_PIN) > LIM_TRIG) break;
      delay(1);
    }
  }

  motor(4, 0);
  kickReady = 1;
}

/* ── Full kick sequence: 2 shots + reload ─────────────── */
void kick() {
  kickReady = 0;
  reload();
  shoot();
  stopAll();
  delay(80);
  reload();
  reload();
  shoot();
  reload();
  reload();
  kickReady = 1;
}
