/* ══════════════════════════════════════════════════════
   K_onana.ino — ONANA: Goalkeeper / Penalty Defender (ข้อ 38.5, 38.6.2)
   ══════════════════════════════════════════════════════

   กติกา ข้อ 38:
   - วางหุ่นในเขตโทษ (ข้อ 38.5)
   - เคลื่อนที่ได้เฉพาะในเขตโทษ 70cm × 25cm (ข้อ 38.6.2)
   - เป้าหมาย: สกัดบอลไม่ให้เข้าประตูกว้าง 60cm

   Sequence:
   1. วนลูปตลอดกาล — ไม่มี timeout
   2. ถ้า S1 (front) trigger → ถอยหลังสั้นๆ เพื่อไม่ออกนอกเขตโทษ
   3. ถ้าเห็นบอล → slide ซ้าย/ขวาตาม ballPosX
   4. ถ้าไม่เห็นบอล → หยุดนิ่ง รักษาทิศ heading
   ══════════════════════════════════════════════════════ */

void onana() {
  sound(500, 250);

  while (true) {
    updateVision();
    getIMU();

    // ตรวจ front line sensor — ห้ามออกนอกเขตโทษ (ข้อ 38.6.2)
    if (S1()) {
      holonomic(40.f, 270.f, headingOmega());
      delay(150);
      stopAll();
      continue;
    }

    if (ballSeen) {
      float rotErr = 160.f - (float)ballPosX;

      if (fabsf(rotErr) < (float)ONANA_CENTER_DEAD) {
        // บอลอยู่กลางพอ — หยุดนิ่ง รักษาทิศ
        holonomic(0.f, 0.f, headingOmega());
      } else {
        // Slide เข้าหาบอล
        float slideSpd = constrain(fabsf(rotErr) * ONANA_SLIDE_KP, 0.f, ONANA_SLIDE_MAX);
        float slideDir = (rotErr > 0.f) ? 180.f : 0.f;
        holonomic(slideSpd, slideDir, headingOmega());
      }
    } else {
      // ไม่เห็นบอล — หยุดนิ่ง รักษาทิศ
      holonomic(0.f, 0.f, headingOmega());
    }
  }
}
