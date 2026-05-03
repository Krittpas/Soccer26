/* ══════════════════════════════════════════════════════
   §14  SETUP
   ══════════════════════════════════════════════════════ */

void setup() {
  Serial1.begin(115200);
  delay(3000);

  reload();

  while (!huskylens.begin(Wire)) {
    oled.text(0, 0, "HuskyLens FAIL!");
    oled.show();
    delay(200);
  }

  // ─── Setup Menu ───────────────────────────────────
  // SetPoint = pvYaw;
  mode = 1;
  cg = 2;
  while (!SW_B()) {
    getIMUU();
    if (SW_OK()) {
      cg = (cg == 2) ? 3 : 2;
      delay(250);
    }
    if (SW_A()) {
      // zeroYaw();
      // delay(250);
      SetPoint = pvYaw;
    }
    //if (SW_B()) { SetPoint = pvYaw; }
    mode = knob(7) + 1;
    drawSetupScreen();
  }

  oled.clearDisplay();
  if (mode == 2) {
    uint32_t tDF = millis();
    while (millis() - tDF < 4750) {
      oled.text(0, 0, "DEFEND");
      oled.show();
      updateVision();
      if (ballSeen && ballPosY > 115) break;
    }
  } else if (mode == 6) {
    oled.text(0, 0, "PENALDO");
    oled.show();
    // penaltyKick();
  } else if (mode == 7) {
    oled.text(0, 0, "TEST KICK");
    oled.show();
    while (true) {
      if (SW_A()) shoot();
      if (SW_B()) reload();
      if (SW_OK()) kick();
    }
  } else if (mode == 8) {
    oled.text(0, 0, "ONANA   ");
    oled.show();
    float rot_error, rot_pError, rot_w;
    while (1) {
      getIMU();
      if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
        ballPosX = constrain(huskylens.blockInfo[SIG_BALL][0].x, 0, 320);
        rot_error = (160 - ballPosX);
        rot_w = rot_error * 0.75f;
        rot_w = constrain(rot_w, -100, 100);
        float dir = (rot_w > 0 ? 180 : 0);  //เลือกทิศซ้ายหรือขวา
        getIMU();
        headZero(abs(rot_w), dir);
      } else {
        holonomic(0, 0, 0);
      }
    }
    holonomic(0, 0, 0);
  }
  sound(1500, 100);
  oled.text(0, 0, "[BenChan] GO!");
  oled.show();

  int count = 0;
  const int sp_rot = 160;
  const int spFli = FWD_SP;
  const int rotErrorGap = ROT_DEAD;
  const int flingErrorGap = FWD_DEAD;
  const float alignErrorGap = ALIGN_ZONE;
  const int idleSpd = 30;

  float rot_error = 0, rot_pError = 0, rot_i = 0, rot_d = 0, rot_w = 0;
  float rot_Kp = ROT_KP, rot_Ki = 0.0f, rot_Kd = ROT_KD;
  float fli_error = 0, fli_pError = 0, fli_i = 0, fli_d = 0, fli_spd = 0;
  float fli_Kp = FWD_KP, fli_Ki = 0.0f, fli_Kd = FWD_KD;
  float lastYaw = 0;
  int vecCurve = 0, radCurve = 0;
  int discoveState = 1;

  while (1) {
    if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
      ballPosX = constrain(huskylens.blockInfo[SIG_BALL][0].x, 0, 320);
      ballPosY = constrain(huskylens.blockInfo[SIG_BALL][0].y, 0, 240);

      if (analogRead(1) > LINE_REF) {
        holonomic(0, 0, 0);
        delay(100);
        holonomic(50, 270, 0);
        delay(250);
      }

      getIMU();

      if (discoveState) {
        // ── หมุน PD ──
        rot_error = sp_rot - ballPosX;
        rot_d = rot_error - rot_pError;
        rot_pError = rot_error;
        rot_w = (rot_error * rot_Kp) + (rot_i * rot_Ki) + (rot_d * rot_Kd);
        rot_w = constrain(rot_w, -100, 100);

        // ── เดิน PID ──
        fli_error = spFli - ballPosY;
        fli_i = constrain(fli_i + fli_error, -100, 100);
        fli_d = fli_error - fli_pError;
        fli_pError = fli_error;
        fli_spd = fli_error * fli_Kp + fli_i * fli_Ki + fli_d * fli_Kd;
        fli_spd = constrain(fli_spd, -100, 100);

        if ((abs(rot_error) < rotErrorGap) && (abs(fli_error) < flingErrorGap)) {
          wheel(0, 0, 0);
          lastYaw = Yaw;  //
          discoveState = 0;
        } else {
          holonomic(fli_spd, 90, rot_w);
        }

      } else {
        if (lastYaw < 0) {
          vecCurve = 0;
          radCurve = 15;
        } else {
          vecCurve = 180;
          radCurve = -15;
        }
        holonomic(40, vecCurve, radCurve);

        if (abs(Yaw) <= 1.5) {  //
          rot_error = sp_rot - ballPosX;

          if (abs(rot_error) < rotErrorGap) {
            count++;
          } else {
            count = 0;
          }

          if (count >= 5) {
            if (mode == 6) {
              uint32_t pushT = millis();
              while (millis() - pushT < 200) {
                if (analog(1) > LINE_REF) {
                  holonomic(0, 0, 0);
                  break;
                }
                holonomic(70, 90, 0);
              }
              shoot();
              stopAll();
              reload();
              while (!SW_OK()) {
                wheel(0, 0, 0);
              }
            } else {
              uint32_t pushT = millis();
              while (millis() - pushT < 555) {
                if (analog(1) > LINE_REF) {
                  holonomic(0, 0, 0);
                  break;
                }
                holonomic(70, 90, 0);
              }
              kick();
              count = 0;
              holonomic(50, 270, 0);
              delay(375);
            }
          }

          discoveState = 1;
        }
      }

    } else {
      int sideRot = sp_rot - ballPosX;  //คำนวนทิศการหมุนหาลูกบอลเมื่อเจอล่าสุด
      holonomic(0, 0, sideRot / abs(sideRot) * idleSpd);
      discoveState = 1;  //เตรียมพร้อมไปหาลูกบอลเมื่อเจอบอลอีกครั้ง
    }
  }
}