/* ══════════════════════════════════════════════════════
   F_chase.ino — Ball-capture AI (Attack mode)
   ══════════════════════════════════════════════════════

   Algorithm: Direct vector chase + EMA speed filter
   ─────────────────────────────────────────────────────
   1. คำนวณ ballAngle = atan2(diffY, diffX) → ทิศเคลื่อนที่
   2. speed = EMA(dist) × CHASE_SPD_SCALE  [SPD_MIN..SPD_MAX]
   3. Arrival zone: บอลใกล้ + centered → hold heading หรือ goal()
   4. holonomic(spd, ballAngle, headingOmega())

   Camera origin: (0,0) = top-left
   diffX = ballX - 160  (+ve = ball right of center)
   diffY = FWD_SP - ballY (+ve = ball needs to come closer)
   ══════════════════════════════════════════════════════ */

static float filtDist = 0.f;  // EMA-filtered distance to ball

void doChase() {
  getIMU();

  float diffX = (float)(ballPosX - 160);
  float diffY = (float)(FWD_SP - ballPosY);

  float dist = sqrtf(diffX * diffX + diffY * diffY);

  // EMA low-pass filter: smooth out camera noise on ball distance
  filtDist = 0.6f * filtDist + 0.4f * dist;

  float ballAngle = atan2f(diffY, diffX) * RAD2DEG;
  if (ballAngle < 0.f) ballAngle += 360.f;

  /* --- ARRIVAL ZONE: ball in range and centered --- */
  if (dist <= 50.f && fabsf(diffX) <= 5.f) {
    if (fabsf(YawErr) <= ALIGN_DEG) {
      stopAll();
      goal();
      sound(1200, 100);
      return;
    } else {
      headZero(0.f, 0.f);
      return;
    }
  }

  /* --- DIRECT VECTOR CHASE: speed proportional to filtered distance --- */
  float spd = constrain(filtDist * CHASE_SPD_SCALE,
                        (float)SPD_CHASE_MIN, (float)SPD_CHASE_MAX);
  holonomic(spd, ballAngle, headingOmega());
}

/* ══════════════════════════════════════════════════════
   doRotFling() — Two-phase rot+fling after search-found

   Phase 1: P-control — center ball X + approach Y
   Phase 2: Curve back to Yaw 0 → goal()
   ══════════════════════════════════════════════════════ */

void doRotFling() {
  int discoverState = 1;
  float lastYaw = 0.f;
  int count = 0;

  uint32_t tStart = millis();

  while (millis() - tStart < RFLING_TIMEOUT_MS) {
    getIMU();
    updateVision();

    if (LineD()) {
      avoidLine();
      return;
    }
    if (!ballSeen) return;

    if (discoverState == 1) {
      // ── Phase 1: P-only ──────────
      float rotErr = (float)(RFLING_SP_ROT - ballPosX);
      float fliErr = (float)(RFLING_SP_FLI - ballPosY);
      float rot_w = constrain(rotErr * RFLING_ROT_KP, -100.f, 100.f);
      float fli_spd = constrain(fliErr * RFLING_FLI_KP, -100.f, 100.f);

      if (fabsf(rotErr) < RFLING_ROT_GAP && fabsf(fliErr) < RFLING_FLI_GAP) {
        stopAll();
        lastYaw = pvYaw;
        discoverState = 0;
      } else {
        holonomic(fli_spd, 90.f, rot_w);
      }

    } else {
      // ── Phase 2: Curve กลับ Yaw 0 ──────────────────────
      float vecCurve = (lastYaw < 0.f) ? 0.f : 180.f;
      float radCurve = (lastYaw < 0.f) ? RFLING_CURVE_RAD : -RFLING_CURVE_RAD;
      holonomic(RFLING_CURVE_SPD, vecCurve, radCurve);

      if (fabsf(pvYaw) <= RFLING_YAW_TOL) { /*if (fabsf((float)(RFLING_SP_ROT - ballPosX)) < RFLING_ROT_GAP) count++; else count = 0; if (count >= RFLING_ALIGN_CNT) {*/
        stopAll();
        goal();
        return;
      }
    }
  }
  // Timeout → loop() จะ doChase() ต่อ*/
  return;
}

/* ══════════════════════════════════════════════════════
   doExtra2() — mode 3: Chase → Orbit-Aim → Push+Shoot

   State 0: Chase (EMA dist + ballAngle) จน ballPosY >= E2_ENTRY_Y
   State 1: Orbit รอบบอลโดยไม่ชน (strafe ตั้งฉาก + omega)
            goal อยู่กลาง E2_ALIGN_CNT frames หรือ timeout → State 2
   State 2: Push + kick() → reset State 0

   Orbit direction:
     goalPosX > 160  →  CW (+omega, strafe 180°)
     goalPosX ≤ 160  →  CCW (−omega, strafe 0°)
   ══════════════════════════════════════════════════════ */

void doExtra2() {
  static uint8_t e2State = 0;
  static float e2Omega = 0.f;
  static float e2Strafe = 0.f;
  static uint32_t e2Timeout = 0;
  static int e2AlignCnt = 0;
  static float e2FiltDist = 0.f;

  getIMU();
  if (!ballSeen) {
    e2State = 0;
    doSearch();
    return;
  }
  if (LineD()) {
    avoidLine();
    return;
  }

  // ── State 0: CHASE ─────────────────────────────────────────
  if (e2State == 0) {
    float diffX = (float)(ballPosX - 160);
    float diffY = (float)(FWD_SP - ballPosY);
    float dist = sqrtf(diffX * diffX + diffY * diffY);
    e2FiltDist = 0.6f * e2FiltDist + 0.4f * dist;

    if (ballPosY >= E2_ENTRY_Y) {
      if (goalSeen && goalPosX > 160) {
        e2Omega = E2_AIM_OMEGA;  // CW
        e2Strafe = 180.f;        // strafe ซ้าย
      } else {
        e2Omega = -E2_AIM_OMEGA;  // CCW
        e2Strafe = 0.f;           // strafe ขวา
      }
      e2Timeout = millis() + (uint32_t)E2_AIM_MS;
      e2AlignCnt = 0;
      e2State = 1;
      return;
    }

    float ballAngle = atan2f(diffY, diffX) * RAD2DEG;
    if (ballAngle < 0.f) ballAngle += 360.f;
    float spd = constrain(e2FiltDist * CHASE_SPD_SCALE,
                          (float)SPD_CHASE_MIN, (float)SPD_CHASE_MAX);
    holonomic(spd, ballAngle, headingOmega());

    // ── State 1: ORBIT + AIM ────────────────────────────────────
  } else if (e2State == 1) {
    holonomic(E2_AIM_STRAFE, e2Strafe, e2Omega);

    bool aligned = goalSeen && (abs(goalPosX - 160) < E2_GOAL_DEAD);
    if (aligned) e2AlignCnt++;
    else e2AlignCnt = 0;

    if (e2AlignCnt >= E2_ALIGN_CNT || millis() > e2Timeout) {
      e2State = 2;
    }

    // ── State 2: PUSH + SHOOT ───────────────────────────────────
  } else {
    stopAll();
    uint32_t tPush = millis();
    while (millis() - tPush < (uint32_t)E2_PUSH_MS) {
      holonomic(E2_PUSH_SPD, 90.f, 0.f);
    }
    kick();
    e2State = 0;
    e2FiltDist = 0.f;
    e2AlignCnt = 0;
  }
}
