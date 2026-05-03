/* ══════════════════════════════════════════════════════
   §5  IMU — ZX-IMU via Serial1
   ══════════════════════════════════════════════════════ */

void zeroYaw() {
  Serial1.write(0xA5);
  delay(10);
  Serial1.write(0x54);
  delay(100);  // baud lock
  Serial1.write(0xA5);
  delay(10);
  Serial1.write(0x55);
  delay(100);  // pitch/roll correction
  Serial1.write(0xA5);
  delay(10);
  Serial1.write(0x52);
  delay(100);  // zero yaw
}

// อ่านแพ็คเก็ต IMU — non-blocking (เรียก 1 ครั้งต่อ loop ก็พอ)
bool getIMUU() {
  while (Serial1.available()) {
    uint8_t b = Serial1.read();
    if (rxCnt == 0 && b != 0xAA) continue;
    rxBuf[rxCnt++] = b;
    if (rxCnt == 8) {
      rxCnt = 0;
      if (rxBuf[0] == 0xAA && rxBuf[7] == 0x55) {
        pvYaw = (int16_t)(rxBuf[1] << 8 | rxBuf[2]) / 100.0f;
        pvPitch = (int16_t)(rxBuf[3] << 8 | rxBuf[4]) / 100.0f;
        pvRoll = (int16_t)(rxBuf[5] << 8 | rxBuf[6]) / 100.0f;
        return true;
      }
    }
  }
  return false;
}

// flush buffer — ดึง IMU ให้ครบก่อนตัดสินใจ
void getIMU() {
  for (byte i = 0; i < 8; i++) getIMUU();
}