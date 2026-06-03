/* ══════════════════════════════════════════════════════
   D_vision.ino — HuskyLens color-block reader
   ══════════════════════════════════════════════════════ */

static uint8_t visionFails = 0;

void updateVision() {
  ballSeen = goalSeen = false;
  if (!huskylens.updateBlocks()) {
    if (++visionFails >= 3) {
      ballSeen = false;
      goalSeen = false;
    }
    return;
  }
  visionFails = 0;

  // if (huskylens.blockSize[SIG_BALL] == 1) {  // > 0
  //   ballSeen = true;
  //   ballPosX = constrain((int)huskylens.blockInfo[SIG_BALL][0].x, 0, 320);
  //   ballPosY = constrain((int)huskylens.blockInfo[SIG_BALL][0].y, 0, 240);
  // }
  if (huskylens.blockSize[SIG_BALL] > 0) {
    ballSeen = true;
    // เมื่อเห็นหลายลูก: เลือกลูกที่มี area (w*h) มากสุด = ใกล้กล้องที่สุด
    int best = 0;
    int bestArea = huskylens.blockInfo[SIG_BALL][0].width * huskylens.blockInfo[SIG_BALL][0].height;
    for (int i = 1; i < huskylens.blockSize[SIG_BALL]; i++) {
      int area = huskylens.blockInfo[SIG_BALL][i].width * huskylens.blockInfo[SIG_BALL][i].height;
      if (area > bestArea) {
        bestArea = area;
        best = i;
      }
    }
    ballPosX = constrain((int)huskylens.blockInfo[SIG_BALL][best].x, 0, 320);
    ballPosY = constrain((int)huskylens.blockInfo[SIG_BALL][best].y, 0, 240);
  }
  if (huskylens.blockSize[cg] > 0) {
    goalSeen = true;
    goalPosX = constrain((int)huskylens.blockInfo[cg][0].x, 0, 320);
    goalPosY = constrain((int)huskylens.blockInfo[cg][0].y, 0, 240);
  }
}
