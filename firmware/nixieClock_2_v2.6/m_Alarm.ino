void Alarm(){
  if (mins==alm_mins && secs==5 && hrs==alm_hrs && ALARM_POWER)//Условие срабатывания будильника
        {             
          alm_flag=1;
          almTimer.start();
        }           
         // Аларм звук и миганиц цифер
        if (alm_flag) 
        {         
          if (blinkTimer.isReady()) 
          {      
            lampState = !lampState;
            if (lampState) {
              anodeStates[0] = 0;
              anodeStates[1] = 0;
              anodeStates[2] = 0;
              anodeStates[3] = 0;
              setPin(PIEZO, 0);
            } else {
                anodeStates[0] = 1;
                anodeStates[1] = 1;
                anodeStates[2] = 1;
                anodeStates[3] = 1;  
               setPin(PIEZO, 1);
            }
           } 
            if (almTimer.isReady()) //Выключить Будильник 
          {
            alm_flag = 0;
            almTimer.stop();            
            setPin(PIEZO, 0);
            lampState = 1;
            anodeStates[0] = 1;
            anodeStates[1] = 1;
            anodeStates[2] = 1;
            anodeStates[3] = 1; 
          }
        }            
      }
