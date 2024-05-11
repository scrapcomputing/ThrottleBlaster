//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __ROTARYLOGIC_H__
#define __ROTARYLOGIC_H__

#include "CommonLogic.h"
#include "DutyCycle.h"
#include "Flash.h"
#include "Pico.h"
#include "RotaryEncoder.h"
#include <config.h>

class RotaryLogic : public CommonLogic {

  RotaryEncoder RotEnc;

  int BeforeMaxMHz = 0;
  int BeforeActualKHz = 0;
  int BeforePWM = 0;

  void escapeReset();

  void onSwRelease();
  void onSwLongPress();
  void onLeft();
  void onRight();

public:
  RotaryLogic(int ClkGPIO, int DtGPIO, int SwGPIO, int SamplePeriod, Pico &Pi,
              Display &Disp, PresetsTable &Presets, DutyCycle &DC,
              FlashStorage &Flash, bool ReverseDirection)
    : RotEnc(ClkGPIO, DtGPIO, SwGPIO, Pi, ReverseDirection),
        CommonLogic(SamplePeriod, Disp, DC, Presets, Flash) {
    setMode(Mode::Presets);
  }
  RotaryLogic(const RotaryLogic &) = delete;
  void tick() final;
};

#endif // __ROTARYLOGIC_H__
