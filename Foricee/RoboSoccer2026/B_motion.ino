/* ══════════════════════════════════════════════════════
   B_motion.ino — 3-wheel holonomic drive
   ══════════════════════════════════════════════════════

   Wheel layout (top view, front = 90°):

          [W1]
         /    \
       [W3]  [W2]

   Motor: motor(1)=W1  motor(2)=W2  motor(3)=W3
   Angle: 0°=right  90°=forward  180°=left  270°=back
   omega > 0 → rotate clockwise
   ══════════════════════════════════════════════════════ */

void stopAll() {
  motor(1, 0);
  motor(2, 0);
  motor(3, 0);
}

void wheel(int s1, int s2, int s3) {
  motor(1, (int)constrain(s1, -100, 100));
  motor(2, (int)constrain(s2, -100, 100));
  motor(3, (int)constrain(s3, -100, 100));
}

void holonomic(float spd, float theta, float omega) {
  float rad = theta * DEG2RAD;
  float vx_ = spd * cosf(rad);
  float vy_ = spd * sinf(rad);
  wheel((int)roundf(vy_ * COS30 - vx_ * SIN30 + omega),
        (int)roundf(-vy_ * COS30 - vx_ * SIN30 + omega),
        (int)roundf(vx_ + omega));
}

// เดินทิศ dir พร้อม hold heading = SetPoint
void headZero(float spd, float dir) {
  getIMU();
  holonomic(spd, dir, headingOmega());
}
