/*
  Скетч к проекту "Часы на ГРИ версия 2"
  Страница проекта (схемы, описания): https://alexgyver.ru/nixieclock_v2/
  Исходники на GitHub: https://github.com/AlexGyver/NixieClock_v2
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  https://AlexGyver.ru/
*/

// ************************** НАСТРОЙКИ **************************
#define BOARD_TYPE 2
// тип платы часов:
// 0 - IN-12 turned
// 1 - IN-12
// 2 - IN-14 (обычная и neon dot)
// 3 другие индикаторы

#define INDI_BRIGHT 24      // яркость цифр дневная (0 - 24) !на 24 могут быть фантомные цифры!
#define INDI_BRIGHT_N 2     // яркость ночная (0 - 24)

#define DOT_BRIGHT 100      // яркость точки дневная (0 - 255)
#define DOT_BRIGHT_N 10     // яркость точки ночная (0 - 255)

#define BACKL_BRIGHT 100    // яркость подсветки ламп дневная (0 - 255)
#define BACKL_BRIGHT_N 10   // яркость подсветки ламп ночная (0 - 255)

#define NIGHT_LIGHT 0       // менять яркость от времени суток (1 вкл, 0 выкл)
#define NIGHT_START 23      // час перехода на ночную подсветку (BRIGHT_N)
#define NIGHT_END 7         // час перехода на дневную подсветку (BRIGHT)

#define FREQ 900            // частота писка будильника

#define CLOCK_TIME 10       // время (с), которое отображаются часы
#define TEMP_TIME 5         // время (с), которое отображается температура и влажность
#define ALM_TIMEOUT 30      // таймаут будильника

// *********************** ДЛЯ РАЗРАБОТЧИКОВ ***********************
#define DOT_TIME 500        // время мигания точки, мс
#define DOT_TIMER 20        // шаг яркости точки, мс

#define BACKL_STEP 2        // шаг мигания подсветки
#define BACKL_TIME 5000     // период подсветки, мс

#define BURN_TIME 200       // период обхода в режиме очистки

// пины
#define PIEZO 2   // пищалка
#define KEY0 3    // часы
#define KEY1 4    // часы 
#define KEY2 5    // минуты
#define KEY3 6    // минуты
#define BTN1 7    // кнопка 1
#define BTN2 8    // кнопка 2
#define GEN 9     // генератор
#define DOT 10    // точка
#define BACKL 11  // подсветка
#define BTN3 12   // кнопка 3

// дешифратор
#define DECODER0 A0
#define DECODER1 A1
#define DECODER2 A2
#define DECODER3 A3

// библиотеки
#include "GyverHacks.h"
#include "GyverTimer.h"
#include "GyverButton.h"
#include <Wire.h>
#include "RTClib.h"
#include "EEPROMex.h"
RTC_DS3231 rtc;

// таймеры
GTimer_ms dotTimer(500);                // полсекундный таймер для часов
GTimer_ms dotBrightTimer(DOT_TIMER);    // таймер шага яркости точки
GTimer_ms backlBrightTimer(30);         // таймер шага яркости подсветки
GTimer_ms almTimer((long)ALM_TIMEOUT * 1000);
GTimer_ms startupTimer(300);

// распиновка ламп
#if (BOARD_TYPE == 0)
byte digitMask[] = {7, 3, 6, 4, 1, 9, 8, 0, 5, 2}; // маска дешифратора платы in12_turned (цифры нормальные)
#elif (BOARD_TYPE == 1)
byte digitMask[] = {2, 8, 1, 9, 6, 4, 3, 5, 0, 7}; // маска дешифратора платы in12 (цифры вверх ногами)
#elif (BOARD_TYPE == 2)
byte digitMask[] = {9, 8, 0, 5, 4, 7, 3, 6, 2, 1}; // маска дешифратора платы in14
#elif (BOARD_TYPE == 3)
//byte digitMask[] = {}; // тут вводим свой порядок пинов
#endif

#if (BOARD_TYPE == 1 || BOARD_TYPE == 2)
byte opts[] = {KEY3, KEY2, KEY1, KEY0};   // порядок индикаторов справа налево (для IN-12 turned)
#else
byte opts[] = {KEY0, KEY1, KEY2, KEY3};   // порядок индикаторов слева направо
#endif

// переменные
volatile byte indiDimm[4];        // величина диммирования (0-24)
volatile byte indiCounter[4];     // счётчик каждого индикатора (0-24)
volatile byte indiDigits[4];      // цифры, которые должны показать индикаторы (0-10)
volatile byte curIndi;            // текущий индикатор (0-3)

boolean dotFlag;
int8_t hrs, mins, secs;
int8_t alm_hrs, alm_mins;
int8_t mode = 0;    // 0 часы, 1 температура, 2 настройка будильника, 3 настройка часов, 4 аларм
boolean changeFlag;
boolean blinkFlag;
byte indiMaxBright = INDI_BRIGHT, dotMaxBright = DOT_BRIGHT, backlMaxBright = BACKL_BRIGHT;
boolean alm_flag;
boolean startup_flag;
boolean dotBrightFlag, dotBrightDirection, backlBrightDirection;
byte indiBrightCounter;
int dotBrightCounter, backlBrightCounter;
byte dotBrightStep;

void setDig(byte digit) {
  digit = digitMask[digit];
  setPin(DECODER3, bitRead(digit, 0));
  setPin(DECODER1, bitRead(digit, 1));
  setPin(DECODER0, bitRead(digit, 2));
  setPin(DECODER2, bitRead(digit, 3));
}

