//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#include "Display.h"
#include <config.h>
#ifdef PICO_TM1637
using uint = unsigned int;
extern "C" {
#include <PicoTM1637.h>
}
#endif

bool Display::flashShouldClear() {
  if (!Flash)
    return false;
  ++FlashCnt;
  switch (State) {
  case FlashState::Show:
    if (FlashCnt == FlashOnPeriod) {
      FlashCnt = 0;
      State = FlashState::Clear;
      return true;
    }
    return false;
  case FlashState::Clear:
    if (FlashCnt == FlashOffPeriod) {
      FlashCnt = 0;
      State = FlashState::Show;
      return false;
    }
    return true;
  }
  return false;
}

Display::Display(int CLK_PIN, int DIO_PIN) {
#ifdef PICO_TM1637
  TM1637_init(CLK_PIN, DIO_PIN);
  TM1637_clear();
  TM1637_set_brightness(DISPLAY_BRIGHTNESS);
#endif
}


#ifdef PICO_TM1637
static int pow10(int Num) {
  int Res = 1;
  for (int Cnt = 0; Cnt < Num; ++Cnt)
    Res = Res * 10;
  return Res;
}
#endif // PICO_TM1637

void Display::printRaw(int Num) {
#ifdef PICO_TM1637
  if (flashShouldClear()) {
    TM1637_clear();
    return;
  }
  int NumToDisplay = Num * pow10(DISPLAY_SHIFT_LEFT);
  TM1637_set_colon(false);
  TM1637_display(NumToDisplay, 0);
#endif
}

void Display::printMHz(int MHz) {
#ifdef PICO_TM1637
  if (flashShouldClear()) {
    TM1637_clear();
    return;
  }
  printRaw(MHz);
#endif
}

void Display::printKHz(int KHz) {
#ifdef PICO_TM1637
  if (flashShouldClear()) {
    TM1637_clear();
    return;
  }
  int NumToPrint = KHz / 1000;
  printMHz(NumToPrint);
#endif
}

void Display::printTxt(const char *Str) {
#ifdef PICO_TM1637
  if (flashShouldClear()) {
    TM1637_clear();
    return;
  }
  TM1637_display_word((char *)Str, true);
#endif
}
