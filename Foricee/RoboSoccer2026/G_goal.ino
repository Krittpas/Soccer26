/* ══════════════════════════════════════════════════════
   G_goal.ino — Goal alignment + shoot sequence
   ══════════════════════════════════════════════════════

   Called from doChase() when ball is in kicker range.

   Strategy:
   1. Both ball AND goal visible
      → steer toward ball with goal-alignment omega correction
      → fire immediately when ball+goal aligned (1 frame)
      → push toward ball's actual angle, then fire
   2. Only ball visible (goal not found)
      → fire only if |YawErr| <= ALIGN_DEG (heading acceptable)
      → hold heading and wait if facing wrong way
   3. Timeout (GOAL_TIMEOUT_MS) → fire as last resort
   4. Line detected during push → abort kick, let loop() escape
   ══════════════════════════════════════════════════════ */

/* ── Post-kick retreat ────────────────────────────────────
   ถ้าหลังยิงแล้ว goalPosY > RETREAT_GOAL_Y_THRESH (ใกล้โกลเกิน)
   → ถอยหลัง RETREAT_AFTER_KICK_MS ms
   → ถ้าเห็นบอลระหว่างนั้น: หยุดออก ให้ loop() chase ต่อ       */
static void postKickRetreat() {
  updateVision();
  if (!goalSeen || goalPosY < RETREAT_GOAL_Y_THRESH) return;

  uint32_t t = millis();
  while (millis() - t < (uint32_t)RETREAT_AFTER_KICK_MS) {
    updateVision();
    getIMU();
    if (ballSeen) return;
    // if (LineD()) {
    //   avoidLine();
    //   return;
    // }
    headZero(RETREAT_SPD, 270.f);
  }
  stopAll();
}

void goal() {
  uint32_t tStart = millis();
  bool done = false;

  // Escape-timer state
  static uint32_t lineEscapeUntil = 0;
  static uint32_t lineHoldoffUntil = 0;

  while (!done) {

    // ── Priority 1: line avoidance with timed escape ──────
    if (LineD()) {
      kick();
      lineEscapeUntil = millis() + LINE_ESCAPE_MS;
    }

    if (millis() < lineEscapeUntil) {
      avoidLine();
      lineHoldoffUntil = millis() + LINE_HOLDOFF_MS;
      return;  // ออกจาก goal() ให้ loop() จัดการ escape ต่อ
    }
    if (millis() < lineHoldoffUntil) {
      getIMU();
      headZero(0.f, 0.f);
      return;
    }

    // ── Read sensors ──────────────────────────────────────
    updateVision();
    getIMU();

    if (!ballSeen) break;  // lost ball — abort

    // ── Case A: goal visible → align then push ────────────
    if (goalSeen) {
      float dx = (float)(ballPosX - 160);
      float dy = (float)(ballPosY);
      float ballDist = sqrtf(dx * dx + dy * dy);
      float angleDir = atan2f(dy, dx) * RAD2DEG;
      if (angleDir < 0.f) angleDir += 360.f;
      int ballErr = 160 - ballPosX;
      int goalErr = 160 - goalPosX;
      float distFactor = constrain(ballDist / 160.f, 0.5f, 1.0f);
      float turnSpd = constrain((float)goalErr * GOAL_KP * distFactor,
                                -100.f, 100.f);

      holonomic(SPD_GOAL, angleDir, turnSpd);

      if (abs(goalErr) <= (int)GOAL_DEAD && abs(ballErr) <= (int)BALL_DEAD) {
        uint32_t tPush = millis();
        while (millis() - tPush < (uint32_t)GOAL_PUSH_MS) {
          if (LineD()) {
            kick();
            done = true;
            break;
          }
          holonomic(SPD_PUSH, angleDir, 0);
        }

        if (!done) {
          kick();
          tStart = millis();
          done = true;
        }
      }

      // ── Case B: goal not visible → fire only if heading OK ─
    } else {
      if (fabsf(YawErr) <= ALIGN_DEG) {
        uint32_t tPush = millis();
        while (millis() - tPush < (uint32_t)GOAL_PUSH_MS) {
          if (LineD()) {
            kick();
            done = true;
            break;
          }
          holonomic(SPD_PUSH, 90, 0);
        }

        if (!done) {
          kick();
          tStart = millis();
          done = true;
        }
      } else {
        headZero(0.f, 0.f);
      }
    }

    // ── Timeout: last-resort fire ─────────────────────────
    if (millis() - tStart > (uint32_t)GOAL_TIMEOUT_MS) {
      kick();
      done = true;
    }
  }

  // ── Post-kick retreat ─────────────────────────────────
  // if (done) postKickRetreat();
}
