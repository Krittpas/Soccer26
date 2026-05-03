/* ══════════════════════════════════════════════════════
   §15  MAIN LOOP
   ══════════════════════════════════════════════════════ */

void loop() {
  // ─── Boundary — highest priority ──────────────────
  if (LineD()) {
    avoidLine();
    return;
  }

  // ─── Read sensors ──────────────────────────────────
  updateVision();
  getIMU();

  // ─── Penalty kick mode ─────────────────────────────
  if (mode == 6) {
    penaltyKick();
    return;
  }

  // ─── Ball visible 
  if (ballSeen) {
    if (kickState == 1) {
      doChase();
    }
  }
  // ─── Ball NOT visible ──────────────────────────────
  else {
    doSearch();
  }

  // ─── OLED debug ───────────────────────────────────
  oled.text(0, 0, "Y:%.1f M:%d G:%d  ", pvYaw, mode, cg);
  oled.text(1, 0, "B:%d (%d,%d)       ", (int)ballSeen, ballPosX, ballPosY);
  oled.text(2, 0, "G:%d gX:%d         ", (int)goalSeen, goalPosX);
  oled.text(3, 0, "L:%d%d%d%d%d        ",
            (int)S1(), (int)S2(), (int)S3(), (int)S4(), (int)S5());
  oled.show();
}