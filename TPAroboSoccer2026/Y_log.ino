void start() {
  Serial1.begin(115200);

  unsigned long t0 = millis();
  while (millis() - t0 < 3000)
    ;

  zero();

  t0 = millis();
  while (millis() - t0 < 1000)
    ;

  updateIMU();
  SetPoint = pvYaw;
  sound(1500, 100);
  reload();

  t0 = millis();
  while (!huskylens.begin(Wire) && millis() - t0 < 2500) {
    oled.text(0, 0, "HuskyLens failed!");
    oled.show();
  }

  while (!SW_OK()) {
    getIMU();

    if (SW_A()) {
      zero();
      sound(1500, 50);
    }
    if (SW_B()) {
      colorGoal = (colorGoal == 2) ? 3 : 2;
      oled.text(2, 8, (colorGoal == 2) ? "YELLOW" : "BLUE  ");
      delay(250);
    }

    if (knob(4) == 0) {
      mode = 1;
      oled.text(4, 9, "ATTACK    ");
    } else if (knob(4) == 1) {
      mode = 2;
      oled.text(4, 9, "DEFEND    ");
    } else if (knob(4) == 2) {
      mode = 3;
      oled.text(4, 9, "EXTRA     ");
    } else if (knob(4) == 3) {
      mode = 4;
      oled.text(4, 9, "PENALTY   ");
    } else if (knob(4) == 4) {
      mode = 5;
      oled.text(4, 9, "TEST KICK ");
    }

    oled.text(0, 0, "yaw  :   %f   ", pvYaw);
    oled.text(2, 0, (colorGoal == 2) ? "goal :   YELLOW" : "goal :   BLUE  ");
    oled.text(4, 0, "mode :");
    oled.show();
  }
  oled.clear();

  /* ======================================================== */

  oled.text(2, 7, "[BenChan]");
  if (mode == 1) {
    oled.text(4, 8, "ATTACK      ");
    oled.show();
  } else if (mode == 2) {
    unsigned long timeDF = millis();
    while (millis() - timeDF < 4750) {
      oled.text(4, 8, "DEFEND    ");
      oled.show();
      if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
        int ballPosY = constrain(huskylens.blockInfo[1][0].y, 0, 240);
        if (ballPosY > 115) break;
      }
    }
  } else if (mode == 3) {
    unsigned long timeDF = millis();
    while (millis() - timeDF < 1000) {
      oled.text(4, 8, "EXTRA     ");
      oled.show();
    }
  } else if (mode == 5) {
    oled.text(4, 8, "TEST KICK   ");
    oled.show();
    while (1) {
      if (SW_OK()) kick();
      if (SW_A()) shoot();
      if (SW_B()) reload();
    }
  }
  sound(1500, 50);
}

void readSensor() {
  while (1) {
    oled.text(1, 1, "%d    %d    ", analogRead(1), digital(1));
    oled.text(2, 1, "%d    %d    ", analogRead(2), digital(2));
    oled.text(3, 1, "%d    %d    ", analogRead(3), digital(3));
    oled.text(4, 1, "%d    %d    ", analogRead(4), digital(4));
    oled.text(5, 1, "%d    %d    ", analogRead(5), digital(5));
    oled.show();
  }
}