void setup() {
  Serial.begin(9600);

  // настройка пинов на выход
  pinMode(DECODER0, OUTPUT);
  pinMode(DECODER1, OUTPUT);
  pinMode(DECODER2, OUTPUT);
  pinMode(DECODER3, OUTPUT);
  pinMode(KEY0, OUTPUT);
  pinMode(KEY1, OUTPUT);
  pinMode(KEY2, OUTPUT);
  pinMode(KEY3, OUTPUT);
  pinMode(PIEZO, OUTPUT);
  pinMode(GEN, OUTPUT);
  pinMode(DOT, OUTPUT);
  pinMode(BACKL, OUTPUT);

  // задаем частоту ШИМ на 9 и 10 выводах 31 кГц
  TCCR1B = TCCR1B & 0b11111000 | 1;		// ставим делитель 1

  // включаем ШИМ
  // от скважности зависит напряжение! у 175 вольт при значении 180 и 145 вольт при 120
  setPWM(9, 180);

  // перенастраиваем частоту ШИМ на пинах 3 и 11 на 7.8 кГц и разрешаем прерывания COMPA
  TCCR2B = (TCCR2B & B11111000) | 2;    // делитель 8
  TCCR2A |= (1 << WGM21);   // включить CTC режим для COMPA
  TIMSK2 |= (1 << OCIE2A);  // включить прерывания по совпадению COMPA

  // ---- RTC ----
  rtc.begin();
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  DateTime now = rtc.now();
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();

  if (EEPROM.readByte(100) != 66) {   // проверка на первый запуск. 66 от балды
    EEPROM.writeByte(100, 66);
    EEPROM.writeByte(0, 0);     // часы будильника
    EEPROM.writeByte(1, 0);     // минуты будильника
  }
  alm_hrs = EEPROM.readByte(0);
  alm_mins = EEPROM.readByte(1);

  sendTime();
  changeBright();
  almTimer.stop();
  dotBrightStep = ceil((float)dotMaxBright * 2 / DOT_TIME * DOT_TIMER);
  if (dotBrightStep == 0) dotBrightStep = 1;
  backlBrightTimer.setInterval((float)BACKL_STEP / backlMaxBright / 2 * BACKL_TIME);
}

// динамическая индикация в прерывании таймера 2
ISR(TIMER2_COMPA_vect) {
  indiCounter[curIndi]++;             // счётчик индикатора
  if (indiCounter[curIndi] == indiDimm[curIndi])  // если достигли порога диммирования
    setPin(opts[curIndi], 0);         // выключить текущий индикатор

  if (indiCounter[curIndi] > 25) {    // достигли порога в 25 единиц
    indiCounter[curIndi] = 0;         // сброс счетчика лампы
    if (++curIndi >= 4) curIndi = 0;  // смена лампы закольцованная

    // отправить цифру из массива indiDigits согласно типу лампы
    if (indiDimm[curIndi] > 0) {
      byte thisDig = digitMask[indiDigits[curIndi]];
      setPin(DECODER3, bitRead(thisDig, 0));
      setPin(DECODER1, bitRead(thisDig, 1));
      setPin(DECODER0, bitRead(thisDig, 2));
      setPin(DECODER2, bitRead(thisDig, 3));
      setPin(opts[curIndi], 1);         // включить анод на текущую лампу
    }
  }
}

void loop() {
  // плавное стартовое нарастание яркости
  if (!startup_flag) startupBright();

  if (dotTimer.isReady()) calculateTime();
  dotBrightTick();
  backlBrightTick();

  //buttonsTick();
}

void startupBright() {
  if (startupTimer.isReady()) {
    for (byte i = 0; i < 4; i++)
      indiDimm[i] = indiBrightCounter;

    indiBrightCounter++;
    if (indiBrightCounter >= indiMaxBright) {
      startup_flag = true;
      for (byte i = 0; i < 4; i++)
        indiDimm[i] = indiMaxBright;
    }
  }
}

void backlBrightTick() {
  if (backlBrightTimer.isReady()) {
    if (backlBrightDirection) {
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
      }
    }
    setPWM(BACKL, backlBrightCounter);
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
    setPWM(DOT, dotBrightCounter);
  }
}

void sendTime() {
  indiDigits[0] = (byte)hrs / 10;
  indiDigits[1] = (byte)hrs % 10;

  indiDigits[2] = (byte)mins / 10;
  indiDigits[3] = (byte)mins % 10;
}

void changeBright() {
#if (NIGHT_LIGHT == 1)
  // установка яркости всех светилок от времени суток
  if ( (hrs >= NIGHT_START && hrs <= 23)
       || (hrs >= 0 && hrs <= NIGHT_END) ) {
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

  backlBrightTimer.setPeriod((float)backlBrightStep / backlMaxBright / 2 * BACKL_TIME);
#endif
}

void calculateTime() {
  dotFlag = !dotFlag;
  if (dotFlag) {
    dotBrightFlag = true;
    dotBrightDirection = true;
    dotBrightCounter = 0;
    secs++;
    if (secs > 59) {
      secs = 0;
      mins++;

      if (mins == 1 || mins == 30) {      // каждые полчаса
        //burnIndicators();               // чистим чистим!
        DateTime now = rtc.now();         // синхронизация с RTC
        secs = now.second();
        mins = now.minute();
        hrs = now.hour();
      }

      if (!alm_flag && alm_mins == mins && alm_hrs == hrs && true) {
        mode = 0;
        alm_flag = true;
        almTimer.start();
        almTimer.reset();
      }
    }
    if (mins > 59) {
      mins = 0;
      hrs++;
      if (hrs > 23) hrs = 0;
      changeBright();
    }

    sendTime();

    /*
        if (mode == 0) sendTime();

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
        sendTime();
      }
    }
  */
}


void burnIndicators() {

}

/*
  ард ног ном
  А0  7   4
  А1  6   2
  А2  4   8
  А3  3   1
*/
