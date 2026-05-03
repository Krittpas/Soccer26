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
void headZero(float spd, float dir) {
  getIMU();
  // float err = -Yaw;  // error จาก SetPoint
  float err = -pvYaw;
  float d = err - x_pError;
  x_i += err;
  x_i = constrain(x_i, -100, 100);
  float w = err * HEAD_KP + d * HEAD_KD;
  w = constrain(w, -100, 100);
  x_pError = err;
  holonomic(spd, dir, w);
}