/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║   ROBO SOCCER 2026 — BEST VERSION                            ║
 * ║   Platform : POP-32i (STM32F103)                             ║
 * ║   Vision   : HuskyLens (Color Recognition)                   ║
 * ║   IMU      : ZX-IMU (Serial1 @ 115200)                       ║
 * ║   Drive    : 3-wheel Holonomic Omni                          ║
 * ║   Kicker   : Motor 4 + Limit switch PA0                      ║
 * ║   Line     : 3× ZX-03 (Front, Back-L, Back-R)                ║
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
#define DEG2RAD 0.017453293f
#define RAD2DEG 57.295779f
static const float SIN30 = 0.5f;
static const float COS30 = 0.8660254f;

/* ─── HuskyLens IDs ───────────────────────────────────────── */
#define SIG_BALL 1
#define SIG_GOAL_YEL 2
#define SIG_GOAL_BLU 3

/* ─── Line sensors — 3 sensors (analog pins) ─────────────── */
// S1 = Front    → port 1
// S2 = Back-L   → port 2
// S3 = Back-R   → port 3
#define LINE_F 1
#define LINE_BL 2
#define LINE_BR 3
#define LINE_REF 1800

/* ─── Kicker ──────────────────────────────────────────────── */
#define LIM_PIN PA0
#define LIM_TRIG 500
#define RELOAD_SPD 60
#define RELOAD_MS 800

/* ─── PID / tuning ────────────────────────────────────────── */
#define HEAD_KP 2.75f
#define HEAD_KI 0.0f
#define HEAD_KD 0.5f

#define FWD_SP 200      // px — "บอลใกล้พอจะยิง" (ปรับได้ 175–200)
#define GOAL_KP 0.3f    // 0.3
#define GOAL_DEAD 20    // px tolerance ก่อนยิง 38
#define BALL_DEAD 20    // px tolerance ก่อนยิง 38
#define ALIGN_DEG 2.0f  // องศา tolerance ทิศ

/* ─── Speeds ──────────────────────────────────────────────── */
#define SPD_CHASE_MAX 90
#define SPD_CHASE_MIN 45  // 50 40
#define SPD_LINE 100
#define SPD_SEARCH 35
#define SPD_GOAL 75
#define SPD_PUSH 100

/* ─── Timers ──────────────────────────────────────────────── */
#define GOAL_PUSH_MS 350      // ระยะเวลา push forward ก่อนยิง 750
#define GOAL_TIMEOUT_MS 1750  // timeout รวมของ goal() function ทั้งหมด 1750
#define GOAL_ALIGN_FRAMES 5   // consecutive frames ที่ต้อง align ก่อน push
#define PENALTY_PUSH_MS 550
#define SEARCH_BACK_MS 750

/* ─── Line avoidance timing ──────────────────────────────── */
#define LINE_ESCAPE_MS 350  // วิ่งหนีต่อเนื่องนานเท่านี้หลัง line trigger 250
#define LINE_HOLDOFF_MS 25  // หยุดนิ่งก่อนกลับ chase หลังพ้นเส้น 50

/* ─── Onana (Goalkeeper) ──────────────────────────────────── */
#define ONANA_SLIDE_KP 1.25f  // slide speed = |err| * this
#define ONANA_SLIDE_MAX 75.f  // max goalkeeper slide speed
#define ONANA_CENTER_DEAD 10  // px deadband before sliding

/* ─── Defend2 (Slide → Attack) ────────────────────────────── */
#define DEFEND2_SLIDE_MS 3750UL  // นาน slide ก่อน switch attack
#define DEFEND2_SLIDE_KP 1.75f   // speed = |ballErr| * this
#define DEFEND2_SLIDE_MAX 75.f   // max slide speed
#define DEFEND2_CENTER_DEAD 15   // px deadband ก่อน slide
#define DEFEND2_BALL_Y 75        // ballPosY เกินนี้ → switch attack ทันที

