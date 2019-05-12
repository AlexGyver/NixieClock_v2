void flipTick() {
#if (FLIP_EFFECT == 0)
  sendTime(hrs, mins);
  newTimeFlag = false;
#elif (FLIP_EFFECT == 1)
  if (!flipInit) {
    flipInit = true;
    // запоминаем, какие цифры поменялись и будем менять их яркость
    for (byte i = 0; i < 4; i++) {
      if (indiDigits[i] != newTime[i]) flipIndics[i] = true;
      else flipIndics[i] = false;
    }
  }
  if (flipTimer.isReady()) {
    if (!indiBrightDirection) {
      indiBrightCounter--;            // уменьшаем яркость
      if (indiBrightCounter <= 0) {   // если яроксть меньше нуля
        indiBrightDirection = true;   // меняем направление изменения
        indiBrightCounter = 0;        // обнуляем яркость
        sendTime(hrs, mins);          // меняем цифры
      }
    } else {
      indiBrightCounter++;                        // увеличиваем яркость
      if (indiBrightCounter >= indiMaxBright) {   // достигли предела
        indiBrightDirection = false;              // меняем направление
        indiBrightCounter = indiMaxBright;        // устанавливаем максимум
        // выходим из цикла изменения
        flipInit = false;
        newTimeFlag = false;
      }
    }
    for (byte i = 0; i < 4; i++)
      if (flipIndics[i]) indiDimm[i] = indiBrightCounter;   // применяем яркость
  }

#elif (FLIP_EFFECT == 2)
  if (!flipInit) {
    flipInit = true;
    // запоминаем, какие цифры поменялись и будем менять их
    for (byte i = 0; i < 4; i++) {
      if (indiDigits[i] != newTime[i]) flipIndics[i] = true;
      else flipIndics[i] = false;
    }
  }

  if (flipTimer.isReady()) {
    byte flipCounter = 0;
    for (byte i = 0; i < 4; i++) {
      if (flipIndics[i]) {
        indiDigits[i]--;
        if (indiDigits[i] < 0) indiDigits[i] = 9;
        if (indiDigits[i] == newTime[i]) flipIndics[i] = false;
      } else {
        flipCounter++;        // счётчик цифр, которые не надо менять
      }
    }
    if (flipCounter == 4) {   // если ни одну из 4 цифр менять не нужно
      // выходим из цикла изменения
      flipInit = false;
      newTimeFlag = false;
    }
  }

  //byte cathodeMask[] = {1, 0, 2, 9, 3, 8, 4, 7, 5, 6};  // порядок катодов in14
#elif (FLIP_EFFECT == 3)
  if (!flipInit) {
    flipInit = true;
    // запоминаем, какие цифры поменялись и будем менять их
    for (byte i = 0; i < 4; i++) {
      if (indiDigits[i] != newTime[i]) {
        flipIndics[i] = true;
        for (byte c = 0; c < 10; c++) {
          if (cathodeMask[c] == indiDigits[i]) startCathode[i] = c;
          if (cathodeMask[c] == newTime[i]) endCathode[i] = c;
        }
      }
      else flipIndics[i] = false;
    }
  }

  if (flipTimer.isReady()) {
    byte flipCounter = 0;
    for (byte i = 0; i < 4; i++) {
      if (flipIndics[i]) {
        if (startCathode[i] > endCathode[i]) {
          startCathode[i]--;
          indiDigits[i] = cathodeMask[startCathode[i]];
        } else if (startCathode[i] < endCathode[i]) {
          startCathode[i]++;
          indiDigits[i] = cathodeMask[startCathode[i]];
        } else {
          flipIndics[i] = false;
        }
      } else {
        flipCounter++;
      }
    }
    if (flipCounter == 4) {   // если ни одну из 4 цифр менять не нужно
      // выходим из цикла изменения
      flipInit = false;
      newTimeFlag = false;
    }
  }
#endif
}
