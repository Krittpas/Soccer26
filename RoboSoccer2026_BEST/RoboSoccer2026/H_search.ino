/* ══════════════════════════════════════════════════════
   H_search.ino — Ball-lost recovery
   ══════════════════════════════════════════════════════

   Pattern:
   1. Back toward own goal (270°) to open view
   2. Sweep left (spin CCW)
   3. Hold heading
   4. Sweep right (spin CW)
   5. Repeat up to 3 times, then hard retreat
   ══════════════════════════════════════════════════════ */

void doSearch() {
  uint32_t tSearch = millis();

  for (int attempt = 0; attempt < 3; attempt++) {

    // Step 1 — back off
    uint32_t t = millis();
    while (millis() - t < (uint32_t)SEARCH_BACK_MS) {
      updateVision();
      if (ballSeen) return;
      if (LineD()) { avoidLine(); break; }
      headZero(80.f, 270.f);
    }
    stopAll();
    delay(60);

    // Emergency full retreat if search takes > 3 s
    if (millis() - tSearch > 3000) {
      headZero(90.f, 270.f);
      delay(800);
      stopAll();
      break;
    }

    // Step 2 — sweep left
    t = millis();
    while (millis() - t < 250) {
      updateVision();
      if (ballSeen) return;
      wheel(SPD_SEARCH, SPD_SEARCH, SPD_SEARCH);   // CCW spin
    }
    stopAll(); delay(50);
    if (ballSeen) return;

    // Step 3 — hold heading
    t = millis();
    while (millis() - t < 350) {
      updateVision();
      if (ballSeen) return;
      headZero(0.f, 0.f);
    }
    stopAll(); delay(50);
    if (ballSeen) return;

    // Step 4 — sweep right
    t = millis();
    while (millis() - t < 250) {
      updateVision();
      if (ballSeen) return;
      wheel(-SPD_SEARCH, -SPD_SEARCH, -SPD_SEARCH); // CW spin
    }
    stopAll(); delay(50);
    if (ballSeen) return;

    // Step 5 — hold heading again
    t = millis();
    while (millis() - t < 350) {
      updateVision();
      if (ballSeen) return;
      headZero(0.f, 0.f);
    }
    stopAll();
  }
}
