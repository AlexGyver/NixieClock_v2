void backlBrightTick() {
  if (BACKL_MODE == 0 && backlBrightTimer.isReady()) {
    if (backlBrightDirection) {
      if (!backlBrightFlag) {
        backlBrightFlag = true;
        backlBrightTimer.setInterval((float)BACKL_STEP / backlMaxBright / 2 * BACKL_TIME);
      }
      backlBrightCounter += BACKL_STEP;
      if (backlBrightCounter >= backlMaxBright) {
        backlBrightDirection = false;
        backlBrightCounter = backlMaxBright;
      }
    } else {
      backlBrightCounter -= BACKL_STEP;
      if (backlBrightCounter <= 0) {
        backlBrightDirection = true;
        backlBrightCounter = 0;
        backlBrightTimer.setInterval(BACKL_PAUSE);
        backlBrightFlag = false;
      }
    }
    setPWM(BACKL, getPWM_CRT(backlBrightCounter));
  }
}

void dotBrightTick() {
  if (dotBrightFlag && dotBrightTimer.isReady()) {
    if (dotBrightDirection) {
      dotBrightCounter += dotBrightStep;
      if (dotBrightCounter >= dotMaxBright) {
        dotBrightDirection = false;
        dotBrightCounter = dotMaxBright;
      }
    } else {
      dotBrightCounter -= dotBrightStep;
      if (dotBrightCounter <= 0) {
        dotBrightDirection = true;
        dotBrightFlag = false;
        dotBrightCounter = 0;
      }
    }
    setPWM(DOT, getPWM_CRT(dotBrightCounter));
  }
}

void changeBright() {
#if (NIGHT_LIGHT == 1)
  // установка яркости всех светилок от времени суток
  if ( (hrs >= NIGHT_START && hrs <= 23)
       || (hrs >= 0 && hrs < NIGHT_END) ) {
    indiMaxBright = INDI_BRIGHT_N;
    dotMaxBright = DOT_BRIGHT_N;
    backlMaxBright = BACKL_BRIGHT_N;
  } else {
    indiMaxBright = INDI_BRIGHT;
    dotMaxBright = DOT_BRIGHT;
    backlMaxBright = BACKL_BRIGHT;
  }
  for (byte i = 0; i < 4; i++) {
    indiDimm[i] = indiMaxBright;
  }

  dotBrightStep = ceil((float)dotMaxBright * 2 / DOT_TIME * DOT_TIMER);
  if (dotBrightStep == 0) dotBrightStep = 1;

  backlBrightTimer.setInterval((float)BACKL_STEP / backlMaxBright / 2 * BACKL_TIME);
  indiBrightCounter = indiMaxBright;

  //change PWM to apply backlMaxBright in case of maximum bright mode
  if (BACKL_MODE == 1) setPWM(BACKL, backlMaxBright);
#endif
}
