/* ══════════════════════════════════════════════════════
   G_goal.ino — Goal alignment + shoot sequence
   ══════════════════════════════════════════════════════

   Strategy:
   1. Both ball AND goal visible → steer ball toward goal,
      fire when error < threshold.
   2. Only ball visible (no goal) → push straight, fire.
   3. Timeout → fire anyway.
   ══════════════════════════════════════════════════════ */

void goal(int goalColor) {
  uint32_t tStart = millis();
  bool done = false;

  while (!done) {
    if (LineD()) { avoidLine(); return; }

    updateVision();
    getIMU();

    if (!ballSeen) break;   // lost ball — abort

    float dx       = (float)(ballPosX - 160);
    float dy       = (float)(ballPosY);
    float ballDist = sqrtf(dx * dx + dy * dy);
    float angleDir = atan2f(dy, dx) * RAD2DEG;
    if (angleDir < 0.f) angleDir += 360.f;

    int  ballErr = 160 - ballPosX;

    if (goalSeen) {
      int goalErr = 160 - goalPosX;

      // Turn speed proportional to goal misalign, scaled by distance
      float distFactor = constrain(ballDist / 160.f, 0.2f, 1.0f);
      float turnSpd    = constrain((float)goalErr * GOAL_KP * distFactor,
                                   -100.f, 100.f);

      holonomic(SPD_GOAL, angleDir, turnSpd);

      // Both aligned → rush + kick
      if (abs(goalErr) <= (int)GOAL_DEAD && abs(ballErr) <= 35) {
        uint32_t tPush = millis();
        while (millis() - tPush < 600) {
          if (LineD()) break;
          holonomic(SPD_PUSH, 90.f, headingOmega());
        }
        stopAll();
        kick();
        done = true;
      }
    } else {
      // No goal visible — push straight and fire
      uint32_t tPush = millis();
      while (millis() - tPush < 500) {
        if (LineD()) break;
        holonomic(SPD_PUSH, 90.f, headingOmega());
      }
      stopAll();
      kick();
      done = true;
    }

    if (millis() - tStart > (uint32_t)GOAL_TIMEOUT_MS) {
      kick();
      done = true;
    }
  }

  stopAll();
}
