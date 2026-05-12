/* ─── OLED setup screen ───────────────────────────────── */
static const char* modeNames[] = {
  "ATTACK  ", "DEFEND  ", "EXTRA   ",
  "LEFT-PEN", "RGHT-PEN", "PENALTY ", "TEST-KCK"
};

void drawSetupScreen() {
  oled.clear();

  // Title bar (inverted)
  oled.fillRect(0, 0, 128, 10, WHITE);
  oled.setTextColor(BLACK);
  oled.setCursor(5, 1);
  oled.print("TPA ROBO SOCCER 2026");
  oled.setTextColor(WHITE);

  // Yaw + goal
  oled.drawLine(0, 11, 128, 11, WHITE);
  oled.setCursor(2, 14);
  oled.print("YAW:");
  oled.setCursor(28, 14);
  oled.print(pvYaw, 1);
  oled.drawLine(64, 11, 64, 32, WHITE);
  oled.setCursor(68, 14);
  oled.print("GOAL:");
  oled.setCursor(102, 14);
  oled.print((cg == SIG_GOAL_YEL) ? "YEL" : "BLU");

  // Mode
  oled.drawLine(0, 32, 128, 32, WHITE);
  oled.setCursor(2, 35);
  oled.print("MODE:");
  oled.setCursor(38, 35);
  oled.print(modeNames[mode - 1]);

  // Knob bar
  oled.drawLine(0, 45, 128, 45, WHITE);
  int barW = ((knob(7) + 1) * 116) / 8;
  oled.fillRect(6, 48, barW, 8, WHITE);
  oled.drawRect(6, 48, 116, 8, WHITE);

  // Hints
  oled.setCursor(2, 57);
  oled.print("A:Zero  B:Run  OK:Goal");

  oled.show();
}