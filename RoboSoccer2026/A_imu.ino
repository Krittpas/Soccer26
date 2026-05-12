/* ══════════════════════════════════════════════════════
   A_imu.ino — ZX-IMU via Serial1 @ 115200
   ══════════════════════════════════════════════════════ */

/* ── Low-level non-blocking read ──────────────────────── */
static bool _getIMU_raw() {
  while (Serial1.available()) {
    uint8_t b = Serial1.read();
    if (rxCnt == 0 && b != 0xAA) continue;
    rxBuf[rxCnt++] = b;
    if (rxCnt == 8) {
      rxCnt = 0;
      if (rxBuf[0] == 0xAA && rxBuf[7] == 0x55) {
        pvYaw   = (int16_t)(rxBuf[1] << 8 | rxBuf[2]) / 100.0f;
        pvPitch = (int16_t)(rxBuf[3] << 8 | rxBuf[4]) / 100.0f;
        pvRoll  = (int16_t)(rxBuf[5] << 8 | rxBuf[6]) / 100.0f;
        return true;
      }
    }
  }
  return false;
}

/* ── Flush buffer (call before any decision) ──────────── */
void getIMU() {
  for (uint8_t i = 0; i < 8; i++) _getIMU_raw();
}

/* ── Send calibration commands ────────────────────────── */
void zeroYaw() {
  Serial1.write(0xA5); delay(10);
  Serial1.write(0x54); delay(100);
  Serial1.write(0xA5); delay(10);
  Serial1.write(0x55); delay(100);
  Serial1.write(0xA5); delay(10);
  Serial1.write(0x52); delay(100);
}

/* ── Auto-zero: spin until IMU settles near 0° ─────────
   Shows progress on OLED. Returns when |pvYaw| < 0.08°  */
void autoZero() {
  zeroYaw();
  delay(200);
  uint32_t t = millis();
  oled.clear();
  oled.text(1, 2, "Zeroing IMU...");
  oled.show();
  while (true) {
    getIMU();
    oled.text(3, 0, "Yaw: %f   ", pvYaw);
    oled.show();
    if (fabsf(pvYaw) < 0.08f) break;
    if (millis() - t > 5000) {   // retry every 5 s
      zeroYaw();
      t = millis();
    }
    delay(20);
  }
  SetPoint = 0.f;
  x_pErr = 0.f;
  x_i    = 0.f;
  oled.clear();
  oled.show();
}

/* ── Heading-hold PD ──────────────────────────────────────
   Computes omega to keep robot facing SetPoint.
   Call this inside holonomic() omega argument.             */
float headingOmega() {
  float err = -YawErr;
  float d   = err - x_pErr;
  x_pErr    = err;
  x_i      += err;
  x_i       = constrain(x_i, -100.f, 100.f);
  float w   = err * HEAD_KP + d * HEAD_KD;
  return constrain(w, -100.f, 100.f);
}
