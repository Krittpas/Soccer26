void wheel(int s1, int s2, int s3) {
  motor(1, s1);
  motor(2, s2);
  motor(3, s3);
}

void holonomic(float spd, float theta, float omega) {
  thetaRad = theta * degToRad;
  vx = spd * cos(thetaRad);
  vy = spd * sin(thetaRad);
  spd1 = vy * cos30 - vx * sin30 + omega;
  spd2 = -vy * cos30 - vx * sin30 + omega;
  spd3 = vx + omega;
  wheel(spd1, spd2, spd3);
}

void headZero(int spd, int dir) {
  getIMU();
  x_error = -pvYaw;
  x_i = x_i + x_error;
  x_i = constrain(x_i, -100, 100);
  x_d = x_error - x_pError;
  x_pError = x_error;
  x_w = (x_error * 1.0) + (x_d * 0.05);
  x_w = constrain(x_w, -100, 100);

  holonomic(spd, dir, x_w);
}