/* ══════════════════════════════════════════════════════
   F_chase.ino — Ball-capture AI (Attack mode)
   ══════════════════════════════════════════════════════

   Algorithm: Tangent-line surround (Yuta Method 4/5)
   ─────────────────────────────────────────────────────
   Three zones based on ball position in camera frame:

   Zone A — ball in front dead-zone (|diffX| < ROT_DEAD)
     → PD control X, forward speed based on distance
     → headZero maintains heading

   Zone B — ball closer than SURROUND_R
     → deflect outward: escape angle = ball_dir + theta_out
     → prevents robot from ramming ball sideways

   Zone C — ball far
     → tangent-line: theta = asin(r/dist), add to ball_dir
     → smoothly rounds toward ball's "back"

   Camera origin: (0,0) = top-left
   diffX = ballX - 160  (+ve = ball right of center)
   diffY = FWD_SP - ballY (+ve = ball needs to come closer)
   ══════════════════════════════════════════════════════ */

static float lastBallDir = 90.f;   // remembered for search spin

void doChase() {
  getIMU();

  float diffX = (float)(ballPosX - 160);
  float diffY = (float)(FWD_SP - ballPosY);
  float dist  = sqrtf(diffX * diffX + diffY * diffY);

  // ── angle to ball in robot-frame (0°=right, 90°=forward) ─
  float ballDir = atan2f(diffY, diffX) * RAD2DEG;
  if (ballDir < 0.f) ballDir += 360.f;
  lastBallDir = ballDir;

  float moveDir, spd;

  /* ── Zone A: ball nearly centered → approach straight ─── */
  if (fabsf(diffX) < (float)ROT_DEAD && fabsf(diffY) < (float)FWD_SP * 1.2f) {
    // PD on X (side-slide), forward speed from diffY
    float rotErr = -diffX;
    float rotD   = rotErr - rot_pErr;
    rot_pErr     = rotErr;
    float slideW = rotErr * ROT_KP + rotD * ROT_KD;
    slideW       = constrain(slideW, -40.f, 40.f);

    float fwdErr = diffY;
    float fwdD   = fwdErr - fwd_pErr;
    fwd_pErr     = fwdErr;
    float fwdSpd = fwdErr * FWD_KP + fwdD * FWD_KD;
    fwdSpd       = constrain(fwdSpd, (float)SPD_CHASE_MIN, (float)SPD_CHASE_MAX);

    // When ball is very close (dist ≤ 25) — trigger goal sequence
    if (dist <= 25.f && fabsf(YawErr) <= ALIGN_DEG) {
      stopAll();
      delay(30);
      goal(cg);
      sound(1200, 300);
      return;
    }

    holonomic(fwdSpd, 90.f, headingOmega() + slideW * 0.5f);
    return;
  }

  /* ── Zone B: ball too close (inside orbit radius) ───────── */
  if (dist < SURROUND_R) {
    float deflect = 90.f + (SURROUND_R - dist) * 90.f / SURROUND_R;
    moveDir = ballDir + (diffX >= 0.f ? deflect : -deflect);
    spd     = SPD_CHASE_MIN + (dist / SURROUND_R) * (SPD_CHASE_MAX - SPD_CHASE_MIN);
    holonomic(constrain(spd, (float)SPD_CHASE_MIN, (float)SPD_CHASE_MAX),
              fmodf(moveDir + 360.f, 360.f),
              headingOmega());
    return;
  }

  /* ── Zone C: far — tangent-line approach ─────────────────── */
  float theta = asinf(constrain(SURROUND_R / dist, -1.f, 1.f)) * RAD2DEG;
  moveDir = ballDir + (diffX >= 0.f ? theta : -theta);
  // Speed proportional to distance, clamped
  spd = constrain(dist * 0.55f, (float)SPD_CHASE_MIN, (float)SPD_CHASE_MAX);
  holonomic(spd,
            fmodf(moveDir + 360.f, 360.f),
            headingOmega());
}
