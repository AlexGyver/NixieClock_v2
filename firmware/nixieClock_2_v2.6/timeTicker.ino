volatile int sinCount = 0;
void calculateTime() {
  dotFlag = !dotFlag;
  if (dotFlag)  {
    dotBrightFlag = true;
	#if (DOT_TICK_STYLE == 0)
		dotBrightDirection = true;
		dotBrightCounter = 0;
	#elif (DOT_TICK_STYLE == 1)
		dotBrightDirection = !dotBrightDirection;
    #endif
    secs++;       
    if (secs > 59) 
    {
      newTimeFlag = true;   // флаг что нужно поменять время
      secs = 0;
      mins++;  
      sinCount++;  

        if (sinCount >= 60)   {            // каждые 1 мин
        sinCount = 0;
        DateTime now = rtc.now();       // синхронизация с RTC
        secs = now.second();
        mins = now.minute();
        hrs = now.hour();
      }    
      if (mins % BURN_PERIOD == 0) burnIndicators();     // Прогон для антиотравления индикаторов      
    }
    if (mins > 59) {
      mins = 0;
      hrs++;
      if (hrs > 23) hrs = 0;
      changeBright();
    }
    if (newTimeFlag) setNewTime();         // обновляем массив времени    
  }   
}
