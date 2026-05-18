/* ══════════════════════════════════════════════════════
   Z_setup.ino — Setup menu + main loop()
   ══════════════════════════════════════════════════════ */

/* ─── Mode names (index 0 = mode 1) ──────────────────── */
static const char* modeNames[] = {
  "ATTACK  ", "DEFEND  ", "EXTRA   ",
  "LEFT-PEN", "RGHT-PEN", "PENALTY ",
  "TEST-KCK", "SEN-CHK ", "BAL-RAD"
};

/* ─── Setup screen ────────────────────────────────────── */
void drawSetupScreen() {
  oled.clear();

  // Title bar (inverted)
  // oled.fillRect(0, 0, 10, 64, WHITE);   // left accent bar
  oled.fillRect(0, 0, 128, 10, WHITE);
  oled.setTextColor(BLACK);
  oled.setCursor(8, 1);
  oled.print("TPA ROBOSOCCER 2026");
  oled.setTextColor(WHITE);

  // Yaw (with mini compass bar)
  oled.setCursor(2, 14);
  oled.print("YAW:");
  oled.setCursor(30, 14);
  oled.print(pvYaw, 1);
  // Yaw bar (±90° → 40px)
  oled.drawRect(2, 22, 60, 5, WHITE);
  int yp = 32 - (int)(pvYaw * 30.f / 90.f);
  yp = constrain(yp, 3, 61);
  oled.fillRect(yp - 1, 23, 3, 3, WHITE);
  oled.drawFastVLine(32, 22, 5, WHITE);  // zero

  // Goal color
  oled.drawLine(68, 11, 68, 32, WHITE);
  oled.setCursor(70, 14);
  oled.print("GOAL:");
  oled.setCursor(70, 23);
  if (cg == SIG_GOAL_YEL) {
    oled.fillRect(98, 21, 26, 8, WHITE);
    oled.setTextColor(BLACK);
    oled.setCursor(100, 22);
    oled.print("YEL");
    oled.setTextColor(WHITE);
  } else {
    oled.drawRect(98, 21, 26, 8, WHITE);
    oled.setCursor(100, 22);
    oled.print("BLU");
  }

  // Mode
  oled.drawLine(0, 32, 128, 32, WHITE);
  oled.setCursor(2, 35);
  oled.print("MODE:");
  oled.setCursor(40, 35);
  oled.print(modeNames[mode - 1]);

  // Knob progress bar
  oled.drawLine(0, 45, 128, 45, WHITE);
  int barW = ((knob(7) + 1) * 116) / 8;  // 9 modes now
  oled.fillRect(6, 48, barW, 8, WHITE);
  oled.drawRect(6, 48, 116, 8, WHITE);

  // Hints
  oled.setCursor(2, 57);
  oled.print("A:Zero B:Run OK:Goal");

  oled.show();
}

/* ─── setup() ─────────────────────────────────────────── */
void setup() {
  Serial1.begin(115200);
  delay(2500);

  reload();

  while (!huskylens.begin(Wire)) {
    oled.clear();
    oled.text(0, 0, "HuskyLens FAIL!");
    oled.show();
    delay(300);
  }

  mode = 1;
  cg = SIG_GOAL_YEL;

  // ── Setup menu loop ────────────────────────────────────
  while (!SW_B()) {
    getIMU();

    if (SW_A()) { autoZero(); }

    if (SW_OK()) {
      cg = (cg == SIG_GOAL_YEL) ? SIG_GOAL_BLU : SIG_GOAL_YEL;
      delay(250);
    }

    mode = constrain(knob(8) + 1, 1, 9);
    drawSetupScreen();
  }

  oled.clear();
  oled.text(2, 3, modeNames[mode - 1]);
  oled.show();

  // ── Mode 8: SensorCheck ───────────────────────────────
  if (mode == 8) {
    oled.clear();
    oled.show();
    while (true) {
      getIMU();
      drawSensorCheck();
      // SW_A = ปรับ LINE_REF ขึ้น 50, SW_B = ออก
      if (SW_B()) break;
    }
    // reset กลับ mode 1 เพื่อเริ่มเกม
    mode = 1;
    oled.clear();
    oled.show();
  }

  // ── Mode 9: BallRadar ─────────────────────────────────
  if (mode == 9) {
    oled.clear();
    oled.show();
    while (true) {
      updateVision();
      getIMU();
      drawBallRadar();
      if (SW_B()) break;
    }
    mode = 1;
    oled.clear();
    oled.show();
  }

  // ── Mode 2: Defend delay ───────────────────────────────
  if (mode == 2) {
    uint32_t tDF = millis();
    while (millis() - tDF < 4500) {
      updateVision();
      if (ballSeen && ballPosY > 115) break;
    }
  }

  // ── Mode 3: Extra delay ────────────────────────────────
  else if (mode == 3) {
    delay(1000);
  }

  // ── Mode 6: Penalty (ไม่กลับมา loop) ─────────────────
  else if (mode == 6) {
    sound(1500, 100);
    penaltyKick();
    while (true)
      ;
  }

  // ── Mode 7: Test kicker ────────────────────────────────
  else if (mode == 7) {
    oled.clear();
    oled.text(0, 0, "A=shoot");
    oled.text(1, 0, "B=reload");
    oled.text(2, 0, "OK=kick");
    oled.show();
    while (true) {
      if (SW_A()) shoot();
      if (SW_B()) reload();
      if (SW_OK()) kick();
    }
  }

  if (!kickReady) reload();

  SetPoint = pvYaw;
  x_pErr = 0.f;
  x_i = 0.f;

  sound(1500, 120);
  oled.clear();
  oled.text(1, 3, "[BenChan]  GO!");
  oled.show();
}