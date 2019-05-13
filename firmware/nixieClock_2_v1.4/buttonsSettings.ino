void settingsTick() {
  if (curMode == 1) {
    if (blinkTimer.isReady()) {
      sendTime(changeHrs, changeMins);
      lampState = !lampState;
      if (lampState) {
        anodeStates[0] = 1;
        anodeStates[1] = 1;
        anodeStates[2] = 1;
        anodeStates[3] = 1;
      } else {
        if (!currentDigit) {
          anodeStates[0] = 0;
          anodeStates[1] = 0;
        } else {
          anodeStates[2] = 0;
          anodeStates[3] = 0;
        }
      }
    }
  }
}

void buttonsTick() {
  btnSet.tick();
  btnL.tick();
  btnR.tick();

  if (curMode == 1) {
    if (btnR.isClick()) {
      if (!currentDigit) {
        changeHrs++;
        if (changeHrs > 23) changeHrs = 0;
      } else {
        changeMins++;
        if (changeMins > 59) {
          changeMins = 0;
          changeHrs++;
          if (changeHrs > 23) changeHrs = 0;
        }
      }
      sendTime(changeHrs, changeMins);
    }
    if (btnL.isClick()) {
      if (!currentDigit) {
        changeHrs--;
        if (changeHrs < 0) changeHrs = 23;
      } else {
        changeMins--;
        if (changeMins < 0) {
          changeMins = 59;
          changeHrs--;
          if (changeHrs < 0) changeHrs = 23;
        }
      }
      sendTime(changeHrs, changeMins);
    }
  } else if (curMode == 0) {
    if (btnR.isClick()) {
      if (++FLIP_EFFECT >= 4) FLIP_EFFECT = 0;
      for (byte i = 0; i < 4; i++) {
        indiDimm[i] = indiMaxBright;
      }
    }
    if (btnL.isClick()) {
      if (++BACKL_MODE >= 3) BACKL_MODE = 0;
    }
  }

  if (btnSet.isHolded()) {
    anodeStates[0] = 1;
    anodeStates[1] = 1;
    anodeStates[2] = 1;
    anodeStates[3] = 1;
    currentDigit = false;
    if (++curMode >= 2) curMode = 0;
    switch (curMode) {
      case 0:
        hrs = changeHrs;
        mins = changeMins;
        rtc.adjust(DateTime(2019, 12, 05, hrs, mins, 0));
        break;
      case 1:
        changeHrs = hrs;
        changeMins = mins;
        break;
    }
  }
  if (btnSet.isClick()) {
    if (curMode == 1) currentDigit = !currentDigit; // настройка времени
  }
}
