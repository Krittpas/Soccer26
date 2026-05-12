/* ══════════════════════════════════════════════════════
   Z_setup.ino — Setup menu + main loop()
   ══════════════════════════════════════════════════════ */
   
/* ─── setup() ─────────────────────────────────────────── */
void setup() {
  Serial1.begin(115200);
  delay(2500);  // let IMU boot

  reload();  // kicker to home

  // HuskyLens init
  /*while (!huskylens.begin(Wire)) {oled.text(0, 0, "HuskyLens FAIL!"); oled.show(); delay(300); }*/

  // ── Setup menu (exit on SW_B) ──────────────────────────
  mode = 1;
  cg = SIG_GOAL_YEL;

  while (!SW_B()) {
    getIMU();

    // SW_A → re-zero IMU
    if (SW_A()) {
      autoZero();
    }
    // SW_OK → toggle goal color
    if (SW_OK()) {
      cg = (cg == SIG_GOAL_YEL) ? SIG_GOAL_BLU : SIG_GOAL_YEL;
      delay(250);
    }

    // Knob selects mode (0-based knob → 1-based mode)
    mode = constrain(knob(7) + 1, 1, 7);

    drawSetupScreen();
  }

  // ── Mode-specific startup delay ────────────────────────
  oled.clear();
  oled.text(2, 5, modeNames[mode - 1]);
  oled.show();

  if (mode == 2) {
    // Defend: wait for ball to come into view, max 4.5 s
    uint32_t tDF = millis();
    while (millis() - tDF < 4750) {
      updateVision();
      if (ballSeen && ballPosY > 115) break;
    }
  } else if (mode == 3) {
    delay(1000);
  } else if (mode == 6) {
    // Penalty — skip loop(), run dedicated function
    sound(1500, 100);
    penaltyKick();
    while (true)
      ;  // never returns
  } else if (mode == 7) {
    // Test kicker
    oled.text(4, 0, "A=shoot B=reload OK=kick");
    oled.show();
    while (true) {
      if (SW_A()) shoot();
      if (SW_B()) reload();
      if (SW_OK()) kick();
    }
  }

  // Ensure kicker is loaded
  if (!kickReady) reload();

  SetPoint = pvYaw;  // lock current heading as reference
  x_pErr = 0.f;
  x_i = 0.f;

  sound(1500, 120);
  oled.clear();
  oled.text(1, 3, "[BenChan]  GO!");
  oled.show();

  while (1) {
    headZero(0.f, 0.f);
  }
}