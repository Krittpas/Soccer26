#include <POP32.h>
#include <POP32_Huskylens.h>
POP32_Huskylens huskylens;

#define degToRad 0.0174f
#define DEG2RAD 0.017453f
#define RAD2DEG 57.29578f
const float sin30 = 0.500f;
const float cos30 = 0.866f;

// ค่าที่ใช้ปรับหุ่นให้เข้าด้านหน้าตรงลูกบอล
#define rot_Kp 0.50
#define rot_Ki 0.00
#define rot_Kd 0.05
#define sp_rot 160      // ค่า setpoint ที่ลูกบอลอยู่ตรงกลางกล้องแกน x  320/2 = 160
#define rotErrorGap 12  // ค่า Error ที่ยอมให้หุ่นหยุดทำงาน
#define idleSpd 30      // ค่าความเร็วการหมุนเมื่อไม่เจอลูกบอล
float rot_error, rot_pError, rot_i, rot_d, rot_w;
int ballPosX;

// ค่าที่ใช้ปรับหุ่นให้เข้าใกล้ลูกบอล
#define fli_Kp 1.25
#define fli_Ki 0.00
#define fli_Kd 0.10
#define flingErrorGap 12  // ค่า Error ที่ยอมให้หุ่นหยุดทำงาน
float spFli = 190;        // ค่า setpoint ที่ยอมให้ลูกบอลอยู่ใกล้หุ่นมากที่สุด อาจเริ่มที่จุดกลางจอ แกน Y
float fli_error, fli_pError, fli_i, fli_d, fli_spd;
int ballPosY;

// ค่าที่ใช้ปรับหุ่นให้ตรงทิศอ้างอิง
#define alignErrorGap 4
float vecCurve, radCurve;
int discoveState = 1;

// ค่าที่ใช้รักษาทิศหุ่นยนต์
#define head_Kp 2.3f
#define head_Ki 0.0f
#define head_Kd 0.5f
float head_error, head_pError, head_w, head_d, head_i;
// ─── Heading hold PD (รักษาทิศ) ───
#define HEAD_KP 2.3f
#define HEAD_KD 0.5f
float x_pError = 0, x_i = 0, head_pErr = 0;

/* >> ball shooting <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define limPin A0
#define reloadSpd 60

float thetaRad, vx, vy, spd1, spd2, spd3;

float pvYaw = 0.0f, SetPoint = 0.0f, lastYaw = 0.0f;
float pvPitch = 0.0f, pvRoll = 0.0f;
uint8_t rxCnt = 0, rxBuf[8];
#define Yaw (atan2f(sinf((pvYaw - SetPoint) * DEG2RAD), \
                    cosf((pvYaw - SetPoint) * DEG2RAD)) \
             * RAD2DEG)