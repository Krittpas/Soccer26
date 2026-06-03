/* ══════════════════════════════════════════════════════
   E_line.ino — 3× ZX-03 white-line sensors
   ══════════════════════════════════════════════════════

   Sensor positions (top view, front = up):

           [ S1 ]   ← Front
          /       \
        [W1]     [W2]
          \       /
       [S2]─────[S3]
      Back-L   Back-R

   Port mapping:
     S1 = Front   → analog pin LINE_F  (1)
     S2 = Back-L  → analog pin LINE_BL (2)
     S3 = Back-R  → analog pin LINE_BR (3)
   ══════════════════════════════════════════════════════ */

bool S1() {
  return analogRead(LINE_F) > LINE_REF;
}  // หน้า     //>
bool S2() {
  return analogRead(LINE_BL) > LINE_REF;
}  // หลังซ้าย  //>
bool S3() {
  return analogRead(LINE_BR) > LINE_REF;
}  // หลังขวา  //>

bool LineD() {
  return S1() || S2() || S3();
  // return 0;
}

/* ── Raw ADC values (ใช้ใน SensorCheck OLED) ──────────── */
int rawS1() {
  return analogRead(LINE_F);
}
int rawS2() {
  return analogRead(LINE_BL);
}
int rawS3() {
  return analogRead(LINE_BR);
}

/* ── Vector-sum avoidance ─────────────────────────────────
   Sensor fires → push robot in the OPPOSITE direction.
   Vectors summed → single escape angle → headZero.

   S1 front    fires → ถอยหลัง         (270°)
   S2 back-L   fires → เดินหน้า-ขวา   ( 45°)
   S3 back-R   fires → เดินหน้า-ซ้าย  (135°)

   S1+S2 corner → เดินขวา-หลัง        (315°)
   S1+S3 corner → เดินซ้าย-หลัง       (225°)
   S2+S3 both   → เดินหน้าตรง         ( 90°)
   ══════════════════════════════════════════════════════ */
void avoidLine() {
  float mx = 0.f, my = 0.f;

  if (S2() && S3()) {
    mx += cosf(90.f * DEG2RAD);
    my += sinf(90.f * DEG2RAD);
  }
  if (S1()) {
    mx += cosf(270.f * DEG2RAD);
    my += sinf(270.f * DEG2RAD);
  }
  if (S2()) {
    mx += cosf(45.f * DEG2RAD);
    my += sinf(45.f * DEG2RAD);
  }
  if (S3()) {
    mx += cosf(135.f * DEG2RAD);
    my += sinf(135.f * DEG2RAD);
  }

  if (fabsf(mx) < 0.01f && fabsf(my) < 0.01f) {
    stopAll();
    return;
  }

  float dir = atan2f(my, mx) * RAD2DEG;
  if (dir < 0.f) dir += 360.f;

  getIMU();
  holonomic(SPD_LINE, dir, headingOmega());
}
