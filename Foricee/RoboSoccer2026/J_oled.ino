/* ══════════════════════════════════════════════════════
   J_oled.ino — Diagnostic OLED screens
   ══════════════════════════════════════════════════════

   drawSensorCheck()  — แสดงกราฟิกสถานะเซนเซอร์ทั้ง 3
   drawBallRadar()    — แสดงตำแหน่งบอลบน radar view
   drawGameDebug()    — แสดงค่า debug ระหว่างเล่นจริง
   ══════════════════════════════════════════════════════ */

/* ══════════════════════════════════════════════════════
   SENSOR CHECK SCREEN

   Layout (128×64):
   ┌─────────────────────────────┐
   │▌ SENSOR CHECK   LINE:1600 ▐│  ← header inverted
   ├─────────────────────────────┤
   │                             │
   │   ┌──────┐  ADC   STATUS   │
   │S1 │██░░░░│  1842   LINE!  │  ← front
   │S2 │░░░░░░│   820   OK     │  ← back-L
   │S3 │░░░░░░│   835   OK     │  ← back-R
   │                             │
   │  ┌──┐   ▲   ┌──┐           │  ← robot diagram
   │  │S2│  [R]  │S3│           │
   │        [S1]                 │
   └─────────────────────────────┘
   ══════════════════════════════════════════════════════ */
void drawSensorCheck() {
  int v1 = rawS1();
  int v2 = rawS2();
  int v3 = rawS3();
  bool t1 = (v1 < LINE_REF);
  bool t2 = (v2 < LINE_REF);
  bool t3 = (v3 < LINE_REF);

  oled.clear();

  // ── Header bar (inverted) ─────────────────────────────
  oled.fillRect(0, 0, 128, 10, WHITE);
  oled.setTextColor(BLACK);
  oled.setCursor(2, 1);
  oled.print("SENSOR CHECK");
  oled.setCursor(78, 1);
  oled.print("REF:");
  oled.print(LINE_REF);
  oled.setTextColor(WHITE);

  // ── Bar graph for each sensor ─────────────────────────
  // Bar area: x=0..80, bar height=6, max ADC=4095
  auto drawBar = [](int y, const char* label, int val, bool tripped) {
    // Label
    oled.setCursor(0, y);
    oled.print(label);

    // Bar background (empty)
    oled.drawRect(14, y, 72, 7, WHITE);

    // Bar fill (proportional to val/4095, max width=70)
    int barW = (int)((long)val * 70 / 4095);
    barW = constrain(barW, 0, 70);
    if (tripped) {
      oled.fillRect(15, y + 1, barW, 5, WHITE);  // solid = triggered
    } else {
      // Dotted fill when OK
      for (int x = 15; x < 15 + barW; x += 2)
        oled.drawFastVLine(x, y + 1, 5, WHITE);
    }

    // Threshold marker
    int threshX = 15 + (int)((long)LINE_REF * 70 / 4095);
    oled.drawFastVLine(threshX, y, 7, WHITE);

    // ADC value text
    oled.setCursor(89, y);
    oled.print(val);

    // Status badge
    // oled.setCursor(111, y);
    // if (tripped) oled.print("ON ");
    // else         oled.print("OK ");
  };

  drawBar(13, "S1", v1, t1);  // Front
  drawBar(22, "S2", v2, t2);  // Back-L
  drawBar(31, "S3", v3, t3);  // Back-R

  // ── Robot top-view diagram ────────────────────────────
  // Robot body = circle at (50,52) r=9
  oled.drawCircle(50, 52, 6, WHITE);
  // Front indicator (top of circle)
  oled.drawFastVLine(50, 41, 3, WHITE);
  oled.setCursor(48, 38);
  oled.print("^");

  // S1 dot at top
  if (t1) oled.fillCircle(50, 42, 3, WHITE);
  else oled.drawCircle(50, 42, 3, WHITE);
  oled.setCursor(54, 40);
  oled.print("1");

  // S2 dot at bottom-left
  if (t2) oled.fillCircle(42, 58, 3, WHITE);
  else oled.drawCircle(42, 58, 3, WHITE);
  oled.setCursor(30, 57);
  oled.print("2");

  // S3 dot at bottom-right
  if (t3) oled.fillCircle(58, 58, 3, WHITE);
  else oled.drawCircle(58, 58, 3, WHITE);
  oled.setCursor(63, 57);
  oled.print("3");

  // // ── IMU Yaw indicator ─────────────────────────────────
  // oled.setCursor(86, 44);
  // oled.print("YAW");
  // oled.setCursor(86, 52);
  // oled.print(pvYaw, 1);
  // oled.print((char)247);  // degree symbol workaround

  // ── Overall status line ───────────────────────────────
  // oled.drawLine(0, 63, 128, 63, WHITE);
  if (!t1 && !t2 && !t3) {
    oled.setCursor(90, 50);
    oled.print("CLEAR");
  } else {
    oled.setCursor(90, 50);
    oled.print("DETEC");
  }

  oled.show();
}

