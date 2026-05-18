/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║   ROBO SOCCER 2026 — BEST VERSION                           ║
 * ║   Platform : POP-32i (STM32F103)                            ║
 * ║   Vision   : HuskyLens (Color Recognition)                  ║
 * ║   IMU      : ZX-IMU (Serial1 @ 115200)                      ║
 * ║   Drive    : 3-wheel Holonomic Omni                         ║
 * ║   Kicker   : Motor 4 + Limit switch PA0                     ║
 * ║   Line     : 3× ZX-03 (Front, Back-L, Back-R)              ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * FILES:
 *   RoboSoccer2026.ino  — constants, globals
 *   A_imu.ino           — IMU read / zero / heading hold
 *   B_motion.ino        — wheel(), holonomic(), headZero()
 *   C_kicker.ino        — shoot(), reload(), kick()
 *   D_vision.ino        — updateVision() via HuskyLens
 *   E_line.ino          — 3× line sensors, avoidLine()
 *   F_chase.ino         — doChase()  (tangent-line AI)
 *   G_goal.ino          — goal()     (align + shoot)
 *   H_search.ino        — doSearch() (ball-lost recovery)
 *   I_penalty.ino       — penaltyKick()
 *   J_oled.ino          — drawSensorCheck(), drawBallRadar()
 *   Z_setup.ino         — setup menu + loop()
 */

#include <POP32.h>
#include <POP32_Huskylens.h>
#include <math.h>

POP32_Huskylens huskylens;

/* ─── Math ────────────────────────────────────────────────── */
#define DEG2RAD  0.017453293f
#define RAD2DEG  57.295779f
static const float SIN30 = 0.5f;
static const float COS30 = 0.8660254f;

/* ─── HuskyLens IDs ───────────────────────────────────────── */
#define SIG_BALL     1
#define SIG_GOAL_YEL 2
#define SIG_GOAL_BLU 3

/* ─── Line sensors — 3 sensors (analog pins) ─────────────── */
// S1 = Front    → port 1
// S3 = Back-L   → port 3
// S4 = Back-R   → port 4
#define LINE_F   1
#define LINE_BL  2
#define LINE_BR  3
#define LINE_REF 1000

/* ─── Kicker ──────────────────────────────────────────────── */
#define LIM_PIN      PA0
#define LIM_TRIG     500
#define RELOAD_SPD   60
#define RELOAD_MS    800

/* ─── PID / tuning ────────────────────────────────────────── */
#define HEAD_KP  2.0f
#define HEAD_KD  0.5f

#define ROT_KP   0.50f
#define ROT_KD   0.05f
#define ROT_DEAD 12

#define FWD_KP   1.30f
#define FWD_KD   0.10f
#define FWD_SP   200      // px — "บอลใกล้พอจะยิง" (ปรับได้ 175–200)

#define SURROUND_R  45.0f // รัศมีวงโคจรรอบบอล (ปรับได้ 45–65)
#define GOAL_KP     0.30f
#define GOAL_DEAD   25    // px tolerance ก่อนยิง 25
#define ALIGN_DEG   2.5f  // องศา tolerance ทิศ

/* ─── Speeds ──────────────────────────────────────────────── */
#define SPD_CHASE_MAX  95
#define SPD_CHASE_MIN  40
#define SPD_LINE       120
#define SPD_SEARCH     55
#define SPD_GOAL       75
#define SPD_PUSH       95
#define SPD_DEFEND     60

/* ─── Timers ──────────────────────────────────────────────── */
#define GOAL_TIMEOUT_MS  500 // 500
#define PENALTY_PUSH_MS  550
#define SEARCH_BACK_MS  1200

/* ─── Global: IMU ─────────────────────────────────────────── */
float pvYaw = 0.f, pvPitch = 0.f, pvRoll = 0.f;
float SetPoint = 0.f;
uint8_t rxCnt = 0, rxBuf[8];

// Yaw error wrapped ±180° relative to SetPoint
#define YawErr (atan2f(sinf((pvYaw-SetPoint)*DEG2RAD), \
                       cosf((pvYaw-SetPoint)*DEG2RAD)) * RAD2DEG)

/* ─── Global: Vision ──────────────────────────────────────── */
int  ballPosX = 160, ballPosY = 0;
int  goalPosX = 160, goalPosY = 0;
bool ballSeen = false, goalSeen = false;

/* ─── Global: PID state ───────────────────────────────────── */
float x_pErr = 0.f, x_i = 0.f;  // heading-hold
float rot_pErr = 0.f;            // chase rotation D
float fwd_pErr = 0.f;            // chase forward D

/* ─── Global: game state ──────────────────────────────────── */
// mode: 1=Attack 2=Defend 3=Extra 4=LeftPen 5=RightPen
//       6=Penalty 7=TestKick 8=SensorCheck 9=BallRadar
int  mode      = 1;
int  cg        = SIG_GOAL_YEL;
int  kickReady = 1;