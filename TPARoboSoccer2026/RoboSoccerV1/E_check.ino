/* ══════════════════════════════════════════════════════
   §7  LINE SENSORS — ZX-03 x5
   ══════════════════════════════════════════════════════ */

bool S1() {
  return analogRead(LINE_F) > LINE_REF;
}
bool S2() {
  return analogRead(LINE_L) > LINE_REF;
}
bool S3() {
  return analogRead(LINE_BL) > LINE_REF;
}
bool S4() {
  return analogRead(LINE_BR) > LINE_REF;
}
bool S5() {
  return analogRead(LINE_R) > LINE_REF;
}
bool LineD() {
  return S1() || S2() || S3() || S4() || S5();
}

/*
 * Vector-based line avoidance (non-blocking)
 * ทิศทางถอย = ตรงข้ามกับเซนเซอร์ที่เจอ พร้อม heading hold
 */
void avoidLine() {
  float mx = 0, my = 0;
  if (S1()) {
    mx += cosf(270 * DEG2RAD);
    my += sinf(270 * DEG2RAD);
  }
  if (S2()) {
    mx += cosf(0 * DEG2RAD);
    my += sinf(0 * DEG2RAD);
  }
  if (S3()) {
    mx += cosf(45 * DEG2RAD);
    my += sinf(45 * DEG2RAD);
  }
  if (S4()) {
    mx += cosf(135 * DEG2RAD);
    my += sinf(135 * DEG2RAD);
  }
  if (S5()) {
    mx += cosf(180 * DEG2RAD);
    my += sinf(180 * DEG2RAD);
  }

  if (mx == 0 && my == 0) {
    stopAll();
    return;
  }

  float dir = atan2f(my, mx) * RAD2DEG;
  if (dir < 0) dir += 360.0f;

  getIMU();
  float rot = constrain(-Yaw * 2.25f, -80, 80);
  holonomic(SPD_LINE, dir, rot);
}