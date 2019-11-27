/*
  Скетч к проекту "Часы на ГРИ версия 2"
  Страница проекта (схемы, описания): https://alexgyver.ru/nixieclock_v2/
  Исходники на GitHub: https://github.com/AlexGyver/NixieClock_v2
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  https://AlexGyver.ru/
*/
/*
  Версия 1.2:
  - Чуть переделан блок кода, отвечающий за распиновку ламп
  
  Версия 1.3:
  - Скетч разбит на вкладки
  - Добавлено управление кнопками!
    - Левая кнопка - выбор, остальные "больше" и "меньше"
    - Удержать "выбор" - настройка времени
    - Клик по "выбору" - смена настройки часы/минуты
    - Клик "больше" и "меньше" - изменение времени
    - Удержать "выбор" - возврат к режиму часов
    
  Версия 1.4:
  - Ещё управление кнопками в режиме часов:
    - Центральная кнопка переключает режимы подсветки ламп
      - Дыхание
      - Постоянное свечение
      - Отключена
    - Правая кнопка переключает режимы перелистывания цифр
      - Без эффекта
      - Плавное угасание
      - Перемотка по порядку числа
      - Перемотка по катодам
*/

// ************************** НАСТРОЙКИ **************************
#define BOARD_TYPE 2
// тип платы часов:
// 0 - IN-12 turned (индикаторы стоят правильно)
// 1 - IN-12 (индикаторы перевёрнуты)
// 2 - IN-14 (обычная и neon dot)
// 3 другие индикаторы

byte FLIP_EFFECT = 1;   // меняется кнопками
// эффекты перелистывания часов:
// 0 - нет эффекта
// 1 - плавное угасание и появление (рекомендуемая скорость: 100-150)
// 2 - перемотка по порядку числа (рекомендуемая скорость: 50-80)
// 3 - перемотка по порядку катодов в лампе (рекомендуемая скорость: 30-50)

byte BACKL_MODE = 0;    // меняется кнопками
// эффекты подсветки:
// 0 - дыхание
// 1 - постоянный свет
// 2 - выключена

#define FLIP_SPEED 40       // скорость эффекта, мс

// -------- яркость --------
#define NIGHT_LIGHT 1       // менять яркость от времени суток (1 вкл, 0 выкл)
#define NIGHT_START 23      // час перехода на ночную подсветку (BRIGHT_N)
#define NIGHT_END 7         // час перехода на дневную подсветку (BRIGHT)

#define INDI_BRIGHT 23      // яркость цифр дневная (0 - 24) !на 24 могут быть фантомные цифры!
#define INDI_BRIGHT_N 3     // яркость ночная (0 - 24)

#define DOT_BRIGHT 10       // яркость точки дневная (0 - 255)
#define DOT_BRIGHT_N 3      // яркость точки ночная (0 - 255)

#define BACKL_BRIGHT 180    // яркость подсветки ламп дневная (0 - 255)
#define BACKL_BRIGHT_N 20   // яркость подсветки ламп ночная (0 - 255)
#define BACKL_PAUSE 600     // пазуа "темноты" между вспышками подсветки, мс

// -------- глюки --------
#define GLITCH_ALLOWED 1    // 1 - включить, 0 - выключить глюки
#define GLITCH_MIN 30       // минимальное время между глюками, с
#define GLITCH_MAX 120      // максимальное время между глюками, с

// -------- мигание --------
#define DOT_TIME 500        // время мигания точки, мс
#define DOT_TIMER 20        // шаг яркости точки, мс

#define BACKL_STEP 2        // шаг мигания подсветки
#define BACKL_TIME 5000     // период подсветки, мс

// -------- будильник --------
#define ALM_TIMEOUT 30      // таймаут будильника
#define FREQ 900            // частота писка будильника

// --------- другое --------
#define BURN_TIME 1         // период обхода в режиме очистки, мс

// *********************** ДЛЯ РАЗРАБОТЧИКОВ ***********************
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

// распиновка ламп
#if (BOARD_TYPE == 0)
byte digitMask[] = {7, 3, 6, 4, 1, 9, 8, 0, 5, 2};   // маска дешифратора платы in12_turned (цифры нормальные)
byte opts[] = {KEY0, KEY1, KEY2, KEY3};              // порядок индикаторов слева направо
byte cathodeMask[] = {1, 6, 2, 7, 5, 0, 4, 9, 8, 3}; // порядок катодов in12

#elif (BOARD_TYPE == 1)
byte digitMask[] = {2, 8, 1, 9, 6, 4, 3, 5, 0, 7};   // маска дешифратора платы in12 (цифры вверх ногами)
byte opts[] = {KEY3, KEY2, KEY1, KEY0};              // порядок индикаторов справа налево (для IN-12 turned) и ин-14
byte cathodeMask[] = {1, 6, 2, 7, 5, 0, 4, 9, 8, 3}; // порядок катодов in12

#elif (BOARD_TYPE == 2)
byte digitMask[] = {9, 8, 0, 5, 4, 7, 3, 6, 2, 1};   // маска дешифратора платы in14
byte opts[] = {KEY3, KEY2, KEY1, KEY0};              // порядок индикаторов справа налево (для IN-12 turned) и ин-14
byte cathodeMask[] = {1, 0, 2, 9, 3, 8, 4, 7, 5, 6}; // порядок катодов in14

