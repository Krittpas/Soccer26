/* ══════════════════════════════════════════════════════
   §6  MOTORS — Holonomic omni 3-wheel
   ══════════════════════════════════════════════════════ */

void wheel(int s1, int s2, int s3) {
  motor(1, s1);
  motor(2, s2);
  motor(3, s3);
}

void stopAll() {
  wheel(0, 0, 0);
}

/*
 * Holonomic drive
 * spd   : 0–100  ความเร็ว
 * theta : 0–360  ทิศทาง (0=ขวา 90=หน้า 180=ซ้าย 270=หลัง)
 * omega : ±100   หมุนตัว (+=หมุนขวา)
 */
void holonomic(float spd, float theta, float omega) {
  thetaRad = theta * DEG2RAD;
  vx = spd * cosf(thetaRad);
  vy = spd * sinf(thetaRad);
  spd1 = vy * COS30 - vx * SIN30 + omega;
  spd2 = -vy * COS30 - vx * SIN30 + omega;
  spd3 = vx + omega;
  wheel((int)constrain(spd1, -100, 100),
        (int)constrain(spd2, -100, 100),
        (int)constrain(spd3, -100, 100));
}

/*
 * Heading-hold drive (PD)
 * ใช้ Yaw macro ที่ wrap ±180 แล้ว
 */
void headZero(float spd, float dir) {
  getIMU();
  float err = -Yaw;  // error จาก SetPoint
  float d = err - x_pError;
  x_i += err;
  x_i = constrain(x_i, -100, 100);
  float w = err * HEAD_KP + d * HEAD_KD;
  w = constrain(w, -100, 100);
  x_pError = err;
  holonomic(spd, dir, w);
}