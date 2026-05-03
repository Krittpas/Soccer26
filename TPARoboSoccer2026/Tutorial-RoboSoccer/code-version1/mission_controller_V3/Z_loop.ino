void loop() {
  if (SW_OK()) {  //กดปุ่ม OK หยุดการทำงาน
    beep();
    oled.clearDisplay();
    oled.text(2, 0, "SW_A => Test_Shoot");
    oled.text(3, 0, "SW_B => RUN");
    oled.show();
    while (!SW_B()) {
      if (SW_A()) {
        shoot();   //ยิง
        reload();  //เก็บก้านยิง
      }
      getIMU();
      headZero(0, 0);
    }
    oled.clearDisplay();
    oled.text(0, 0, "RUN");
    oled.show();
  }
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;
    /*if (analog(A2) > 2000) {
      wheel(0, 0, 0);
      delay(200);
      holonomic(30, 270, 0);
      delay(200);
    }*/

    for (int i = 0; i < 8; i++) {
      getIMU();
    }
    if (discoveState) {  //วิ่งปรับหุ่นยนต์ให้ใกล้ลูกบอลมากที่สุด discoveState=1
      rot_error = sp_rot - ballPosX;
      rot_d = rot_d + rot_error;
      rot_d = constrain(rot_d, -100, 100);
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
      fli_spd = constrain(fli_spd, -50, 50);

      holonomic(fli_spd, 90, rot_w);
      if ((abs(rot_error) < rotErrorGap) && (abs(fli_error) < flingErrorGap)) {
        wheel(0, 0, 0);
        lastYaw = pvYaw;  //บันทึกทิศล่า
        discoveState = 0;
      }

    } else {  //จะทำงานเมื่อเจอลูกบอลแต่ทิศไม่ตรงที่จะยิ่ง
      // หุ่นเลือกทิศทางที่ใกล้ที่สุด ที่จะปรับท้ายหุ่นหาลูกบอล
      if (lastYaw < 0) {
        vecCurve = 0;
        radCurve = 15;
      } else {
        vecCurve = 180;
        radCurve = -15;
      }
      holonomic(40, vecCurve, radCurve);
      if (abs(pvYaw) < alignErrorGap) {      //เมื่อทิศอยู่ในค่าที่รับได้
        rot_error = sp_rot - ballPosX;       //คำนวนหาค่า Error ว่าลูกบอลอยู่ตรงกลางหรือไม่
        if (abs(rot_error) < rotErrorGap) {  //ถ้าลูกบอลอยู่ตรงกลางให้ทำการยิง
          holonomic(50, 90, 0);              //เดินหน้าตรงความเร็ว 50%
          delay(1000);                       // นาน 1 วินาที
          beep();
          shoot();   //ยิง
          reload();  //เก็บก้านยิง
        }
        discoveState = 1;
      }
    }
  } else {                            //หมุนตัวหาลูกบอล
    int sideRot = sp_rot - ballPosX;  //คำนวนทิศการหมุนหาลูกบอลเมื่อเจอล่าสุด
    holonomic(0, 0, sideRot / abs(sideRot) * idleSpd);
    discoveState = 1;  //เตรียมพร้อมไปหาลูกบอลเมื่อเจอบอลอีกครั้ง
  }
}