/* ══════════════════════════════════════════════════════
   D_vision.ino — HuskyLens color-block reader
   ══════════════════════════════════════════════════════ */

void updateVision() {
  ballSeen = goalSeen = false;
  if (!huskylens.updateBlocks()) return;

  if (huskylens.blockSize[SIG_BALL] > 0) {
    ballSeen = true;
    ballPosX = constrain((int)huskylens.blockInfo[SIG_BALL][0].x, 0, 320);
    ballPosY = constrain((int)huskylens.blockInfo[SIG_BALL][0].y, 0, 240);
  }
  if (huskylens.blockSize[cg] > 0) {
    goalSeen = true;
    goalPosX = constrain((int)huskylens.blockInfo[cg][0].x, 0, 320);
    goalPosY = constrain((int)huskylens.blockInfo[cg][0].y, 0, 240);
  }
}
