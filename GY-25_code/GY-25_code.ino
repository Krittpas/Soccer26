#include <POP32.h>

/*
https://www.facebook.com/chanutpon.thammalee
https://www.instagram.com/accounts/settings/v2/your_account/?next=%2F
https://grabcad.com/chanutpon.thammalee-1
*/

void zero( void )
{
  Serial1.write(0XA5);
  delay(100);
  Serial1.write(0X54);
  delay(100);
  // pitch correction roll angle
  Serial1.write(0XA5);
  delay(100);
  Serial1.write(0X55);
  delay(100);
  // zero degree heading
  Serial1.write(0XA5);
  delay(100);
  Serial1.write(0X52);
  delay(100);
  // automatic mode
}

float pvYaw,pvRoll,pvPitch;
uint8_t rxCnt = 0, rxBuf[8];
bool getIMU() {
  while (Serial1.available()) {
    rxBuf[rxCnt] = Serial1.read();
    if (rxCnt == 0 && rxBuf[0] != 0xAA) return false;
    rxCnt++;
    if (rxCnt == 8) { // package is complete
      rxCnt = 0;
      if (rxBuf[0] == 0xAA && rxBuf[7] == 0x55) { // data package is correct
        pvYaw = -(int16_t)(rxBuf[1] << 8 | rxBuf[2]) / 100.f;
        pvPitch = (int16_t)(rxBuf[3] << 8 | rxBuf[4]) / 100.f;
        pvRoll = (int16_t)(rxBuf[5] << 8 | rxBuf[6]) / 100.f;
        return true;
      }}}
  return false;
}

void updateIMU( void )
{
  for ( byte i = 0 ; i < 8 ; i++ )
  {
    getIMU();
  }
}

float SetPoint;

#define Yaw ( atan2( sin( ( pvYaw - SetPoint ) * PI / 180), cos( ( pvYaw - SetPoint ) * PI / 180) ) * 180 / PI )

void setup()
{
  Serial1.begin(115200);
  delay(100);
}

void loop()
{
  updateIMU();
  if( SW_A() )
  {
    zero(); //หากต้องการ Calibrate ให้ใช้งานคำสั่งนี้
  }
  if( SW_B() )
  {
    SetPoint = pvYaw; //เปลี่ยนจุดนี้ให้เป็นจุดเริ่มต้นแกนหมุน
  }
  Serial.print("\r\t");
  Serial.print("pvYaw ");
  Serial.print(pvYaw,5);
  Serial.print("\r\t");
  Serial.print("pvPitch ");
  Serial.print(pvPitch,5);
  Serial.print("\r\t");
  Serial.print("pvRoll ");
  Serial.println(pvRoll,5);
}
