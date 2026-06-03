/* ══════════════════════════════════════════════════════
   I_penalty.ino — RONALDO: Penalty attacker
   ══════════════════════════════════════════════════════

   กติกา ข้อ 38:
   - วางหุ่นที่ใดในสนามก็ได้ (ข้อ 38.4)
   - ยิงได้ทันที หรือเลี้ยงบอลยิงจากนอกเขตโทษ (ข้อ 38.6.1)
   - ต้องยิงภายใน 10 วินาทีหลังกรรมการให้สัญญาณ

   Sequence:
   1. Slide ซ้าย/ขวาจนบอลอยู่กลางภาพ (Phase 1)
   2. เดินหน้าเข้าหาบอลจน ballPosY >= E2_ENTRY_Y (Phase 2)
   3. Orbit รอบบอล + รักษาระยะ Y จนหน้าตรงกลาง goal (Phase 3)
   4. Push + kick (Phase 4)
   5. Force kick ที่ 9 วินาที
   6. Line trigger → kick ทันทีแล้ว freeze
   ══════════════════════════════════════════════════════ */

void ronaldo(bool returnAfter = false) {
  sound(1500, 100);
  uint32_t tStart = millis();

  while (true) {
    if (millis() - tStart >= 9000UL) {
      kick();
      break;
    }

    updateVision();  // [Fix 2] อัปเดต vision ทุก loop — จำเป็นเพราะ ronaldo ไม่กลับไป loop()
    getIMU();

    if (!ballSeen) {  // [Fix 4] บอลหาย → hold heading รอบอลกลับมา
      headZero(0.f, 0.f);
      continue;
    }

    if (LineD()) {  // [Fix 5] เหยียบเส้นขาว → kick ทันทีแล้ว freeze
      kick();
      break;
    }

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
        uint32_t tGoal = millis();
        bool done = false;
        while (!done) {
          // ── Read sensors ──────────────────────────────────────
          updateVision();
          getIMU();

          if (!ballSeen) break;  // lost ball — abort

          // ── Case A: goal visible → curve orbit แล้ว kick ────────────
          if (goalSeen) {
            int goalErr = 160 - goalPosX;
            float vecCurve = (goalErr > 0) ? 0.f : 180.f;
            float radCurve = (goalErr > 0) ? RFLING_CURVE_RAD : -RFLING_CURVE_RAD;  // [Fix 1] ใช้ goalErr แทน lastYaw ที่ไม่ได้ declare
            holonomic(RFLING_CURVE_SPD, vecCurve, radCurve);

            if (fabsf(goalErr) <= 5) {
              stopAll();
              uint32_t tPush = millis();
              while (millis() - tPush < (uint32_t)GOAL_PUSH_MS) {
                if (LineD()) {
                  kick();
                  return;
                }
                holonomic(SPD_PUSH, 90, 0);
              }
              kick();  // ยิงหลัง push timer หมด (กรณีไม่เหยียบเส้น)
              return;
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
                done = true;
              }
            } else {
              headZero(0.f, 0.f);
            }
          }

          // ── Timeout: last-resort fire ─────────────────────────
          if (millis() - tGoal > (uint32_t)8500) {
            kick();
            done = true;
          }
        }
        sound(1200, 100);
        return;
      } else {
        headZero(0.f, 0.f);
        continue;  // [Fix 3] continue วน loop ต่อ — ไม่ return ออกจาก ronaldo()
      }
    }

    /* --- DIRECT VECTOR CHASE: speed proportional to filtered distance --- */
    float spd = constrain(filtDist * CHASE_SPD_SCALE,
                          (float)SPD_CHASE_MIN, (float)SPD_CHASE_MAX);
    holonomic(spd, ballAngle, headingOmega());
  }

  while (true) {
    if (returnAfter) return;  // mode 3: ออกจาก ronaldo() เพื่อต่อด้วย attack
    stopAll();
    delay(100);
  }
}
