void loop() {
  static unsigned long ballLostTimer = 0;
  static bool wasSeeing = false;

  static unsigned long stateTimer = 0;

  /*if (LineD()) {
    avoidLine();
    return;
  }  //
  else {*/
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    ballLostTimer = millis();
    wasSeeing = true;

    int ballPosX = constrain(huskylens.blockInfo[1][0].x, 0, 320);
    int ballPosY = constrain(huskylens.blockInfo[1][0].y, 0, 240);

    // อัพเดท IMU
    for (int i = 0; i < 5; i++) {  // ลดจาก 8 เป็น 5
      getIMU();
    }

    if (kickState == 1) {
      getIMU();

      // Calculate DIFFERENCE between BALL and CENTER
      diffX = ballPosX - 160;
      diffY = 220 - ballPosY;

      // SPEED depends on distance
      float distance = sqrt(diffX * diffX + diffY * diffY);
      float moveSpeed = constrain(distance, 35, 45);  //45 95
      // DIRECTION depends on angle to ball
      float angleToball = atan2(diffY, diffX) * 180 / PI;

      headZero(moveSpeed, fmod(angleToball + 360, 360));

      if ((distance <= 100 || ((mode == 4 || mode == 5) && distance <= 15)) && abs(pvYaw) <= alignErrorGap && abs(diffX) <= 5) {  //75
        wheel(0, 0, 0);
        //
        if (mode == 4 || mode == 5) {
          x_error = -pvYaw;
          x_i = x_i + x_error;
          x_i = constrain(x_i, -100, 100);
          x_d = x_error - x_pError;
          x_pError = x_error;
          x_w = (x_error * 1.5) + (x_d * 0.1);
          x_w = constrain(x_w, -100, 100);
          holonomic(60, (mode == 4) ? 180 : 0, x_w);
          delay(650);
          wheel(0, 0, 0);
          mode = 1;
        }
        //
        goal(cg);
        sound(1250, 500);
      }
    }  //
    //else if (kickState == 2) {
    //kickState = 1;

    /*if (stateTimer == 0) {
        stateTimer = millis();
      }

      // ถ้าอยู่ใน kickState == 2 นานเกิน 3 วินาที ให้เปลี่ยนกลับเป็น kickState == 1
      if (millis() - stateTimer > 1800) {
        beep();
        kickState = 1;
        stateTimer = 0;
        wheel(0, 0, 0);
        return;
      }

      rot_error = sp_rot - ballPosX;
      rot_i = rot_i + rot_error;
      rot_i = constrain(rot_i, -100, 100);
      rot_d = rot_error - rot_pError;
      rot_pError = rot_error;

      float rot_Kp_base = 0.5;
      if (abs(rot_error) > 50) {
        rot_Kp = rot_Kp_base;
      } else if (abs(rot_error) > 40) {
        rot_Kp = rot_Kp_base - 0.2;
      } else if (abs(rot_error) <= 30) {
        rot_Kp = rot_Kp_base - 0.22;
      } else if (abs(rot_error) > 70) {
        rot_Kp = rot_Kp_base + 5;
      }
      rot_w = (rot_error * rot_Kp) + (rot_i * rot_Ki) + (rot_d * rot_Kd);
      rot_w = constrain(rot_w, -100, 100);

      fli_error = spFli - ballPosY;
      fli_i = fli_i + fli_error;
      fli_i = constrain(fli_i, -100, 100);
      fli_d = fli_error - fli_pError;
      fli_pError = fli_error;
      fli_spd = fli_error * fli_Kp + fli_i * fli_Ki + fli_d * fli_Kd;
      fli_spd = constrain(fli_spd, -100, 100);

      holonomic(fli_spd, 90, rot_w);

      if ((abs(rot_error) <= rotErrorGap) && (abs(fli_error) <= flingErrorGap)) {
        wheel(0, 0, 0);

        static unsigned long curveStartTime = 0;
        if (curveStartTime == 0) {
          curveStartTime = millis();
        }
        if (pvYaw < 0) {
          vecCurve = 0;
          radCurve = 15;
        } else {
          vecCurve = 180;
          radCurve = -15;
        }

        holonomic(55, vecCurve, radCurve);
        if (abs(pvYaw) <= 15 || (millis() - curveStartTime > 1250)) {
          wheel(0, 0, 0);
          curveStartTime = 0;  // reset timer
          stateTimer = 0;      // reset state timer
          goal(cg);
        }
      }*/
    // }  //
    // else {
    //   stateTimer = 0;  // reset timer เมื่อไม่ได้อยู่ใน kickState == 2
    // }
  }  //
  // NO BALL //
  else {
    unsigned long searchTimer = millis();
    for (int i = 1; i <= 3; i++) {
        if (millis() - searchTimer > 3000) {
        headZero(90, 270);
        delay(800);
        break;
      }
      if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) break;  // Ball found, exit

      // Backward
      unsigned long backwardTimer = millis();
      while (millis() - backwardTimer < 1000) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) break;
        if (LineD()) {
          if (huskylens.updateBlocks() && huskylens.blockSize[cg]) {
            headZero(90, 270);
            delay(300);
            return;
          }
          break;
        }
        headZero(90, 270);
      }
      wheel(0, 0, 0);
      if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) break;

      // Turn left
      unsigned long leftTurnTimer = millis();
      while (millis() - leftTurnTimer < 200) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) {
          kickState = 2;
          break;
        }
        if (huskylens.updateBlocks() && huskylens.blockSize[cg]) break;
        wheel(55, 55, 55);
      }
      wheel(0, 0, 0);
      if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) break;

      unsigned long centerTimer = millis();
      while (millis() - centerTimer < 350) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) break;
        if (huskylens.updateBlocks() && huskylens.blockSize[cg]) break;
        headZero(0, 0);
      }
      wheel(0, 0, 0);
      if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) break;

      //Turn right
      unsigned long rightTurnTimer = millis();
      while (millis() - rightTurnTimer < 200) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) {
          kickState = 2;
          break;
        }
        if (huskylens.updateBlocks() && huskylens.blockSize[cg]) break;
        wheel(-55, -55, -55);
      }
      wheel(0, 0, 0);
      if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) break;

      centerTimer = millis();
      while (millis() - centerTimer < 350) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1] > 0) break;
        if (huskylens.updateBlocks() && huskylens.blockSize[cg]) break;
        headZero(0, 0);
      }
      wheel(0, 0, 0);
    }
  }
  // }  //
}