/* ══════════════════════════════════════════════════════
   BALL RADAR SCREEN

   Layout (128×64):
   ┌─────────────────────────────┐
   │▌ BALL RADAR    G:YEL      ▐│
   ├─────────────────────────────┤
   │  ┌────────────────────┐    │
   │  │      camera        │    │  ← 80×50 field view
   │  │   [BALL]           │    │     proportional to
   │  │        ·           │    │     320×240 camera
   │  │       [R]          │    │     R = robot crosshair
   │  └────────────────────┘    │
   │ bX:152 bY:188 SEEN:YES     │
   └─────────────────────────────┘
   ══════════════════════════════════════════════════════ */
void drawBallRadar() {
  oled.clear();

  // ── Header ────────────────────────────────────────────
  oled.fillRect(0, 0, 128, 10, WHITE);
  oled.setTextColor(BLACK);
  oled.setCursor(2, 1);
  oled.print("BALL RADAR");
  oled.setCursor(72, 1);
  oled.print("G:");
  oled.print((cg == SIG_GOAL_YEL) ? "YEL" : "BLU");
  oled.setTextColor(WHITE);

  // ── Camera view box ───────────────────────────────────
  // Box: top-left=(4,12), size=80×48
  const int BX = 4, BY = 12, BW = 80, BH = 48;
  oled.drawRect(BX, BY, BW, BH, WHITE);

  // Field center crosshair (160,120 in camera = box center)
  int cx = BX + BW / 2;
  int cy = BY + BH / 2;
  oled.drawFastHLine(cx - 3, cy, 7, WHITE);
  oled.drawFastVLine(cx, cy - 3, 7, WHITE);

  // Grid lines (thirds)
  oled.drawFastVLine(BX + BW / 3, BY + 1, BH - 2, WHITE);
  oled.drawFastVLine(BX + BW * 2 / 3, BY + 1, BH - 2, WHITE);

  // Robot marker (fixed center bottom of box = robot position)
  int rx = cx;
  int ry = BY + BH - 5;
  oled.drawRect(rx - 3, ry - 3, 7, 7, WHITE);
  oled.drawPixel(rx, ry, WHITE);

  // Ball marker (mapped from camera 320×240 → box BW×BH)
  if (ballSeen) {
    int bx = BX + (int)((long)ballPosX * BW / 320);
    int by_p = BY + (int)((long)ballPosY * BH / 240);
    bx = constrain(bx, BX + 2, BX + BW - 3);
    by_p = constrain(by_p, BY + 2, BY + BH - 3);
    // Ball = filled circle
    oled.fillCircle(bx, by_p, 3, WHITE);
    // Distance line from robot to ball
    oled.drawLine(rx, ry, bx, by_p, WHITE);
  }

  // Goal marker (if seen)
  if (goalSeen) {
    int gx = BX + (int)((long)goalPosX * BW / 320);
    gx = constrain(gx, BX + 2, BX + BW - 3);
    // Goal = horizontal bar at top of box
    oled.fillRect(gx - 4, BY + 1, 9, 3, WHITE);
  }

  // ── Data sidebar ──────────────────────────────────────
  oled.setCursor(88, 12);
  if (ballSeen) oled.print("SEEN");
  else oled.print("LOST");

  oled.setCursor(88, 20);
  oled.print("X:");
  oled.print(ballPosX);
  oled.setCursor(88, 28);
  oled.print("Y:");
  oled.print(ballPosY);

  // Distance from center
  if (ballSeen) {
    int dx = ballPosX - 160;
    int dy = ballPosY - 120;
    int d = (int)sqrtf((float)(dx * dx + dy * dy));
    oled.setCursor(88, 36);
    oled.print("D:");
    oled.print(d);
  }

  // ── Status bar ────────────────────────────────────────
  // oled.drawLine(0, 55, 128, 55, WHITE);
  oled.setCursor(88, 48);
  oled.print("Y:");
  oled.print(pvYaw, 1);
  oled.setCursor(88, 56);
  oled.print("L:");
  oled.print((int)S1());
  oled.print((int)S2());
  oled.print((int)S3());

  oled.show();
}

/* ══════════════════════════════════════════════════════
   GOAL RADAR SCREEN — mode 11
   แสดงตำแหน่งประตู X, Y แบบ real-time สำหรับ calibrate

   Layout (128×64):
   ┌─────────────────────────────┐
   │▌ GOAL RADAR    G:YEL      ▐│
   ├─────────────────────────────┤
   │  ┌────────────────────┐    │
   │  │ │  ████(goal)      │SEEN│
   │  │ │    ●(ball)       │X:  │
   │  │ │       [R]        │Y:  │
   │  └────────────────────┘ b: │
   │                        Yw: │
   │                        L:  │
   └─────────────────────────────┘
   ══════════════════════════════════════════════════════ */
