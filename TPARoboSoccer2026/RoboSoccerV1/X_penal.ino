/* ══════════════════════════════════════════════════════
   §11  PENALTY KICK (โหมด mode 6 / Onana)
   ══════════════════════════════════════════════════════ */

void penaltyKick() {
  sound(500, 300);

  while (true) {
    // if (LineD()) {
    //   avoidLine();
    //   return;
    // }

    updateVision();
    getIMU();

    if (ballSeen) {
      float rot_err = 160.0f - ballPosX;
      float moveSpd = constrain(fabsf(rot_err) * 1.0f, 0, 100);
      float moveDir2 = (rot_err > 0) ? 180.0f : 0.0f;
      float w = constrain(-Yaw * 0.75f, -80, 80);
      holonomic(moveSpd, moveDir2, w);

      if (fabsf(rot_err) <= 5) {
        stopAll();
        uint32_t t = millis();
        while (millis() - t < PENALTY_PUSH_MS) holonomic(80, 90, 0);
        stopAll();
        shoot();
        reload();
        while (1) {
          wheel(0, 0, 0);
        }
        break;
      }
    } else {
      // หมุนหาบอลที่จุดโทษ
      float w = constrain(-Yaw * 2.0f, -80, 80);
      holonomic(0, 0, w);
    }
  }
}