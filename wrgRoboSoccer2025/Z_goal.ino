void goal(int goalColor) {
  bool goall = false;
  unsigned long startTime = millis();

  while (!goall) {
    /*if (LineD()) {
      avoidLine();
      return;
    }  //
    else {*/
    if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
      if (huskylens.updateBlocks() && huskylens.blockSize[goalColor]) {
        int goalPosX = constrain(huskylens.blockInfo[goalColor][0].x, 0, 320);
        int goalPosY = constrain(huskylens.blockInfo[goalColor][0].y, 0, 240);
        ballPosX = constrain(huskylens.blockInfo[1][0].x, 0, 320);
        ballPosY = constrain(huskylens.blockInfo[1][0].y, 0, 240);

        for (int i = 0; i < 8; i++) {
          getIMU();
        }

        int ball_error = 160 - ballPosX;
        int goal_error = 160 - goalPosX;

        diffX = ballPosX - 160;
        diffY = ballPosY;
        //
        float angleToball = atan2(diffY, diffX) * 180 / PI;
        if (angleToball < 0) {
          angleToball += 360;
        }
        float moveDirection = angleToball;
        //
        float turnSpeed = 0;
        if (abs(goal_error) > 3) {  //7
          float Kp = 0.3;
          turnSpeed = constrain(goal_error * Kp, -100, 100);
        }

        holonomic(70, moveDirection, turnSpeed);

        if (abs(goal_error) <= 20 && abs(ball_error) <= 20) {  // 20 20 // 40 30
          unsigned long shootTime = millis();
          while (millis() - shootTime < 50) { //450
            // if (LineD()) {
            //   // if (huskylens.updateBlocks() && huskylens.blockSize[cg]) {
            //   //   return;
            //   // }
            //   break;
            // }
            holonomic(100, 90, 0);
          }
          // wheel(0, 0, 0);
          kick();
          goall = true;
          break;
        }
      }                                                                        //
      else if (huskylens.updateBlocks() && !huskylens.blockSize[goalColor]) {  //no goal
        unsigned long Time = millis();
        while (millis() - Time < 50) { //500
          // if (LineD()) break;
          holonomic(100, 90, 0);
        }
        //wheel(0, 0, 0);
        kick();
        break;
      }
    } else {
      break;
    }
    if (millis() - startTime > 350) {  //750 900 1250 1750 1500 1300
      //holonomic(0, 0, 0);
      kick();
      break;
    }  //
    // }
  }
}