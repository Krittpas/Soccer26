int moveDirection = 0;

void Onana() {
  sound(500, 500);
  while (1) {
    if (LineD()) {
      //avoidLine();
      return;
    }
    if (huskylens.updateBlocks() && huskylens.blockSize[1]) {

      ballPosX = constrain(huskylens.blockInfo[1][0].x, 0, 320);
      ballPosY = constrain(huskylens.blockInfo[1][0].y, 0, 240);
      for (int i = 0; i < 8; i++) {
        getIMU();
      }
      if (kickState) {
        getIMU();

        float rot_error = 160 - ballPosX;
        float moveSpeed = constrain(abs(rot_error) * 1.0, 0, 100);
        moveDirection = rot_error > 0 ? 180 : 0;

        float x_error = -pvYaw;
        x_i = x_i + x_error;
        x_i = constrain(x_i, -100, 100);
        float x_d = x_error - x_pError;
        x_pError = x_error;
        float x_w = (x_error * 0.75) + (x_d * 0.05);
        x_w = constrain(x_w, -100, 100);

        holonomic(moveSpeed, moveDirection, x_w);

        if (abs(rot_error) <= 5) {
          wheel(0, 0, 0);
          kickState = 1;
          break;
        }
      }
    }  // no ball
    else {
      getIMU();
      x_error = -pvYaw;
      x_i = x_i + x_error;
      x_i = constrain(x_i, -100, 100);
      x_d = x_error - x_pError;
      x_pError = x_error;
      x_w = (x_error * 2.0) + (x_d * 0.15);
      x_w = constrain(x_w, -100, 100);
      holonomic(0, 0, x_w);

      if (abs(pvYaw) < alignErrorGap) {
        wheel(0, 0, 0);
      }
    }
  }
}