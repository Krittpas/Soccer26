# RoboSoccer 2026 — Best Version
### TPA / BenChan Team

---

## โครงสร้างไฟล์

| ไฟล์ | หน้าที่ |
|------|---------|
| `RoboSoccer2026.ino` | constants, globals, includes |
| `A_imu.ino` | ZX-IMU read, autoZero, headingOmega() |
| `B_motion.ino` | wheel(), holonomic(), headZero() |
| `C_kicker.ino` | shoot(), reload(), kick() |
| `D_vision.ino` | updateVision() via HuskyLens |
| `E_line.ino` | S1–S5 sensors, avoidLine() |
| `F_chase.ino` | doChase() — tangent-line algorithm |
| `G_goal.ino` | goal() — align + shoot sequence |
| `H_search.ino` | doSearch() — ball-lost recovery |
| `I_penalty.ino` | penaltyKick() — mandatory round 2+ |
| `Z_setup.ino` | setup menu, loop() |

---

## Quick Start

1. เปิด Arduino IDE → **File > Open** → เลือก folder `RoboSoccer2026_BEST/`
2. ติดตั้ง library: `POP32` และ `POP32_Huskylens`
3. เลือก Board: **POP-32i (STM32F103)**
4. Upload ได้เลย

---

## การปรับ Tuning (สำคัญมาก!)

### 1. LINE_REF — เซ็นเซอร์ขาว
```cpp
#define LINE_REF 1600
```
ทดสอบในสนามจริง: พิมพ์ค่า analogRead(LINE_F) บน OLED
- พื้นเขียว (ไม่มีเส้น) ≈ 800–1200
- เส้นขาว ≈ 1800–2500
ตั้ง LINE_REF ให้อยู่กึ่งกลางระหว่างสองค่า

### 2. FWD_SP — ระยะที่คิดว่า "บอลใกล้พอจะยิง"
```cpp
#define FWD_SP 185   // px ใน camera frame (0–240)
```
ค่ายิ่งมาก = ยิงไกล, ค่ายิ่งน้อย = ต้องเข้าใกล้มากขึ้น
แนะนำ: 175–200

### 3. SURROUND_R — รัศมีวงโคจรรอบบอล
```cpp
#define SURROUND_R 55.0f   // px
```
ค่ายิ่งมาก = วนรอบบอลรัศมีใหญ่ขึ้น (safe แต่ช้า)
ค่ายิ่งน้อย = เข้าใกล้บอลตรงกว่า (เร็วแต่อาจชนบอล)
แนะนำ: 45–65

### 4. HEAD_KP / HEAD_KD — ความแม่นยำทิศ
```cpp
#define HEAD_KP 2.5f
#define HEAD_KD 0.6f
```
ถ้าหุ่น oscillate (สั่นหัว) → ลด KP หรือเพิ่ม KD
ถ้าหุ่นหมุนช้าเกิน → เพิ่ม KP

### 5. GOAL_DEAD — tolerance ก่อนยิง
```cpp
#define GOAL_DEAD 25   // px
```
ค่ายิ่งน้อย = ตั้งตรงประตูแม่นยำมาก (แต่อาจรีรอ)
ค่ายิ่งมาก = ยิงได้เร็วขึ้น (แต่แม่นน้อยลง)

---

## Mode ต่าง ๆ (ปรับด้วย Knob)

| Knob | Mode | ใช้เมื่อ |
|------|------|---------|
| 0 | ATTACK | ผู้เล่นบุก (default) |
| 1 | DEFEND | ผู้รักษาประตู — สไลด์ตามบอล |
| 2 | EXTRA | delay 1 วินาทีก่อนออกสตาร์ท |
| 3 | LEFT-PEN | penalty setup เดินซ้ายก่อน |
| 4 | RGHT-PEN | penalty setup เดินขวาก่อน |
| 5 | PENALTY | ยิงโทษโดยตรง |
| 6 | TEST-KCK | ทดสอบกลไกยิง |

---

## ปุ่มในเมนู Setup

- **SW_A** → Auto-zero IMU (รอจนค่า Yaw < 0.08°)
- **SW_OK** → สลับสีประตู Yellow ↔ Blue
- **SW_B** → เริ่มเกม!

## ปุ่มกลางเกม

- **SW_OK** → Pause + เมนู re-zero / resume

---

## Algorithm: Tangent-line Chase (F_chase.ino)

```
Zone A (บอลอยู่ตรงหน้า): PD slide + forward speed
Zone B (บอลใกล้เกิน):    หักเลี้ยงออกตาม surround radius
Zone C (บอลอยู่ไกล):     tangent angle = asin(r/dist) เข้าทางด้านหลังบอล
```

ผลลัพธ์: หุ่นเข้าหาบอลจาก "ด้านหลัง" เสมอ ไม่ชนบอลออกข้าง

---

## Bug fixes จาก version เก่า

- [x] Division by zero: `rot_w / abs(rot_w)` → safe guard ด้วย lastBallDir
- [x] `getIMU()` return type: `return;` → `return false;` (A_imu.ino)
- [x] Yaw wrap-around: ใช้ `atan2(sin,cos)` macro แล้ว
- [x] Motor clamp: ทุก wheel() ผ่าน `constrain(-100,100)` แล้ว
- [x] IMU integral reset: `x_i` reset ตอน autoZero และ setup()
- [x] OLED blocking: update ทุก 100 ms เท่านั้น
