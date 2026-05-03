// /*void zeroYaw() {
//   Serial1.begin(115200);
//   delay(100);
//   // Sets data rate to 115200 bps
//   Serial1.write(0XA5);
//   delay(100);
//   Serial1.write(0X54);
//   delay(100);
//   // pitch correction roll angle
//   Serial1.write(0XA5);
//   delay(100);
//   Serial1.write(0X55);
//   delay(100);
//   // zero degree heading
//   Serial1.write(0XA5);
//   delay(100);
//   Serial1.write(0X52);
//   delay(100);
//   // automatic mode
// }*/

// // unsigned long lastUpdateTime = 0;
// // bool imuAdjusted = false;


// bool getIMU() {
//   unsigned long currentTime = millis();

//   // Reset timing after 15s
//   if (currentTime - lastUpdateTime >= 15000) {
//     lastUpdateTime = currentTime;
//     imuAdjusted = false;  // Ready for next adjustment
//   }

//   // Apply adjustment once after 15s
//   if (!imuAdjusted && currentTime - lastUpdateTime >= 15000) {
//     // Negative = left drift, Positive = right drift
//     pvYaw += 0.0f;  // Change this if adjustment needed
//     imuAdjusted = true;
//   }

//   // Read serial data
//   while (Serial1.available()) {
//     rxBuf[rxCnt] = Serial1.read();

//     if (rxCnt == 0 && rxBuf[0] != 0xAA) {
//       return false;  // Invalid start byte
//     }

//     rxCnt++;

//     if (rxCnt == 8) {  // Full packet received
//       rxCnt = 0;

//       if (rxBuf[0] == 0xAA && rxBuf[7] == 0x55) {  // Valid packet
//         pvYaw = (int16_t)(rxBuf[1] << 8 | rxBuf[2]) / 100.0f;

//         if (lastUpdateTime == 0) {
//           lastUpdateTime = currentTime;
//         }

//         return true;
//       }
//     }
//   }

//   return false;  // Incomplete or invalid packet
// }

// void re8imu() {
//   for (int i = 0; i < 8; i++) {
//     if (getIMU()) break;
//   }
// }

// float angleToball;
// float moveDir(float x, float y) {
//   float angleToball = atan2(y, x) * 180 / PI;
//   if (angleToball < 0) angleToball += 360;
//   return angleToball;
// }