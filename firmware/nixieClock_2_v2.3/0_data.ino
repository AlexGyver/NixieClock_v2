// библиотеки
#include "timer2Minim.h"
#include <GyverButton.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>

RTC_DS3231 rtc;

// таймеры
timerMinim dotTimer(500);                // полсекундный таймер для часов
timerMinim dotBrightTimer(DOT_TIMER);    // таймер шага яркости точки
timerMinim backlBrightTimer(30);         // таймер шага яркости подсветки
timerMinim almTimer((long)ALM_TIMEOUT * 1000);
timerMinim flipTimer(FLIP_SPEED[FLIP_EFFECT]);
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
byte currentLamp, flipEffectStages;
bool trainLeaving;

const uint8_t CRTgamma[256] PROGMEM = {
  0,    0,    1,    1,    1,    1,    1,    1,
  1,    1,    1,    1,    1,    1,    1,    1,
  2,    2,    2,    2,    2,    2,    2,    2,
  3,    3,    3,    3,    3,    3,    4,    4,
  4,    4,    4,    5,    5,    5,    5,    6,
  6,    6,    7,    7,    7,    8,    8,    8,
  9,    9,    9,    10,   10,   10,   11,   11,
  12,   12,   12,   13,   13,   14,   14,   15,
  15,   16,   16,   17,   17,   18,   18,   19,
  19,   20,   20,   21,   22,   22,   23,   23,
  24,   25,   25,   26,   26,   27,   28,   28,
  29,   30,   30,   31,   32,   33,   33,   34,
  35,   35,   36,   37,   38,   39,   39,   40,
  41,   42,   43,   43,   44,   45,   46,   47,
  48,   49,   49,   50,   51,   52,   53,   54,
  55,   56,   57,   58,   59,   60,   61,   62,
  63,   64,   65,   66,   67,   68,   69,   70,
  71,   72,   73,   74,   75,   76,   77,   79,
  80,   81,   82,   83,   84,   85,   87,   88,
  89,   90,   91,   93,   94,   95,   96,   98,
  99,   100,  101,  103,  104,  105,  107,  108,
  109,  110,  112,  113,  115,  116,  117,  119,
  120,  121,  123,  124,  126,  127,  129,  130,
  131,  133,  134,  136,  137,  139,  140,  142,
  143,  145,  146,  148,  149,  151,  153,  154,
  156,  157,  159,  161,  162,  164,  165,  167,
  169,  170,  172,  174,  175,  177,  179,  180,
  182,  184,  186,  187,  189,  191,  193,  194,
  196,  198,  200,  202,  203,  205,  207,  209,
  211,  213,  214,  216,  218,  220,  222,  224,
  226,  228,  230,  232,  233,  235,  237,  239,
  241,  243,  245,  247,  249,  251,  253,  255,
};

byte getPWM_CRT(byte val) {
  return pgm_read_byte(&(CRTgamma[val]));
}

// быстрый digitalWrite
void setPin(uint8_t pin, uint8_t x) {
  switch (pin) { // откл pwm
    case 3:  // 2B
      bitClear(TCCR2A, COM2B1);
      break;
    case 5: // 0B
      bitClear(TCCR0A, COM0B1);
      break;
    case 6: // 0A
      bitClear(TCCR0A, COM0A1);
      break;
    case 9: // 1A
      bitClear(TCCR1A, COM1A1);
      break;
    case 10: // 1B
      bitClear(TCCR1A, COM1B1);
      break;
    case 11: // 2A
      bitClear(TCCR2A, COM2A1);
      break;
  }

  if (pin < 8) bitWrite(PORTD, pin, x);
  else if (pin < 14) bitWrite(PORTB, (pin - 8), x);
  else if (pin < 20) bitWrite(PORTC, (pin - 14), x);
  else return;
}

// быстрый analogWrite
void setPWM(uint8_t pin, uint16_t duty) {
  if (duty == 0) setPin(pin, LOW);
  else {
    switch (pin) {
      case 5:
        bitSet(TCCR0A, COM0B1);
        OCR0B = duty;
        break;
      case 6:
        bitSet(TCCR0A, COM0A1);
        OCR0A = duty;
        break;
      case 10:
        bitSet(TCCR1A, COM1B1);
        OCR1B = duty;
        break;
      case 9:
        bitSet(TCCR1A, COM1A1);
        OCR1A = duty;
        break;
      case 3:
        bitSet(TCCR2A, COM2B1);
        OCR2B = duty;
        break;
      case 11:
        bitSet(TCCR2A, COM2A1);
        OCR2A = duty;
        break;
      default:
        break;
    }
  }
}
