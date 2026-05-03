// อาร์เรย์ชื่อ mode (ประกาศ global)
const char* modeName[] = {
  "ATTACK  ", "DEFEND  ", "EXTRA   ",
  "LEFT-PEN", "RGHT-PEN", "PENALDO ", "TEST-KCK", "ONANA  "
};
void oledBox(int x, int y, int w, int h,
             const char* label, const char* value) {
  oled.drawRect(x, y, w, h, WHITE);
  oled.fillRect(x, y, w, 9, WHITE);  // inverted header
  oled.setTextColor(BLACK);
  oled.setCursor(x + 2, y + 1);
  oled.print(label);
  oled.setTextColor(WHITE);
  oled.setCursor(x + 2, y + 12);
  oled.print(value);
}
void oledProgressBar(int x, int y,
                     int w, int h,
                     int val, int maxVal) {
  oled.drawRect(x, y, w, h, WHITE);
  int fill = map(constrain(val, 0, maxVal), 0, maxVal, 0, w - 2);
  oled.fillRect(x + 1, y + 1, fill, h - 2, WHITE);
}
// ── Setup screen  (ใส่ใน while(1) loop) ──────────────
void drawSetupScreen() {
  oled.clear();

  // Row 0 — title bar (inverted)
  oled.fillRect(0, 0, 128, 10, WHITE);
  oled.setTextColor(BLACK);
  oled.setCursor(22, 1);
  oled.print("TPA SOCCER 2026");
  oled.setTextColor(WHITE);

  // Row 1 — Yaw + Goal side by side
  oled.drawLine(0, 11, 128, 11, WHITE);  // divider
  oled.setCursor(2, 14);
  oled.print("YAW: ");
  oled.setCursor(26, 14);
  oled.print(pvYaw, 2);

  oled.drawLine(64, 11, 64, 32, WHITE);  // vertical split
  oled.setCursor(68, 14);
  oled.print("GOAL: ");
  oled.setCursor(100, 14);
  oled.print((cg == 2) ? "YEL" : "BLU");

  // Row 2 — Mode label
  oled.drawLine(0, 32, 128, 32, WHITE);
  oled.setCursor(2, 35);
  oled.print("MODE:");
  oled.setCursor(38, 35);
  oled.print(modeName[mode - 1]);

  // Row 3 — progress bar (knob position)
  oled.drawLine(0, 45, 128, 45, WHITE);
  int barW = ((knob(6) + 1) * 116) / 7;
  oled.fillRect(6, 48, barW, 8, WHITE);
  oled.drawRect(6, 48, 116, 8, WHITE);

  // Row 4 — button hints
  oled.setCursor(2, 57);
  oled.print("A:Zero B:RUN OK:Goal");

  oled.show();
}