/* ─── Extra2 (Orbit-Aim mode 3) ────────────────────────────── */
#define E2_ENTRY_Y 155      // ballPosY threshold: chase → orbit
#define E2_AIM_OMEGA 30.f   // omega magnitude ขณะ orbit
#define E2_AIM_STRAFE 35.f  // lateral speed ขณะ orbit
#define E2_GOAL_DEAD 18     // px deadband goalPosX ก่อน "aligned"
#define E2_ALIGN_CNT 4      // consecutive aligned frames ก่อน push
#define E2_AIM_MS 1800      // timeout orbit phase (ms) → force shoot
#define E2_PUSH_SPD 80.f    // push speed ก่อน kick
#define E2_PUSH_MS 220      // push duration (ms)

/* ─── Ronaldo Orbit-Aim ──────────────────────────────────── */
#define RON_AIM_Y 165   // target ballPosY during orbit (distance hold)
#define RON_Y_KP 0.40f  // P gain for Y distance control during orbit

/* ─── Misc tuning constants (replaces magic numbers) ─────── */
#define CHASE_SPD_SCALE 0.825f     // speed = dist * this 0.75f
#define PENALTY_SLIDE_KP 0.9f      // slide speed = |err| * this
#define PENALTY_SLIDE_MAX 80.f     // max penalty slide speed
#define PENALTY_PUSH_SPD 85.f      // push forward speed in penalty
#define RETREAT_SPD 90.f           // emergency retreat speed
#define RETREAT_GOAL_Y_THRESH 175  // goalPosY เกินนี้ = ใกล้โกลเกิน → ถอยหลังยิง
#define RETREAT_AFTER_KICK_MS 750  // ระยะเวลาถอยหลังยิง (ms)

/* ─── Rot-Fling tuning ───────────────────────────────── */
#define RFLING_ROT_KP 0.275f    // ROT phase: centering Kp
#define RFLING_FLI_KP 1.0f      // ROT phase: approach Kp
#define RFLING_SP_ROT 160       // target ball X (center = 160 px)
#define RFLING_SP_FLI 200       // target ball Y (kicker zone)
#define RFLING_ROT_GAP 10       // centering deadband (px)
#define RFLING_FLI_GAP 10       // forward deadband (px)
#define RFLING_CURVE_SPD 55.f   // FLING phase: curve speed
#define RFLING_CURVE_RAD 15.f   // FLING phase: rotation magnitude
#define RFLING_YAW_TOL 10.f     // FLING phase: Yaw alignment (°)
#define RFLING_ALIGN_CNT 5      // consecutive aligned frames before goal()
#define RFLING_TIMEOUT_MS 2500  // overall timeout → fallback to doChase 1500

/* ─── Global: IMU ─────────────────────────────────────────── */
float pvYaw = 0.f, pvPitch = 0.f, pvRoll = 0.f;
float SetPoint = 0.f;
volatile uint8_t rxCnt = 0;
volatile uint8_t rxBuf[8];

// Yaw error wrapped ±180° relative to SetPoint
#define YawErr (atan2f(sinf((pvYaw - SetPoint) * DEG2RAD), \
                       cosf((pvYaw - SetPoint) * DEG2RAD)) \
                * RAD2DEG)

/* ─── Global: Vision ──────────────────────────────────────── */
int ballPosX = 160, ballPosY = 0;
int goalPosX = 160, goalPosY = 0;
bool ballSeen = false, goalSeen = false;

/* ─── Global: PID state ───────────────────────────────────── */
float x_pErr = 0.f, x_i = 0.f;  // heading-hold

/* ─── Global: game state ──────────────────────────────────── */
// mode: 1=Attack 2=Extra1 3=Extra2 4=Defend1 5=Defend2
//       6=Onana  7=Ronaldo 8=SenChk 9=BallRadar 10=GoalRadar 11=TestKick
int mode = 1;
int cg = SIG_GOAL_YEL;
int kickReady = 1;

/* ─── Global: Defend2 state ───────────────────────────────── */
bool gDefendPhase = false;
uint32_t gDefendUntilMs = 0UL;
