/* ══════════════════════════════════════════════════════
   I_penalty.ino — Penalty kick (mandatory Round 2+)
   ══════════════════════════════════════════════════════

   Sequence:
   1. Center the ball in X (slide left/right)
   2. Confirm heading straight (headingOmega)
   3. Push forward PENALTY_PUSH_MS ms
   4. Fire + reload
   5. Hold position (while(1))
   ══════════════════════════════════════════════════════ */

void penaltyKick() {
  sound(500, 250);

  while (true) {
    updateVision();
    getIMU();

    if (ballSeen) {
      float rotErr = 160.f - (float)ballPosX;    // +ve = ball right, slide left
      float slideSpd = constrain(fabsf(rotErr) * 0.9f, 0.f, 80.f);
      float slideDir = (rotErr > 0.f) ? 180.f : 0.f;   // move toward ball X

      holonomic(slideSpd, slideDir, headingOmega());

      // Centered enough
      if (fabsf(rotErr) <= 6.f) {
        stopAll();

        // Push into ball
        uint32_t tPush = millis();
        while (millis() - tPush < (uint32_t)PENALTY_PUSH_MS) {
          if (LineD()) { stopAll(); break; }
          holonomic(85.f, 90.f, headingOmega());
        }
        stopAll();
        delay(50);
        shoot();
        stopAll();
        reload();

        // Freeze until reset
        while (true) { stopAll(); delay(200); }
      }
    } else {
      // No ball — spin slowly toward last known side
      holonomic(0.f, 0.f, headingOmega() + 20.f);
    }
  }
}
