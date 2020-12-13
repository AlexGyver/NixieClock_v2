void burnIndicators() //прогон для антиотравления катодов
{
  for (byte k = 0; k < BURN_LOOPS; k++) {
    for (byte d = 0; d < 10; d++) {
      for (byte i = 0; i < 4; i++) {
        indiDigits[i]--;
        if (indiDigits[i] < 0) indiDigits[i] = 9;
      }
      delay(BURN_TIME);
    }
  }
}

void glitchTick() //Функция глюков
{
  if (!glitchFlag && secs > 7 && secs < 55) {
    if (glitchTimer.isReady()) {
      glitchFlag = true;
      indiState = 0;
      glitchCounter = 0;
      glitchMax = random(2, 6);
      glitchIndic = random(0, 4);
      glitchTimer.setInterval(random(1, 6) * 20);
    }
  } else if (glitchFlag && glitchTimer.isReady()) {
    indiDimm[glitchIndic] = indiState * indiMaxBright;
    indiState = !indiState;
    glitchTimer.setInterval(random(1, 6) * 20);
    glitchCounter++;
    if (glitchCounter > glitchMax) {
      glitchTimer.setInterval(random(GLITCH_MIN * 1000L, GLITCH_MAX * 1000L));
      glitchFlag = false;
      indiDimm[glitchIndic] = indiMaxBright;
    }
  }
}
