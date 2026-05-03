/* ══════════════════════════════════════════════════════
   §9  VISION — อ่าน HuskyLens
   ══════════════════════════════════════════════════════ */

void updateVision() {
  ballSeen = goalSeen = false;
  if (!huskylens.updateBlocks()) return;

  if (huskylens.blockSize[SIG_BALL] > 0) {
    ballSeen  = true;
    ballPosX  = constrain(huskylens.blockInfo[SIG_BALL][0].x, 0, 320);
    ballPosY  = constrain(huskylens.blockInfo[SIG_BALL][0].y, 0, 240);
  }
  if (huskylens.blockSize[cg] > 0) {
    goalSeen  = true;
    goalPosX  = constrain(huskylens.blockInfo[cg][0].x, 0, 320);
    goalPosY  = constrain(huskylens.blockInfo[cg][0].y, 0, 240);
  }
}