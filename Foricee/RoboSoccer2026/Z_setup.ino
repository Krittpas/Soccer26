/* ══════════════════════════════════════════════════════
   Z_setup.ino — Setup menu + main loop()
   ══════════════════════════════════════════════════════ */

/* ─── Mode names (index 0 = mode 1) ──────────────────── */
static const char* modeNames[] = {
  "ATTACK  ", "EXTRA1  ", "EXTRA2  ",
  "DEFEND1 ", "DEFEND2 ", "ONANA   ",
  "RONALDO ", "SEN-CHK ", "BAL-RAD ",
  "GOAL-RAD", "TEST-KCK"
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
  int barW = ((knob(10) + 1) * 116) / 11;  // 11 modes
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
  delay(2500);  // IMU warm-up: wait for ZX-IMU to stabilize after power-on

  reload();

  {
    uint8_t camRetry = 0;
    while (!huskylens.begin(Wire)) {
      oled.clear();
      oled.text(0, 0, camRetry < 10 ? "HuskyLens FAIL!" : "NO CAM-Skipped");
      oled.show();
      delay(300);
      if (++camRetry >= 10) break;  // fallback: continue without camera
    }
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

    mode = constrain(knob(10) + 1, 1, 11);
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

  // ── Mode 9: BallRadar ────────────────────────────────
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

  // ── Mode 10: GoalRadar ────────────────────────────────
  if (mode == 10) {
    oled.clear();
    oled.show();
    while (true) {
      updateVision();
      getIMU();
      drawGoalRadar();
      if (SW_B()) break;
    }
    mode = 1;
    oled.clear();
    oled.show();
  }

  // ── Mode 4: DEFEND1 delay (break early if ball comes close) ──
  if (mode == 4) {
    uint32_t tDF = millis();
    while (millis() - tDF < 5000) {
      updateVision();
      if (ballSeen && ballPosY > 75) break;
    }
  }

  // ── Mode 2: EXTRA1 delay ───────────────────────────────
  else if (mode == 2) {
    delay(1750);
  }

  // ── Mode 3: EXTRA2 — Penalty ก่อน แล้วต่อด้วย Attack ─
  else if (mode == 3) {
    sound(1500, 100);
    ronaldo(true);  // Phase 1: penalty kick (return หลังยิง ไม่ freeze)
    mode = 1;       // Phase 2: เปลี่ยนเป็น ATTACK ให้ loop() ใช้ doChase()
  }

  // ── Mode 5: DEFEND2 — รอก่อน แล้ว Slide ปิดมุม แล้ว Attack ──
  else if (mode == 5) {
    uint32_t tDF = millis();
    while (millis() - tDF < 5000) {
      updateVision();
      getIMU();
      if (ballSeen && ballPosY > 75) break;
    }
    gDefendPhase = true;
    gDefendUntilMs = millis() + DEFEND2_SLIDE_MS;
  }

  // ── Mode 6: ONANA — Goalkeeper (ไม่กลับมา loop) ──────
  else if (mode == 6) {
    sound(1500, 100);
    onana();  // onana() loops forever internally
  }

  // ── Mode 7: RONALDO — Penalty Attacker (ไม่กลับมา loop)
  else if (mode == 7) {
    sound(1500, 100);
    ronaldo();
    while (true) {
      stopAll();
      delay(100);
    }  // freeze — penalty ended, ห้ามยิงซ้ำ (กติกาข้อ 38.8.5)
  }

  // ── Mode 11: Test kicker ──────────────────────────────
  else if (mode == 11) {
    oled.clear();
    oled.text(0, 0, "A  = shoot");
    oled.text(1, 0, "B  = reload");
    oled.text(2, 0, "OK = kick");
    oled.show();
    while (true) {
      oled.text(3, 0, "%d     ", analogRead(LIM_PIN));
      oled.show();
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