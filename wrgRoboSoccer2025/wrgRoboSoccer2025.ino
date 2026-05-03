#include <POP32.h>
#include <POP32_Huskylens.h>
POP32_Huskylens huskylens;
#include <math.h>

#define degToRad 0.0174f
#define sin30 sin(30.f * degToRad)
#define cos30 cos(30.f * degToRad)

// ค่าที่ใช้ปรับหุ่นให้เข้าด้านหน้าตรงลูกบอล
float rot_Kp = 0.5;
float rot_Ki = 0.0;
float rot_Kd = 0.0;
float sp_rot = 160;     // ค่า setpoint ที่ลูกบอลอยู่ตรงกลางกล้องแกน x  320/2 = 160
#define rotErrorGap 12  // ค่า Error ที่ยอมให้หุ่นหยุดทำงาน
#define idleSpd 60      // ค่าความเร็วการหมุนเมื่อไม่เจอลูกบอล
float rot_error, rot_pError, rot_i, rot_d, rot_w;
int ballPosX;

// ค่าที่ใช้ปรับหุ่นให้เข้าใกล้ลูกบอล
float fli_Kp = 1.25;  //1.5
float fli_Ki = 0.0;
float fli_Kd = 0.0;
#define flingErrorGap 12  // ค่า Error ที่ยอมให้หุ่นหยุดทำงาน
float spFli = 190;        // ค่า setpoint ที่ยอมให้ลูกบอลอยู่ใกล้หุ่นมากที่สุด อาจเริ่มที่จุดกลางจอ แกน Y
float fli_error, fli_pError, fli_i, fli_d, fli_spd;
int ballPosY;

// ค่าที่ใช้ปรับหุ่นให้ตรงทิศอ้างอิง
#define ali_Kp 0.8
#define ali_Kd 0.0
#define alignErrorGap 4
float ali_error, ali_pError, ali_d, ali_vec, vecCurve, radCurve;

// ค่าที่ใช้รักษาทิศหุ่นยนต์
#define head_Kp 2.3f  //2.3f
#define head_Ki 0.0f
#define head_Kd 0.5f
float head_error, head_pError, head_w, head_d, head_i;

#define limPin PA0
int reloadSpd = 100;

// float pvYaw, lastYaw;
// uint8_t rxCnt = 0, rxBuf[8];

float thetaRad, vx, vy, spd1, spd2, spd3;

float x_error, x_d, x_w, x_pError, x_i;

int kickState = 1;

int cg = 2;
bool Cansee = false;
int mode;

float diffX, diffY;

int cross = 0;