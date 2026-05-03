void goal(int colorGoal) {
  bool goall = false;
  unsigned long tStart = millis();

  while (millis() - tStart < 3000) {
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
        ball_error = sp_rot - ballPosX;
        goal_error = sp_rot - goalPosX;

        diffX = ballPosX - sp_rot;
        diffY = ballPosY;

        angleToball = atan2(diffY, diffX) * 180 / PI;
        if (angleToball < 0) {
          angleToball += 360;
        }
        moveDirection = angleToball;

        turnSpeed = 0;
        if (abs(goal_error) > 3) {
          float Kp = 0.3;
          turnSpeed = constrain(goal_error * Kp, -100, 100);
        }

        holonomic(70, moveDirection, turnSpeed);

        if (abs(goal_error) <= 20 && abs(ball_error) <= 20) {
          unsigned long tShoot = millis();
          while (millis() - tShoot < 75) {
            /**/
            if (detectLine()) {
              avoidLine();
              return;
            }
            /**/
            holonomic(100, 90, 0);
          }
          kick();
          goall = true;
          break;
        }
      }
      /* === Not See Ball . See Goal === */
      else if (!huskylens.blockSize[1] && huskylens.blockSize[colorGoal]) {
        unsigned long t1 = millis();
        while (millis() - t1 < 75) {
          holonomic(100, 90, 0);
        }
        kick();
        break;
      }
      /* === Not See Goal === */
      else if (!huskylens.blockSize[colorGoal]) {
        unsigned long t1 = millis();
        while (millis() - t1 < 75) {
          holonomic(100, 90, 0);
        }
        kick();
        break;
      }

      if (millis() - tStart > 500) {
        kick();
        break;
      }
    }
  }

  wheel(0, 0, 0);
  kickState = 1;
}