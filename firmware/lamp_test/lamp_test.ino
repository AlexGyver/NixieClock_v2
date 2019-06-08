/*
  Проверочный скетч к проекту "Часы на ГРИ v2"
  Страница проекта (схемы, описания): https://alexgyver.ru/nixieclock_v2/
  Исходники на GitHub: https://github.com/AlexGyver/NixieClock_v2
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  https://AlexGyver.ru/
*/

// "минимальный" код для работы часов, можете начать с него разработку своей прошивки
// проверочный код для индикаторов, выводит 0-9 по очереди на каждую лампу

#define DELAY 1000    // задержка между сменой цифр, мс
#define DUTY 180      // скважность ШИМ. От скважности зависит напряжение! у меня 175 вольт при значении 180 и 145 вольт при 120

#define BOARD_TYPE 0
// тип платы часов:
// 0 - IN-12 turned
// 1 - IN-12
// 2 - IN-14 (обычная и neon dot)
// 3 другие индикаторы

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

#define DECODER0 A0
#define DECODER1 A1
#define DECODER2 A2
#define DECODER3 A3

// распиновка ламп
#if (BOARD_TYPE == 0)
byte digitMask[] = {7, 3, 6, 4, 1, 9, 8, 0, 5, 2}; // маска дешифратора платы in12_turned (цифры нормальные)
byte opts[] = {KEY0, KEY1, KEY2, KEY3};            // порядок индикаторов слева направо

#elif (BOARD_TYPE == 1)
byte digitMask[] = {2, 8, 1, 9, 6, 4, 3, 5, 0, 7}; // маска дешифратора платы in12 (цифры вверх ногами)
byte opts[] = {KEY3, KEY2, KEY1, KEY0};            // порядок индикаторов справа налево (для IN-12 turned) и ин-14

#elif (BOARD_TYPE == 2)
byte digitMask[] = {9, 8, 0, 5, 4, 7, 3, 6, 2, 1}; // маска дешифратора платы in14
byte opts[] = {KEY3, KEY2, KEY1, KEY0};            // порядок индикаторов справа налево (для IN-12 turned) и ин-14

#elif (BOARD_TYPE == 3)
byte digitMask[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // тут вводим свой порядок пинов
byte opts[] = {KEY0, KEY1, KEY2, KEY3};            // свой порядок индикаторов
#endif

#include "GyverHacks.h"

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
  setPWM(9, DUTY);
/*
  // перенастраиваем частоту ШИМ на пинах 3 и 11 на 7.8 кГц и разрешаем прерывания по совпадению
  TCCR2B = (TCCR2B & B11111000) | 2;    // делитель 8
  TCCR2A |= (1 << WGM21);   // включить CTC режим для COMPA
  TIMSK2 |= (1 << OCIE2A);  // включить прерывания по совпадению COMPA
*/
  setPWM(BACKL, 1);   // шим на светодиоды подсветки
  setPWM(DOT, 1);     // шим на точку (разделитель часы:минуты)
}

void loop() {
  for (byte i = 0; i < 4; i++) {
    setPin(opts[i], 1);
    // тупо перебираем числа от 0 до 1
    for (byte k = 0; k < 10; k++) {
      setDig(k);
      Serial.println("ind #" + String(i) + " digit: " + String(k));
      delay(DELAY);
    }
    setPin(opts[i], 0);
  }
}
