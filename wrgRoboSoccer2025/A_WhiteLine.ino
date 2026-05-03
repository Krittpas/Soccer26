#define ref 1100  //1750

bool S1() {  // Front
  return analogRead(1) > ref;
}
bool S2() {  // Left
  return analogRead(2) > ref;
}
bool S3() {  // Back Left
  return analogRead(3) > ref;
}
bool S4() {  // Back Right
  return analogRead(4) > ref;
}
bool S5() {  // Right
  return analogRead(5) > ref;
}

bool LineD() {
  return S1() || S2() || S3() || S4() || S5();
}

void avoidLine() {
  // unsigned long avoidTimer = millis();
  // if (millis() - avoidTimer > 4000) {
  //   wheel(-50, 50, 0);
  //   delay(200);
  // }
  float Speed = 120;
  float avoidDirection = 0;
  bool needAvoid = false;

  // ทิศทางการหลบตามระบบ: ขวา 0°, หน้า 90°, ซ้าย 180°, หลัง 270°
  if (S1()) {  // เซนเซอร์หน้า -> ถอยหลัง (270°)
    avoidDirection = 270;
    needAvoid = true;
  } else if (S2()) {  // เซนเซอร์ซ้าย -> เดินขวา (0°)
    avoidDirection = 0;
    needAvoid = true;
  } else if (S3()) {  // เซนเซอร์หลังซ้าย -> เดินหน้าขวา (45°)
    avoidDirection = 45;
    needAvoid = true;
  } else if (S4()) {  // เซนเซอร์หลังขวา -> เดินหน้าซ้าย (135°)
    avoidDirection = 135;
    needAvoid = true;
  } else if (S5()) {  // เซนเซอร์ขวา -> เดินซ้าย (180°)
    avoidDirection = 180;
    needAvoid = true;
  }

  // ถ้าตรวจพบหลายเซนเซอร์ (เส้นมุม)
  if (S1() && S2()) {  // หน้า+ซ้าย -> เดินขวาหลัง (315°)
    avoidDirection = 315;
  } else if (S1() && S5()) {  // หน้า+ขวา -> เดินซ้ายหลัง (225°)
    avoidDirection = 225;
  } else if (S2() && S3()) {  // ซ้าย+หลังซ้าย -> เดินขวา (0°)
    avoidDirection = 0;
  } else if (S4() && S5()) {  // หลังขวา+ขวา -> เดินซ้าย (180°)
    avoidDirection = 180;
  } else if (S3() && S4()) {  // หลังซ้าย+หลังขวา -> เดินหน้า (90°)
    avoidDirection = 90;
  } else if (S2() && S5()) {
    avoidDirection = 90;
  }

  if (!needAvoid) {
    wheel(0, 0, 0);
    return;
  }

  getIMU();
  float rotationControl = -pvYaw * 2.25;
  rotationControl = constrain(rotationControl, -80, 80);
  holonomic(Speed, avoidDirection, rotationControl);
}

/*void avoidLine() {
  float Speed = 120;
  float moveX = 0, moveY = 0;
  // เซนเซอร์หน้า (S1) -> ถอยหลัง (270°)
  if (S1()) {
    moveX += cos(radians(270));
    moveY += sin(radians(270));
  }
  // เซนเซอร์ซ้าย (S2) -> เดินขวา (0°)
  if (S2()) {
    moveX += cos(radians(0));
    moveY += sin(radians(0));
  }
  // เซนเซอร์หลังซ้าย (S3) -> เดินหน้าขวา (45°)
  if (S3()) {
    moveX += cos(radians(90));
    moveY += sin(radians(90));
  }
  // เซนเซอร์หลังขวา (S4) -> เดินหน้าซ้าย (135°)
  if (S4()) {
    moveX += cos(radians(90));
    moveY += sin(radians(90));
  }
  // เซนเซอร์ขวา (S5) -> เดินซ้าย (180°)
  if (S5()) {
    moveX += cos(radians(180));
    moveY += sin(radians(180));
  }
  // ถ้าไม่มีการตรวจจับเส้น ให้หยุด
  // if (moveX == 0 && moveY == 0) return;

  // if (moveX == 0 && moveY == 0) {
  //   wheel(0, 0, 0);
  //   return;
  // }
  // คำนวณทิศทางการหลบ
  float avoidDirection = atan2(moveY, moveX) * 180 / M_PI;
  if (avoidDirection < 0) avoidDirection += 360;

  getIMU();
  float rotationControl = -pvYaw * 2.0;
  rotationControl = constrain(rotationControl, -100, 100);

  holonomic(Speed, avoidDirection, rotationControl);
}*/