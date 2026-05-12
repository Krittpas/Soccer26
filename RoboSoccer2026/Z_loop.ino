/* ─── loop() ──────────────────────────────────────────── */
void loop() {
  // ── Priority 1: boundary ──────────────────────────────
  if (LineD()) {
    avoidLine();
    return;
  }

  // ── Read sensors ──────────────────────────────────────
  updateVision();
  getIMU();

  // ── SW_OK pause menu (mid-game re-zero / test) ────────
  if (SW_OK()) {
    stopAll();
    sound(800, 100);
    oled.clear();
    oled.text(1, 0, "PAUSED");
    oled.text(3, 0, "A:ReZero  B:Resume");
    oled.show();
    while (!SW_B()) {
      if (SW_A()) { autoZero(); }
      getIMU();
      oled.text(5, 0, "Yaw:%.1f  ", pvYaw);
      oled.show();
    }
    oled.clear();
    oled.show();
    return;
  }

  // ── Mode 2 (Defend / Goalkeeper) ──────────────────────
  if (mode == 2) {
    if (ballSeen) {
      float rotErr = 150.f - (float)ballPosX;
      float w = constrain(rotErr * 1.0f, -100.f, 100.f);
      float sideDir = (w > 0.f) ? 180.f : 0.f;
      // Stay near goal line, slide to block
      headTarget(constrain(fabsf(w), 20.f, SPD_DEFEND),
                 sideDir, SetPoint);
    } else {
      headZero(0.f, 0.f);  // hold position
    }
    _oledDebug();
    return;
  }

  // ── Attack modes (1, 3, 4, 5) ─────────────────────────
  if (ballSeen) {
    doChase();
  } else {
    doSearch();
  }

  _oledDebug();
}

/* ─── OLED debug (bottom status line) ────────────────────
   Minimal: only updates every ~100 ms to avoid I2C blocking */
static uint32_t _lastOledMs = 0;
void _oledDebug() {
  if (millis() - _lastOledMs < 100) return;
  _lastOledMs = millis();
  oled.text(0, 0, "Y:%.1f M:%d G:%d  ", pvYaw, mode, cg);
  oled.text(1, 0, "B:%d(%3d,%3d)     ", (int)ballSeen, ballPosX, ballPosY);
  oled.text(2, 0, "G:%d gX:%3d       ", (int)goalSeen, goalPosX);
  oled.text(3, 0, "L:%d%d%d%d%d KR:%d  ",
            (int)S1(), (int)S2(), (int)S3(), (int)S4(), (int)S5(), kickReady);
  oled.show();
}
