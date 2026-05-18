/* ─── loop() ──────────────────────────────────────────── */
void loop() {
  // ── Priority 1: white-line boundary ───────────────────
  if (LineD()) {
    avoidLine();
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

  // ── Mode 2: Goalkeeper ────────────────────────────────
  if (mode == 2) {
    if (ballSeen) {
      float rotErr = 150.f - (float)ballPosX;
      float w = constrain(rotErr, -100.f, 100.f);
      float sideDir = (w > 0.f) ? 180.f : 0.f;
      headTarget(constrain(fabsf(w), 20.f, (float)SPD_DEFEND),
                 sideDir, SetPoint);
    } else {
      headZero(0.f, 0.f);
    }
    drawGameDebug();
    return;
  }

  // ── Attack (mode 1, 3, 4, 5) ──────────────────────────
  if (ballSeen) doChase();
  else doSearch();

  // drawGameDebug();
}
