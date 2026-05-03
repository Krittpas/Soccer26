void loop() {
  static unsigned long tBallLost = 0;
  static unsigned long tState = 0;
  static unsigned long tCurve = 0;
  static bool seeBall = false;

  updateIMU();

  /**/
  if (detectLine()) {
    avoidLine();
    return;
  }
  /**/

  if (huskylens.updateBlocks()) {
    ballPosX = constrain(huskylens.blockInfo[1][0].x, 0, 320);
    ballPosY = constrain(huskylens.blockInfo[1][0].y, 0, 240);
    goalPosX = constrain(huskylens.blockInfo[colorGoal][0].x, 0, 320);
    goalPosY = constrain(huskylens.blockInfo[colorGoal][0].y, 0, 240);

    /* === See Ball === */
    if (huskylens.blockSize[1]) {
      tBallLost = millis();
      seeBall = true;

      /* === KickState 1 === */
      if (kickState == 1) {
        diffX = ballPosX - sp_rot;
        diffY = spFli - ballPosY;

        distance = sqrt(diffX * diffX + diffY * diffY);
        moveSpeed = constrain(distance, 35, 45);
        angleToball = atan2(diffY, diffX) * 180 / PI;

        /**/
        if (detectLine()) {
          avoidLine();
          return;
        }
        /**/

        headZero(moveSpeed, fmod(angleToball + 360, 360));

        if (distance <= 100 && abs(pvYaw) <= alignErrorGap && abs(diffX) <= 5) {
          // goal(colorGoal);
          kick();
        }
      }
      /* === KickState 2 === */
      else if (kickState == 2) {
        if (millis() - tState > 1750) {
          kickState = 1;
          tState = 0;
          wheel(0, 0, 0);
          return;
        }

        rot_error = sp_rot - ballPosX;
        rot_i = rot_i + rot_error;
        rot_i = constrain(rot_i, -100, 100);
        rot_d = rot_error - rot_pError;
        rot_pError = rot_error;
        rot_w = (rot_error * rot_Kp) + (rot_i * rot_Ki) + (rot_d * rot_Kd);
        rot_w = constrain(rot_w, -100, 100);

        fli_error = spFli - ballPosY;
        fli_i = fli_i + fli_error;
        fli_i = constrain(fli_i, -100, 100);
        fli_d = fli_error - fli_pError;
        fli_pError = fli_error;
        fli_spd = fli_error * fli_Kp + fli_i * fli_Ki + fli_d * fli_Kd;
        fli_spd = constrain(fli_spd, -100, 100);

        /**/
        if (detectLine()) {
          avoidLine();
          return;
        }
        /**/

        holonomic(fli_spd, 90, rot_w);

        if ((abs(rot_error) <= rotErrorGap) && (abs(fli_error) <= flingErrorGap)) {
          wheel(0, 0, 0);

          if (tCurve == 0) {
            tCurve = millis();
          }

          if (pvYaw < 0) {
            vecCurve = 0;
            radCurve = 15;
          } else {
            vecCurve = 180;
            radCurve = -15;
          }

          /**/
          if (detectLine()) {
            avoidLine();
            return;
          }
          /**/

          holonomic(50, vecCurve, radCurve);

          if (abs(pvYaw) <= 10 || (millis() - tCurve > 1250)) {
            wheel(0, 0, 0);
            tCurve = 0;
            tState = 0;
            goal(colorGoal);
          }
        }
      }
    }

    /* === Not See Ball === */
    else {
      static int searchState = 0;
      static unsigned long searchTimer = 0;

      if (detectLine()) {
        avoidLine();
        searchState = 0;
        searchTimer = 0;
        kickState = 2;  // ⭐ เปลี่ยนเป็น kickState 2
        return;
      }

      if (searchState == 0) {  // ถอยหลัง
        kickState = 2;         // ⭐ เปลี่ยนเป็น kickState 2 ขณะค้นหา

        if (searchTimer == 0) searchTimer = millis();
        headZero(75, 270);
        if (millis() - searchTimer >= 2500) {
          wheel(0, 0, 0);
          searchState = 1;
          searchTimer = 0;
        }
      } else if (searchState == 1) {  // หมุนซ้าย
        kickState = 2;                // ⭐ เปลี่ยนเป็น kickState 2

        if (searchTimer == 0) searchTimer = millis();
        wheel(55, 55, 55);
        if (millis() - searchTimer >= 300) {
          wheel(0, 0, 0);
          searchState = 2;
          searchTimer = 0;
        }
      } else if (searchState == 2) {  // หมุนขวา
        kickState = 2;                // ⭐ เปลี่ยนเป็น kickState 2

        if (searchTimer == 0) searchTimer = millis();
        wheel(-55, -55, -55);
        if (millis() - searchTimer >= 500) {
          wheel(0, 0, 0);
          searchState = 3;
          searchTimer = 0;
        }
      } else if (searchState == 3) {  // กลับหน้าตรง
        kickState = 2;                // ⭐ เปลี่ยนเป็น kickState 2

        if (searchTimer == 0) searchTimer = millis();
        headZero(0, 90);
        if (millis() - searchTimer >= 250) {
          wheel(0, 0, 0);
          searchState = 0;
          searchTimer = 0;
        }
      }
    }
  }
}