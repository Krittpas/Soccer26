void wheel(int s1, int s2, int s3) {
  motor(1, s1);
  motor(2, s2);
  motor(3, s3);
}

void holonomic(float spd, float theta, float omega) {
  float thetaRad = theta * degToRad;
  float vx = spd * cos(thetaRad);
  float vy = spd * sin(thetaRad);
  float spd1 = -vx * sin30 + vy * cos30 + omega;
  float spd2 = -vx * sin30 - vy * cos30 + omega;
  float spd3 = vx + omega;

  wheel((int)spd1, (int)spd2, (int)spd3);
}

void headZero(int spd, int dir) {
  getIMU();

  x_error = -pvYaw;
  x_i += x_error;
  x_i = constrain(x_i, -100.0, 100.0);
  x_d = x_error - x_pError;
  x_pError = x_error;
  x_w = (x_error * head_Kp) + (x_i * head_Ki) + (x_d * head_Kd);
  x_w = constrain(x_w, -100.0, 100.0);

  holonomic(spd, dir, x_w);
}