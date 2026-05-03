const int ref = 1500;

bool digital(int port) {
  return analogRead(port) < ref;  // >
}

bool detectLine() {
  return digital(2) || digital(3) || digital(4) || digital(5);
}

bool isLine1() {
  return digital(2);
}
bool isLine2() {
  return digital(3);
}
bool isLine3() {
  return digital(4);
}
bool isLine4() {
  return digital(5);
}

bool detectFront() {
  return isLine1() || isLine3();
}
bool detectBack() {
  return isLine2() || isLine4();
}
bool detectLeft() {
  return isLine1() || isLine2();
}
bool detectRight() {
  return isLine3() || isLine4();
}

int getEscapeDirection() {
  bool s1 = isLine1();
  bool s2 = isLine2();
  bool s3 = isLine3();
  bool s4 = isLine4();

  // มุม: 90 = หน้า, 270 = หลัง, 0 = ขวา, 180 = ซ้าย

  /* === Corners === */
  if (s1 && s3) return 270;  // หน้าทั้งคู่ -> ถอยหลัง
  if (s2 && s4) return 90;   // หลังทั้งคู่ -> ไปข้างหน้า
  if (s1 && s2) return 0;    // ซ้ายทั้งคู่ -> ไปขวา
  if (s3 && s4) return 180;  // ขวาทั้งคู่ -> ไปซ้าย

  /* === Diagonal === */
  if (s1 && s4) return 315;  // หน้าซ้าย+หลังขวา -> ถอยหลัง-ขวา
  if (s3 && s2) return 225;  // หน้าขวา+หลังซ้าย -> ถอยหลัง-ซ้าย

  /* === Single === */
  if (s1) return 315;  // หน้าซ้าย -> ถอยหลัง-ขวา
  if (s2) return 45;   // หลังซ้าย -> หน้า-ขวา
  if (s3) return 225;  // หน้าขวา -> ถอยหลัง-ซ้าย
  if (s4) return 135;  // หลังขวา -> หน้า-ซ้าย

  /* === Not Detect Line === */
  return -1;
}

int getEscapeSpeed();      // คืนค่าความเร็ว 0-100
int getEscapeDirection();  // คืนค่ามุม 0-359 หรือ -1
int getEscapeRotation();   // คืนค่าการหมุน -20 ถึง 20

int getEscapeSpeed() {
  int lineCount = isLine1() + isLine2() + isLine3() + isLine4();

  if (lineCount >= 3) {
    return 100;  // เจอเยอะ = หนีเร็ว
  } else if (lineCount == 2) {
    return 80;
  } else if (lineCount == 1) {
    return 65;  // เจอนิดหน่อย = หนีปานกลาง
  }

  return 0;
}

int getEscapeRotation() {
  if (detectLeft() && !detectRight()) {
    return -20;
  } else if (detectRight() && !detectLeft()) {
    return 20;
  }

  return 0;
}

void avoidLine() {
  if (!detectLine()) return;

  int escapeSpeed = getEscapeSpeed();
  int escapeDirection = getEscapeDirection();
  int escapeRotation = getEscapeRotation();

  if (escapeDirection == -1) return;

  unsigned long tEscape = millis();
  while (millis() - tEscape < 275) {
    // holonomic(escapeSpeed, escapeDirection, escapeRotation);
    headZero(escapeSpeed, escapeDirection);
    if (!detectLine()) break;
  }

  wheel(0, 0, 0);
  delay(50);
}