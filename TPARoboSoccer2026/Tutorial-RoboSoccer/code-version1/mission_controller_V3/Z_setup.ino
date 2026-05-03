void setup() {
  Serial1.begin(115200);
  delay(100);

  reload();

  while (!huskylens.begin(Wire)) {
    oled.text(0, 0, "HuskyLens FAIL!");
    oled.show();
    delay(200);
  }

  while (!huskylens.begin(Wire)) {
    oled.text(0, 0, "HuskyLens FAIL!");
    oled.show();
    delay(100);
  }

  // zeroYaw();
  // delay(3000);
  // pvYaw = 90.0f;
  oled.text(2, 0, "SW_A => Test_Shoot");
  oled.text(3, 0, "SW_B => RUN");
  oled.show();
  while (!SW_B()) {
    if (SW_A()) {
      shoot();   //ยิง
      reload();  //เก็บก้านยิง
    }
    getIMU();
    headZero(0, 0);
  }
  oled.clearDisplay();
  oled.text(0, 0, "RUN");
  oled.show();
}