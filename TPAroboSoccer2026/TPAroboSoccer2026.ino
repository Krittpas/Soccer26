#include <POP32.h>
#include <POP32_Huskylens.h>
#include <math.h>

POP32_Huskylens huskylens;

/* === MATH === */
const float degToRad = PI / 180.0;
const float cos30 = cos(30.0 * degToRad);
const float sin30 = sin(30.0 * degToRad);

/* === ROTATE === */
const float rot_Kp = 0.5;
const float rot_Ki = 0.0;
const float rot_Kd = 0.0;
const float sp_rot = 160;
//
const float rotErrorGap = 10;
const int idleSpd = 60;
float rot_error, rot_pError, rot_i, rot_d, rot_w;

/* === FLING === */
const float fli_Kp = 1.25;  //1.5
const float fli_Ki = 0.0;
const float fli_Kd = 0.0;
const float spFli = 220;
//
const float flingErrorGap = 10;
float fli_error, fli_pError, fli_i, fli_d, fli_spd;

/* === ALIGN === */
const float ali_Kp = 0.8;
const float ali_Ki = 0.0;
const float ali_Kd = 0.0;
const float alignErrorGap = 3;
float ali_error, ali_pError, ali_d, ali_vec, vecCurve, radCurve;

/* === HEADING === */
const float head_Kp = 2.25;
const float head_Ki = 0.0;
const float head_Kd = 0.5;
float head_error, head_pError, head_w, head_d, head_i;

/* === KICK === */
#define limPin PA0
const int reloadSpd = 100;

/* === CALCULATE === */
float x_error = 0, x_i = 0, x_d = 0, x_pError = 0, x_w = 0;
float distance = 0, moveSpeed = 0, moveDirection = 0, turnSpeed = 0, angleToball = 0, diffX = 0, diffY = 0;
int ball_error = 0, goal_error = 0;

/* === Goal Ball === */
int ballPosX = 0, ballPosY = 0;
int goalPosX = 0, goalPosY = 0;

/* === Setup === */
int colorGoal = 2;
int mode = 0;
int kickState = 1;