/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║   ROBO SOCCER 2026 — BEST VERSION                           ║
 * ║   Platform : POP-32i (STM32F103)                            ║
 * ║   Vision   : HuskyLens (Color Recognition)                  ║
 * ║   IMU      : ZX-IMU (Serial1 @ 115200)                      ║
 * ║   Drive    : 3-wheel Holonomic Omni                         ║
 * ║   Kicker   : Motor 4 + Limit switch PA0                     ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * ALGORITHM: Vector-path chase (angle to ball via atan2) +
 *            PD heading hold + tangent-line surround +
 *            goal-color align before shoot.
 *
 * FILES:
 *   RoboSoccer2026.ino  — constants, globals, setup()
 *   A_imu.ino           — IMU read / zero / heading hold
 *   B_motion.ino        — wheel(), holonomic(), headZero()
 *   C_kicker.ino        — shoot(), reload(), kick()
 *   D_vision.ino        — updateVision() via HuskyLens
 *   E_line.ino          — line sensors S1–S5, avoidLine()
 *   F_chase.ino         — doChase()  (attack AI)
 *   G_goal.ino          — goal()     (align + shoot sequence)
 *   H_search.ino        — doSearch() (ball-lost recovery)
 *   I_penalty.ino       — penaltyKick()
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

/* ─── Line sensors (analog pins) ─────────────────────────── */
#define LINE_F   1
#define LINE_L   2
#define LINE_BL  3
#define LINE_BR  4
#define LINE_R   5
#define LINE_REF 1600   // calibrate on actual carpet + light

/* ─── Kicker ──────────────────────────────────────────────── */
#define LIM_PIN      PA0
#define LIM_TRIG     600
#define RELOAD_SPD   65
#define RELOAD_MS    900

/* ─── PID / tuning constants ─────────────────────────────── */
// Heading-hold PD  (keeps robot facing 0° / SetPoint)
#define HEAD_KP  2.5f
#define HEAD_KD  0.6f

// Chase: rotation (X-axis of camera, center = 160 px)
#define ROT_KP   0.50f
#define ROT_KD   0.05f
#define ROT_DEAD 12      // px dead-zone

// Chase: forward (Y-axis of camera, setpoint = close)
#define FWD_KP   1.30f
#define FWD_KD   0.10f
#define FWD_DEAD 12
#define FWD_SP   185     // px  — "ball this close = ready to kick"

// Tangent-line surround radius
#define SURROUND_R 55.0f  // px in camera space

// Goal-align
#define GOAL_KP    0.30f
#define GOAL_DEAD  25    // px

// Alignment acceptance
#define ALIGN_DEG  2.5f  // degrees

/* ─── Speeds ──────────────────────────────────────────────── */
#define SPD_CHASE_MAX  95
#define SPD_CHASE_MIN  35
#define SPD_LINE       120
#define SPD_SEARCH     55
#define SPD_GOAL       75
#define SPD_PUSH       95
#define SPD_DEFEND     60

/* ─── Timers ──────────────────────────────────────────────── */
#define GOAL_TIMEOUT_MS   500
#define PENALTY_PUSH_MS   550
#define SEARCH_BACK_MS   1200

/* ─── Global: IMU ─────────────────────────────────────────── */
float pvYaw = 0.f, pvPitch = 0.f, pvRoll = 0.f;
float SetPoint = 0.f;
uint8_t rxCnt = 0, rxBuf[8];

// Yaw error wrapped to ±180°
#define YawErr  (atan2f(sinf((pvYaw - SetPoint)*DEG2RAD), \
                        cosf((pvYaw - SetPoint)*DEG2RAD)) * RAD2DEG)

/* ─── Global: Vision ──────────────────────────────────────── */
int  ballPosX = 160, ballPosY = 0;
int  goalPosX = 160, goalPosY = 0;
bool ballSeen  = false;
bool goalSeen  = false;

/* ─── Global: PID state ───────────────────────────────────── */
float x_pErr = 0.f, x_i = 0.f;   // heading hold integrator
float rot_pErr = 0.f;             // rotation D term
float fwd_pErr = 0.f;             // forward D term

/* ─── Global: motor kinematics ────────────────────────────── */
float thetaRad, vx, vy, spd1, spd2, spd3;

/* ─── Global: game state ──────────────────────────────────── */
int  mode      = 1;   // 1=Attack 2=Defend 3=Extra 4=LeftPen 5=RightPen 6=Penalty 7=TestKick
int  cg        = 2;   // color goal: 2=Yellow 3=Blue
int  kickReady = 1;   // 1 = kicker loaded and ready
