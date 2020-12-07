void settingsTick() {
  if (curMode == 1) 
  {
    if (blinkTimer.isReady()) 
    {
      if(currentDigit==0 || currentDigit==1) 
      {sendTime(changeHrs, changeMins);}  
      else {sendTime(changeHrs2, changeMins2);}
      lampState = !lampState;
      if (lampState) {
        anodeStates[0] = 1;
        anodeStates[1] = 1;
        anodeStates[2] = 1;
        anodeStates[3] = 1;
      } else 
      {
        if (currentDigit==0||currentDigit==2) 
        {
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

void buttonsTick() 
{
  btnSet.tick();
  btnL.tick();
  btnR.tick();
    
  if (curMode == 1) 
  {
    if (btnR.isClick()) 
    {
      if (currentDigit==0) 
      {
        changeHrs++;
        if (changeHrs > 23) changeHrs = 0;
      }
      if (currentDigit==2) 
      {
        changeHrs2++;
        if (changeHrs2 > 23) changeHrs2 = 0;
      }
      if (currentDigit==1)
       {
          changeMins++;
          if (changeMins > 59) 
          {
            changeMins = 0;
            changeHrs++;
          }
            if (changeHrs > 23) changeHrs = 0;
        }
        if (currentDigit==3)
       {
          changeMins2++;
          if (changeMins2 > 59) 
          {
            changeMins2 = 0;
            changeHrs2++;
          }
            if (changeHrs2 > 23) changeHrs2 = 0;
        }
     }      
     if(currentDigit==0 || currentDigit==1) 
     {sendTime(changeHrs, changeMins);}  
     else {sendTime(changeHrs2, changeMins2);}
       
    
    if (btnL.isClick()) 
    {
      if (currentDigit==0) 
      {
        changeHrs--;
        if (changeHrs < 0) changeHrs = 23;
      }
      if (currentDigit==2) 
      {
        changeHrs2--;
        if (changeHrs2 < 0) changeHrs2 = 23;
      }
        if (currentDigit==1)
      {
        changeMins--;
        if (changeMins < 0) {
          changeMins = 59;
          changeHrs--;
          if (changeHrs < 0) changeHrs = 23;
        }
      }
      if (currentDigit==3)
      {
        changeMins2--;
        if (changeMins2 < 0) {
          changeMins2 = 59;
          changeHrs2--;
          if (changeHrs2 < 0) changeHrs2 = 23;
        }
      }
      if(currentDigit==0 || currentDigit==1) 
      {sendTime(changeHrs, changeMins);}  
      else {sendTime(changeHrs2, changeMins2);}     
     }
  }
  
  
  if (curMode == 0) 
{
  
    // переключение эффектов цифр
    if (btnR.isClick()) {
      if (++FLIP_EFFECT >= FLIP_EFFECT_NUM) FLIP_EFFECT = 0;
      EEPROM.put(0, FLIP_EFFECT);
      flipTimer.setInterval(FLIP_SPEED[FLIP_EFFECT]);
      for (byte i = 0; i < 4; i++) {
        indiDimm[i] = indiMaxBright;
        anodeStates[i] = 1;
      }
      // показать эффект
      newTimeFlag = true;
      for (byte i = 0; i < 4; i++) indiDigits[i] = FLIP_EFFECT;
    }

    // переключение эффектов подсветки
    if (btnL.isClick()) 
    {
      if (++BACKL_MODE >= 3) BACKL_MODE = 0;
      EEPROM.put(1, BACKL_MODE);
      if (BACKL_MODE == 1) {
        setPWM(BACKL, backlMaxBright);
      } else if (BACKL_MODE == 2) {
        digitalWrite(BACKL, 0);
      }
    }

    // переключение глюков
    if (btnL.isHolded()) 
    {
      GLITCH_ALLOWED = !GLITCH_ALLOWED;      
      EEPROM.put(2, GLITCH_ALLOWED);
        if(GLITCH_ALLOWED)
        {
          setPin(PIEZO, 1);
          delay(10);
          setPin(PIEZO, 0);
          delay(20);
          setPin(PIEZO, 1);
          delay(10);
          setPin(PIEZO, 0);
        }
        else
        {
          setPin(PIEZO, 1);
          delay(20);
          setPin(PIEZO, 0);
        }
    }
}

  if (btnSet.isHolded()) 
  {    
    anodeStates[0] = 1;
    anodeStates[1] = 1;
    anodeStates[2] = 1;
    anodeStates[3] = 1;
    currentDigit = 0;    
    curMode = !curMode;    
    switch (curMode) 
    {//Запись после настройки
        case 0:
        hrs = changeHrs;
        mins = changeMins;
        secs = 0;
        alm_hrs = changeHrs2;
        alm_mins = changeMins2;
        EEPROM.put(4, alm_hrs);     // часы будильника
        EEPROM.put(5, alm_mins);     // минуты будильника
        rtc.adjust(DateTime(2019, 12, 05, hrs, mins, 0));
        changeBright();
        break;        
        case 1:
        changeHrs2 = alm_hrs;
        changeMins2 = alm_mins;
        changeHrs = hrs;
        changeMins = mins;
        break;        

    }
  }

  if (btnSet.isClick()) 
  { 
    if(curMode == 0 && alm_flag==0) //Переключатель будильника
    {
      ALARM_POWER=!ALARM_POWER; 
      EEPROM.put(3, ALARM_POWER);
      if(ALARM_POWER)  
      {        
        setPin(PIEZO, 1);
        sendTime(alm_hrs, alm_mins); 
        delay(200);          
        setPin(PIEZO, 0); 
        delay(100);
        setPin(PIEZO, 1);       
        delay(200);          
        setPin(PIEZO, 0);   
        newTimeFlag=1;               
      } 
      else
      {
        setPin(PIEZO, 1);        
        delay(500);          
        setPin(PIEZO, 0);           
      }
    }  
    if(curMode == 0 && alm_flag)
    {
       alm_flag=0;
       setPin(PIEZO, 0);
       lampState = 1;
       anodeStates[0] = 1;
       anodeStates[1] = 1;
       anodeStates[2] = 1;
       anodeStates[3] = 1; 
    }  
    
   if (curMode == 1) currentDigit ++; // настройка времени 
    if(currentDigit>=4)
    {
      currentDigit=0; 
    }    
  }
}
