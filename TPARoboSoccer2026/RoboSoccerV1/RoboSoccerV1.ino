#include <POP32.h>
#include <POP32_Huskylens.h>
#include <math.h>

POP32_Huskylens huskylens;

/* ══════════════════════════════════════════════════════
   §1  MATH CONSTANTS
   ══════════════════════════════════════════════════════ */
#define DEG2RAD 0.017453f
#define RAD2DEG 57.29578f
const float SIN30 = 0.5f;
const float COS30 = 0.8660254f;

/* ══════════════════════════════════════════════════════
   §2  HARDWARE PIN / ID MAP
   ══════════════════════════════════════════════════════ */
// HuskyLens signature IDs
#define SIG_BALL 1      // ลูกกอล์ฟสีส้ม
#define SIG_GOAL_YEL 2  // ประตูสีเหลือง
#define SIG_GOAL_BLU 3  // ประตูสีน้ำเงิน

// Line sensors (ZX-03) — analog pins
#define LINE_F 1       // A1 หน้า
#define LINE_L 2       // A2 ซ้าย
#define LINE_BL 3      // A3 หลังซ้าย
#define LINE_BR 4      // A4 หลังขวา
#define LINE_R 5       // A5 ขวา
#define LINE_REF 1600  // Threshold

// Kicker
#define LIM_PIN PA0    // limit switch (analog)
#define LIM_TRIG 500  // ADC threshold

/* ══════════════════════════════════════════════════════
   §3  TUNING PARAMETERS
   ══════════════════════════════════════════════════════ */

// ─── Chase PD (เข้าหาบอล แกน X = หมุน, แกน Y = เดิน) ───
float ROT_KP = 0.50f;  // หมุนหุ่นตาม X ของบอล
float ROT_KD = 0.05f;
#define ROT_DEAD 12  // pixel dead-zone

float FWD_KP = 1.25f;  // วิ่งเข้าหาบอลตาม Y
float FWD_KD = 0.10f;
#define FWD_DEAD 12  // pixel dead-zone
#define FWD_SP 175   // Y setpoint "บอลอยู่ติดหน้า"

// ─── Heading hold PD (รักษาทิศ) ───
#define HEAD_KP 2.3f
#define HEAD_KD 0.5f

// ─── Goal align PD ───
#define GOAL_KP 0.30f
#define GOAL_DEAD 20  // pixel dead-zone

// ─── Speeds ───
#define SPD_SEARCH 55  // หมุนหาบอล
#define SPD_CHASE 90   // วิ่งเข้าบอล (max 100)
#define SPD_GOAL 70    // วิ่งเข้าประตู
#define SPD_PUSH 90    // ดันบอลก่อนยิง
#define SPD_LINE 120   // ถอยหนีเส้น
#define SPD_DEFEND 60  // โหมด defend

// ─── Timers ───
#define GOAL_TIMEOUT_MS 350  // ms รอเห็นประตูก่อนยิงมั่ว
#define PENALTY_PUSH_MS 500  // ms ดันบอลในการยิงจุดโทษ
#define RELOAD_MAX_MS 800    // ms timeout reload

// ─── Alignment ───
#define ALIGN_ZONE 2.5f  // องศา ถือว่าทิศตรงแล้ว

/* ══════════════════════════════════════════════════════
   §4  GLOBAL VARIABLES
   ══════════════════════════════════════════════════════ */

// IMU
float pvYaw = 0.0f, SetPoint = 0.0f;
float pvPitch = 0.0f, pvRoll = 0.0f;
uint8_t rxCnt = 0, rxBuf[8];

// Vision
int ballPosX = 160, ballPosY = 0;
int goalPosX = 160, goalPosY = 0;
bool ballSeen = false, goalSeen = false;

// PID state
float rot_pErr = 0, fwd_pErr = 0, head_pErr = 0;
float x_pError = 0, x_i = 0;
float diffX = 0, diffY = 0;

// Kicker state
int kickState = 1;  // 1 = พร้อมยิง
int reloadSpd = 60;

// Mode / Goal color
int mode = 1;  // 1 = Attack 2 = Defend 3 = Extra 4 = LeftPen 5 = RightPen 6 = Penalty 7 = TestKick
int cg = 2;    // color goal: 2=Yellow, 3=Blue
int lineCounter = 0;

// Motor kinematics temps
float thetaRad, vx, vy, spd1, spd2, spd3;

// Yaw macro (wrap ±180 relative to SetPoint)
#define Yaw (atan2f(sinf((pvYaw - SetPoint) * DEG2RAD), \
                    cosf((pvYaw - SetPoint) * DEG2RAD)) \
             * RAD2DEG)