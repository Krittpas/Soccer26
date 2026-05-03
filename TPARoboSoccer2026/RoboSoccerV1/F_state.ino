/* ══════════════════════════════════════════════════════
   §12  SEARCH — หมุนหาบอล
   ══════════════════════════════════════════════════════ */

void doSearch() {
  uint32_t searchStart = millis();

  for (int attempt = 0; attempt < 3; attempt++) {
    // ถอยกลับกลาง 270°
    uint32_t t = millis();
    while (millis() - t < 1000) {
      updateVision();
      if (ballSeen) return;
      if (LineD()) {
        avoidLine();
        break;
      }
      headZero(90, 270);
    }
    stopAll();
    if (millis() - searchStart > 3000) {
      headZero(90, 270);
      delay(800);
      break;
    }

    // กวาดซ้าย
    t = millis();
    while (millis() - t < 200) {
      updateVision();
      if (ballSeen) return;
      wheel(55, 55, 55);
    }
    stopAll();

    // คงทิศ
    t = millis();
    while (millis() - t < 350) {
      updateVision();
      if (ballSeen) return;
      headZero(0, 0);
    }
    stopAll();

    // กวาดขวา
    t = millis();
    while (millis() - t < 200) {
      updateVision();
      if (ballSeen) return;
      wheel(-55, -55, -55);
    }
    stopAll();

    // คงทิศ
    t = millis();
    while (millis() - t < 350) {
      updateVision();
      if (ballSeen) return;
      headZero(0, 0);
    }
    stopAll();
  }
}

/* ══════════════════════════════════════════════════════
   §13  MAIN CHASE LOGIC (kickState == 1)
   ══════════════════════════════════════════════════════ */

void doChase() {
  getIMU();

  diffX = ballPosX - 160.0f;
  diffY = 220.0f - ballPosY;  // ยิ่ง diffY น้อย = บอลใกล้

  float dist = sqrtf(diffX * diffX + diffY * diffY);
  float moveSpd = constrain(dist, 35, 45);
  float angleDir = atan2f(diffY, diffX) * RAD2DEG;
  if (angleDir < 0) angleDir += 360.0f;
  float angleToBall = atan2f(diffY, diffX) * RAD2DEG;
  if (angleToBall < 0) angleToBall += 360.0f;

  headZero(moveSpd, fmod(angleDir + 360.0f, 360.0f));

  // เงื่อนไขเข้าโหมด goal()
  bool closeEnough = (dist <= 100) || ((mode == 4 || mode == 5) && dist <= 15);
  bool headingOK = fabsf(Yaw) <= ALIGN_ZONE;
  bool ballCentered = fabsf(diffX) <= 5;

  if (closeEnough && headingOK && ballCentered) {
    stopAll();

    // mode 4/5 = penalty setup (เดินซ้าย/ขวาก่อน)
    if (mode == 4 || mode == 5) {
      getIMU();
      float w = constrain(-Yaw * 1.5f, -100, 100);
      holonomic(60, (mode == 4) ? 180.0f : 0.0f, w);
      delay(650);
      stopAll();
      mode = 1;
    }

    goal(cg);
    sound(1250, 500);
  }
}