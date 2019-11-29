byte minsCount = 0;
void calculateTime() {
  dotFlag = !dotFlag;
  if (dotFlag) {
    dotBrightFlag = true;
    dotBrightDirection = true;
    dotBrightCounter = 0;
    secs++;
    if (secs > 59) {
      newTimeFlag = true;   // флаг что нужно поменять время
      secs = 0;
      mins++;
      minsCount++;

      if (minsCount >= 15) {            // каждые 15 мин
        minsCount = 0;
        DateTime now = rtc.now();       // синхронизация с RTC
        secs = now.second();
        mins = now.minute();
        hrs = now.hour();
      }

      if (mins % BURN_PERIOD == 0) burnIndicators();     // чистим чистим!

      /*if (!alm_flag && alm_mins == mins && alm_hrs == hrs && true) {
        mode = 0;
        alm_flag = true;
        almTimer.start();
        almTimer.reset();
        }*/
    }
    if (mins > 59) {
      mins = 0;
      hrs++;
      if (hrs > 23) hrs = 0;
      changeBright();
    }
    if (newTimeFlag) setNewTime();         // обновляем массив времени

    /*
        if (mode == 0) sendTime(hrs, mins);

        if (alm_flag) {
          if (almTimer.isReady() || true ) {
            alm_flag = false;
            almTimer.stop();
            mode = 0;
            noTone(PIEZO);
          }
        }
    */
  }

  /*
    // мигать на будильнике
    if (alm_flag) {
      if (!dotFlag) {
        noTone(PIEZO);
        for (byte i = 1; i < 7; i++) digitsDraw[i] = 10;
      } else {
        tone(PIEZO, FREQ);
        sendTime(hrs, mins);
      }
    }
  */
}
