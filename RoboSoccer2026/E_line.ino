/* ══════════════════════════════════════════════════════
   E_line.ino — 5× ZX-03 white-line sensors
   ══════════════════════════════════════════════════════

   Sensor positions (robot top view, front = up):
         S1 (front)
     S2 (left)  S5 (right)
     S3 (back-L) S4 (back-R)
   ══════════════════════════════════════════════════════ */

bool S1() { return analogRead(LINE_F)  > LINE_REF; }
bool S2() { return analogRead(LINE_L)  > LINE_REF; }
bool S3() { return analogRead(LINE_BL) > LINE_REF; }
bool S4() { return analogRead(LINE_BR) > LINE_REF; }
bool S5() { return analogRead(LINE_R)  > LINE_REF; }

bool LineD() { return S1() || S2() || S3() || S4() || S5(); }

/* ── Vector-sum avoidance ─────────────────────────────────
   Each triggered sensor pushes the robot in the OPPOSITE
   direction. Vectors are summed, then normalized to one
   escape angle. Uses headZero() to keep facing forward.    */
void avoidLine() {
  float mx = 0.f, my = 0.f;

  // Sensor fires → add escape vector (opposite direction)
  if (S1()) { mx += cosf(270.f * DEG2RAD); my += sinf(270.f * DEG2RAD); } // front  → back
  if (S2()) { mx += cosf(  0.f * DEG2RAD); my += sinf(  0.f * DEG2RAD); } // left   → right
  if (S3()) { mx += cosf( 45.f * DEG2RAD); my += sinf( 45.f * DEG2RAD); } // BL     → FR
  if (S4()) { mx += cosf(135.f * DEG2RAD); my += sinf(135.f * DEG2RAD); } // BR     → FL
  if (S5()) { mx += cosf(180.f * DEG2RAD); my += sinf(180.f * DEG2RAD); } // right  → left

  if (mx == 0.f && my == 0.f) { stopAll(); return; }

  float dir = atan2f(my, mx) * RAD2DEG;
  if (dir < 0.f) dir += 360.f;

  getIMU();
  holonomic(SPD_LINE, dir, headingOmega());
}
