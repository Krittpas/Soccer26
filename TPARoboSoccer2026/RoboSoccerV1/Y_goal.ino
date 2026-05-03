/* ══════════════════════════════════════════════════════
   §10  GOAL SEQUENCE — วิ่งเข้าประตู + ยิง
   ══════════════════════════════════════════════════════ */

void goal(int goalColor) {
  bool done = false;
  uint32_t startTime = millis();

  while (!done) {
    // Boundary check ภายใน goal loop
    // if (LineD()) {
    //   avoidLine();
    //   return;
    // }

    updateVision();
    getIMU();

    if (ballSeen) {
      // ─── เห็นทั้งบอลและประตู ───
      if (goalSeen) {
        int ball_err = 160 - ballPosX;
        int goal_err = 160 - goalPosX;

        // คำนวณทิศวิ่งจากตำแหน่งบอล
        float dx = ballPosX - 160.0f;
        float dy = (float)ballPosY;
        float distToBall = sqrtf(dx * dx + dy * dy);
        float angleDir = atan2f(dy, dx) * RAD2DEG;
        if (angleDir < 0) angleDir += 360.0f;

        // turnSpeed ปรับตาม goal_error + distance factor
        float distFactor = constrain(distToBall / 160.0f, 0.25f, 1.0f);
        float turnSpeed = constrain(goal_err * GOAL_KP * distFactor, -100, 100);

        holonomic(SPD_GOAL, angleDir, turnSpeed);

        // ถ้าตำแหน่งดีพอ — rush + ยิง!
        if (abs(ball_err) <= 30 && abs(goal_err) <= 40) {
          uint32_t pushT = millis();
          while (millis() - pushT < 800) {
            if (S1() || S2() || S5()) break;
            holonomic(SPD_PUSH, 90, 0);
          }
          stopAll();
          kick();
          done = true;
        }

        // ─── เห็นบอลแต่ไม่เห็นประตู — ยิงมั่ว ───
      } else {
        uint32_t pushT = millis();
        while (millis() - pushT < 800) {
          if (S1() || S2() || S5()) break;
          holonomic(SPD_PUSH, 90, 0);
        }
        stopAll();
        kick();
        done = true;
      }
    } else {
      // ไม่เห็นบอลแล้ว — หลุดออก
      break;
    }

    // Timeout
    if (millis() - startTime > GOAL_TIMEOUT_MS) {
      kick();
      break;
    }
  }
}