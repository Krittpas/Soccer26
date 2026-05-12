/* ══════════════════════════════════════════════════════
   B_motion.ino — 3-wheel holonomic drive
   ══════════════════════════════════════════════════════

   Wheel layout (looking from above, front = 90°):

          [W1]
         /    \
       [W3]  [W2]

   Motor assignment:
     motor(1) = W1  (front-left area)
     motor(2) = W2  (front-right area)
     motor(3) = W3  (rear)

   Angle convention: 0°=right 90°=forward 180°=left 270°=back
   omega > 0  → rotate clockwise (right turn)
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

/* ── Core holonomic drive ─────────────────────────────── */
void holonomic(float spd, float theta, float omega) {
  float rad = theta * DEG2RAD;
  float vx_ = spd * cosf(rad);
  float vy_ = spd * sinf(rad);
  float s1  =  vy_ * COS30 - vx_ * SIN30 + omega;
  float s2  = -vy_ * COS30 - vx_ * SIN30 + omega;
  float s3  =  vx_ + omega;
  wheel((int)s1, (int)s2, (int)s3);
}

/* ── Heading-hold move: go direction dir, hold 0° ────── */
void headZero(float spd, float dir) {
  getIMU();
  holonomic(spd, dir, headingOmega());
}

/* ── Heading-hold move toward arbitrary SetPoint yaw ─── */
void headTarget(float spd, float dir, float targetYaw) {
  float savedSP = SetPoint;
  SetPoint = targetYaw;
  headZero(spd, dir);
  SetPoint = savedSP;
}