void drawGoalRadar() {
  oled.clear();

  // ── Header ────────────────────────────────────────────
  oled.fillRect(0, 0, 128, 10, WHITE);
  oled.setTextColor(BLACK);
  oled.setCursor(2, 1);
  oled.print("GOAL RADAR");
  oled.setCursor(72, 1);
  oled.print("G:");
  oled.print((cg == SIG_GOAL_YEL) ? "YEL" : "BLU");
  oled.setTextColor(WHITE);

  // ── Camera box ────────────────────────────────────────
  const int BX = 4, BY = 12, BW = 80, BH = 48;
  oled.drawRect(BX, BY, BW, BH, WHITE);

  // Center vertical line (X=160 in camera)
  int cx = BX + BW / 2;
  oled.drawFastVLine(cx, BY + 1, BH - 2, WHITE);

  // Robot marker (bottom-center)
  int rx = cx;
  int ry = BY + BH - 5;
  oled.drawRect(rx - 3, ry - 3, 7, 7, WHITE);
  oled.drawPixel(rx, ry, WHITE);

  // Goal marker — แสดงตำแหน่ง X,Y จริงใน camera space
  if (goalSeen) {
    int gx = BX + (int)((long)goalPosX * BW / 320);
    int gy = BY + (int)((long)goalPosY * BH / 240);
    gx = constrain(gx, BX + 2, BX + BW - 3);
    gy = constrain(gy, BY + 2, BY + BH - 3);
    oled.fillRect(gx - 5, gy - 2, 11, 5, WHITE);  // goal = แท่งนอน
    oled.drawLine(rx, ry, gx, gy, WHITE);           // เส้นระยะห่าง robot→goal
  }

  // Ball marker
  if (ballSeen) {
    int bx = BX + (int)((long)ballPosX * BW / 320);
    int by_p = BY + (int)((long)ballPosY * BH / 240);
    bx   = constrain(bx,   BX + 2, BX + BW - 3);
    by_p = constrain(by_p, BY + 2, BY + BH - 3);
    oled.fillCircle(bx, by_p, 2, WHITE);
  }

  // ── Sidebar — Goal data ───────────────────────────────
  oled.setCursor(88, 12);
  oled.print(goalSeen ? "SEEN" : "LOST");

  oled.setCursor(88, 20);
  oled.print("X:");
  oled.print(goalPosX);

  oled.setCursor(88, 28);
  oled.print("Y:");
  oled.print(goalPosY);

  oled.setCursor(88, 36);
  oled.print("b:");
  oled.print(ballPosY);

  oled.setCursor(88, 48);
  oled.print("Yw:");
  oled.print(pvYaw, 1);

  oled.setCursor(88, 56);
  oled.print("L:");
  oled.print((int)S1());
  oled.print((int)S2());
  oled.print((int)S3());

  oled.show();
}

/* ══════════════════════════════════════════════════════
   GAME DEBUG SCREEN (ใช้ระหว่างเล่น)
   อัปเดตทุก 100 ms เพื่อไม่ blocking
   ══════════════════════════════════════════════════════ */
static uint32_t _lastOledMs = 0;
void drawGameDebug() {
  if (millis() - _lastOledMs < 100) return;
  _lastOledMs = millis();

  oled.clear();

  // Row 0 — Yaw + mode
  oled.setCursor(0, 0);
  oled.print("Y:");
  oled.print(pvYaw, 1);
  oled.print(" M:");
  oled.print(mode);
  oled.print(" G:");
  oled.print((cg == SIG_GOAL_YEL) ? "Y" : "B");
  oled.print(kickReady ? "K" : ".");

  // Row 1 — Ball
  oled.setCursor(0, 9);
  if (ballSeen) {
    oled.print("B:");
    oled.print(ballPosX);
    oled.print(",");
    oled.print(ballPosY);
  } else {
    oled.print("B: ---,---");
  }

  // Row 2 — Goal
  oled.setCursor(0, 18);
  if (goalSeen) {
    oled.print("G:");
    oled.print(goalPosX);
  } else {
    oled.print("G: ---");
  }

  // Row 3 — Line sensors as mini bar
  oled.setCursor(0, 27);
  oled.print("LINE ");
  oled.print(S1() ? "[S1]" : "    ");
  oled.print(S2() ? "[S2]" : "    ");
  oled.print(S3() ? "[S3]" : "    ");

  // Row 4 — Mini ball position bar (horizontal)
  oled.drawRect(0, 38, 128, 8, WHITE);
  if (ballSeen) {
    int bx = (int)((long)ballPosX * 126 / 320);
    bx = constrain(bx, 0, 126);
    oled.fillRect(bx, 39, 3, 6, WHITE);
    // Center marker
    oled.drawFastVLine(64, 38, 8, WHITE);
  }

  // Row 5 — Yaw bar (±60° range)
  oled.drawRect(0, 48, 128, 8, WHITE);
  {
    int yp = 64 + (int)(YawErr * 64.f / 60.f);
    yp = constrain(yp, 1, 127);
    oled.fillRect(yp - 1, 49, 3, 6, WHITE);
    oled.drawFastVLine(64, 48, 8, WHITE);  // zero marker
  }

  oled.show();
}
