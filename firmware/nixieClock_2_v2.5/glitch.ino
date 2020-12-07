void glitchTick() {
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
