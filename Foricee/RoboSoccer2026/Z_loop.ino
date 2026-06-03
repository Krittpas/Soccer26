/* ─── loop() ──────────────────────────────────────────── */
void loop() {
  // ── Priority 1: white-line boundary (timed escape) ────
  static uint32_t lineEscapeUntil = 0;
  static uint32_t lineHoldoffUntil = 0;

  if (LineD()) lineEscapeUntil = millis() + LINE_ESCAPE_MS;

  if (millis() < lineEscapeUntil) {
    avoidLine();
    lineHoldoffUntil = millis() + LINE_HOLDOFF_MS;
    return;
  }
  if (millis() < lineHoldoffUntil) {
    getIMU();
    headZero(0.f, 0.f);
    return;
  }

  // ── Read sensors ──────────────────────────────────────
  updateVision();
  getIMU();

  // ── SW_OK: pause / re-zero mid-game ───────────────────
  if (SW_OK()) {
    stopAll();
    sound(800, 100);
    oled.clear();
    oled.text(1, 0, "== PAUSED ==");
    oled.text(3, 0, "A:ReZero");
    oled.text(4, 0, "B:Resume");
    oled.show();
    while (!SW_B()) {
      if (SW_A()) autoZero();
      getIMU();
      oled.text(5, 0, "Yaw:%f   ", pvYaw);
      oled.show();
    }
    oled.clear();
    oled.show();
    return;
  }

  // ── DEFEND2: slide ปิดมุมก่อน switch attack ─────────────
  if (mode == 5 && gDefendPhase) {
    if ((ballSeen && ballPosY > DEFEND2_BALL_Y) || millis() > gDefendUntilMs) {
      gDefendPhase = false;  // บอลเข้าใกล้หรือหมดเวลา → เปลี่ยนเป็น attack
    } else {
      if (ballSeen) {
        float err = 160.f - (float)ballPosX;
        if (fabsf(err) >= (float)DEFEND2_CENTER_DEAD) {
          float spd = constrain(fabsf(err) * DEFEND2_SLIDE_KP, 0.f, DEFEND2_SLIDE_MAX);
          holonomic(spd, (err > 0.f) ? 180.f : 0.f, headingOmega());
        } else {
          holonomic(0.f, 0.f, headingOmega());
        }
      } else {
        holonomic(0.f, 0.f, headingOmega());
      }
      return;
    }
  }

  // ── Attack (mode 1–5) ────────────────────────────────────
  if (ballSeen) {
    // if (mode == 3) doExtra2();
    // else
    doChase();
  } else {
    doSearch();
  }

  // drawGameDebug();
}
