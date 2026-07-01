

void burnIndicators() {
  for (byte k = 0; k < BURN_LOOPS; k++) {
    for (byte d = 0; d < 10; d++) {
      for (byte i = 0; i < 4; i++) {
        indiDigits[i] = indiDigits[i] ? indiDigits[i] - 1 : 9;  // одной записью, без промежуточного -1 (гонка с ISR)
      }
      delay(BURN_TIME);
    }
  }
}