#elif (BOARD_TYPE == 3)
byte digitMask[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};   // тут вводим свой порядок пинов
byte opts[] = {KEY0, KEY1, KEY2, KEY3};              // свой порядок индикаторов
byte cathodeMask[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // и свой порядок катодов

#endif

// библиотеки
#include <GyverHacks.h>
//#include <GyverTimer.h>
#include <GyverButton.h>
#include <Wire.h>
#include <RTClib.h>
#include "timer2Minim.h"

RTC_DS3231 rtc;

// таймеры
timerMinim dotTimer(500);                // полсекундный таймер для часов
timerMinim dotBrightTimer(DOT_TIMER);    // таймер шага яркости точки
timerMinim backlBrightTimer(30);         // таймер шага яркости подсветки
timerMinim almTimer((long)ALM_TIMEOUT * 1000);
timerMinim flipTimer(FLIP_SPEED);
timerMinim glitchTimer(1000);
timerMinim blinkTimer(500);

// кнопки
GButton btnSet(BTN1, HIGH_PULL, NORM_OPEN);
GButton btnL(BTN2, HIGH_PULL, NORM_OPEN);
GButton btnR(BTN3, HIGH_PULL, NORM_OPEN);

// переменные
volatile int8_t indiDimm[4];      // величина диммирования (0-24)
volatile int8_t indiCounter[4];   // счётчик каждого индикатора (0-24)
volatile int8_t indiDigits[4];    // цифры, которые должны показать индикаторы (0-10)
volatile int8_t curIndi;          // текущий индикатор (0-3)

boolean dotFlag;
int8_t hrs, mins, secs;
int8_t alm_hrs, alm_mins;
int8_t mode = 0;    // 0 часы, 1 температура, 2 настройка будильника, 3 настройка часов, 4 аларм
boolean changeFlag;
boolean blinkFlag;
byte indiMaxBright = INDI_BRIGHT, dotMaxBright = DOT_BRIGHT, backlMaxBright = BACKL_BRIGHT;
boolean alm_flag;
boolean dotBrightFlag, dotBrightDirection, backlBrightFlag, backlBrightDirection, indiBrightDirection;
int dotBrightCounter, backlBrightCounter, indiBrightCounter;
byte dotBrightStep;
boolean newTimeFlag;
boolean flipIndics[4];
byte newTime[4];
boolean flipInit;
byte startCathode[4], endCathode[4];
byte glitchCounter, glitchMax, glitchIndic;
boolean glitchFlag, indiState;
byte curMode = 0;
boolean currentDigit = false;
int8_t changeHrs, changeMins;
boolean lampState = false;
boolean anodeStates[] = {1, 1, 1, 1};

void setDig(byte digit) {
  digit = digitMask[digit];
  setPin(DECODER3, bitRead(digit, 0));
  setPin(DECODER1, bitRead(digit, 1));
  setPin(DECODER0, bitRead(digit, 2));
  setPin(DECODER2, bitRead(digit, 3));
}

void setup() {
  //Serial.begin(9600);
  randomSeed(analogRead(6) + analogRead(7));

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

  /*if (EEPROM.read(100) != 66) {   // проверка на первый запуск. 66 от балды
    EEPROM.write(100, 66);
    EEPROM.write(0, 0);     // часы будильника
    EEPROM.write(1, 0);     // минуты будильника
    }
    alm_hrs = EEPROM.read(0);
    alm_mins = EEPROM.read(1);*/

  sendTime(hrs, mins);  // отправить время на индикаторы
  changeBright();       // изменить яркость согласно времени суток

  for (byte i = 0; i < 4; i++)
    indiDimm[i] = indiMaxBright;

  dotBrightStep = ceil((float)dotMaxBright * 2 / DOT_TIME * DOT_TIMER);
  if (dotBrightStep == 0) dotBrightStep = 1;

  backlBrightTimer.setInterval((float)BACKL_STEP / backlMaxBright / 2 * BACKL_TIME);

  //almTimer.stop();
  glitchTimer.setInterval(random(GLITCH_MIN * 1000L, GLITCH_MAX * 1000L));
  indiBrightCounter = indiMaxBright;
}

void loop() {
  if (dotTimer.isReady()) calculateTime();      // каждые 500 мс пересчёт и отправка времени
  if (newTimeFlag && curMode == 0) flipTick();  // перелистывание цифр
  dotBrightTick();                              // плавное мигание точки
  backlBrightTick();                            // плавное мигание подсветки ламп
  if (mode == 0) glitchTick();                  // глюки
  buttonsTick();                                // кнопки
  settingsTick();                               // настройки
}

void burnIndicators() {
  for (byte d = 0; d < 10; d++) {
    for (byte i = 0; i < 4; i++) {
      indiDigits[i]--;
      if (indiDigits[i] < 0) indiDigits[i] = 9;
    }
    delay(BURN_TIME);
  }
}

/*
  ард ног ном
  А0  7   4
  А1  6   2
  А2  4   8
  А3  3   1
*/
