void setup() {
  Serial1.begin(115200);
  delay(100);

  reload();
  while (!huskylens.begin(Wire)) {
    oled.text(0, 0, "HuskyLens failed!");
    oled.show();
  }

  mode = 0;
  cg = 2;
  while (digitalRead(A6) /* && !SW_B()*/) {
    getIMU();
    oled.text(0, 0, "yaw  :    %f   ", pvYaw);
    oled.text(2, 0, (cg == 2) ? "goal :   YELLOW" : "goal :   BLUE  ");
    oled.text(4, 0, "mode : ");

    if (SW_OK()) {
      cg = (cg == 2) ? 3 : 2;
      oled.text(2, 8, (cg == 2) ? "YELLOW" : "BLUE  ");
      delay(300);
    }
    if (SW_A()) {
      zeroYaw();
      sound(1500, 50);
    }
    if (SW_B()) {
      SetPoint = pvYaw;  //เปลี่ยนจุดนี้ให้เป็นจุดเริ่มต้นแกนหมุน
    }
    if (knob(6) == 0) {
      mode = 1;
      oled.text(4, 9, "ATTACK  ");
    } else if (knob(6) == 1) {
      mode = 2;
      oled.text(4, 9, "DEFEND  ");
    } else if (knob(6) == 2) {
      mode = 3;
      oled.text(4, 9, "EXTRA   ");
    }  //
    else if (knob(6) == 3) {
      mode = 4;
      oled.text(4, 9, "LEFT    ");
    } else if (knob(6) == 4) {
      mode = 5;
      oled.text(4, 9, "RIGHT   ");
    }  //
    else if (knob(6) == 5) {
      mode = 6;
      oled.text(4, 9, "PENALTY ");
    } else if (knob(6) == 6) {
      mode = 7;
      oled.text(4, 9, "TEST K ");
    }
    oled.show();
  }
  oled.clear();
  //
  oled.text(2, 7, "[BenChan]");
  if (mode == 1) {
    oled.text(4, 8, "ATTACK  ");
    oled.show();
  } else if (mode == 2) {
    unsigned long timeDF = millis();
    while (millis() - timeDF < 4500) {
      oled.text(4, 8, "DEFEND  ");
      oled.show();
      if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
        int ballPosY = constrain(huskylens.blockInfo[1][0].y, 0, 240);
        if (ballPosY > 115) break;
      }
    }
  } else if (mode == 3) {
    unsigned long timeDF = millis();
    while (millis() - timeDF < 1000) {
      oled.text(4, 8, "EXTRA   ");
      oled.show();
    }
  }  //
  /* else if (mode == 6) {
    oled.text(4, 8, "PENALTY ");
    oled.show();
    Onana();
  }*/
  else if (mode == 7) {
    oled.text(4, 8, "TEST K ");
    oled.show();
    while (1) {
      if (SW_A()) shoot();
      if (SW_B()) reload();
      if (SW_OK()) kick();
    }
  }
  sound(1500, 50);

  /*while (1) {
    oled.text(1, 1, "%d    %d    ", analogRead(1), S1());
    oled.text(2, 1, "%d    %d    ", analogRead(2), S2());
    oled.text(3, 1, "%d    %d    ", analogRead(3), S3());
    oled.text(4, 1, "%d    %d    ", analogRead(4), S4());
    oled.text(5, 1, "%d    %d    ", analogRead(5), S5());
    oled.show();
  }*/
}