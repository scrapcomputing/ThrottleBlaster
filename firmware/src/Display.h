//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "ConfigOpts.h"

class Display {
  bool Flash = false;
  static constexpr const int FlashOnPeriod = DisplayFlashOnPeriod;
  static constexpr const int FlashOffPeriod = DisplayFlashOffPeriod;
  int FlashCnt = 0;
  enum class FlashState {
    Show,
    Clear,
  };
  FlashState State = FlashState::Show;

  bool flashShouldClear();

public:
  Display(int CLK_PIN, int DIO_PIN);
  Display(const Display &) = delete;
  void printRaw(int Num);
  void printMHz(int MHz);
  void printKHz(int KHz);
  void printTxt(const char *Str);
  /// Turns on/off flashing of the display.
  void setFlash(bool NewState) { Flash = NewState; }
};

#endif // __DISPLAY_H__
