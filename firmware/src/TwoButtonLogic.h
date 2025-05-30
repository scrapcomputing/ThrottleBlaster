//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __TWOBUTTONLOGIC_H__
#define __TWOBUTTONLOGIC_H__

#include "Button.h"
#include "CommonLogic.h"
#include "ConfigOpts.h"
#include "Display.h"
#include "Flash.h"
#include "Pico.h"
#include "Presets.h"

class TwoButtonLogic : public CommonLogic {
  static constexpr const int PotMaxVal = 1024;
  Button</*OffVal=*/true, ButtonDebounceSz, ButtonLongPressCnt,
         ButtonMedReleaseCnt>
      LBtn;
  Button</*OffVal=*/true, ButtonDebounceSz, ButtonLongPressCnt,
         ButtonMedReleaseCnt>
      RBtn;
  Pico &Pi;
  int LoopCntSinceSample = 0;

  CommonLogic::Mode LastMode = Mode::Manual;

  void cyclePresets(ButtonState LBtnState, ButtonState RBtnState);

  void manual(ButtonState LBtnState, ButtonState RBtnState);

  void uart(ButtonState LBtnState, ButtonState RBtnState);

  void configPeriod(ButtonState LBtnState, ButtonState RBtnState);

  void configResetToDefaults(ButtonState LBtnState, ButtonState RBtnState);

  void configMaxMHz(ButtonState LBtnState, ButtonState RBtnState);

  void configMHz(ButtonState LBtnState, ButtonState RBtnState);

public:
  TwoButtonLogic(int LeftButtonGPIO, int RightButtonGPIO, int SamplePeriod,
                 Pico &Pi, Display &Disp, DutyCycle &DC, PresetsTable &Presets,
                 FlashStorage &Flash)
      : CommonLogic(SamplePeriod, Disp, DC, Presets, Flash, Pi),
        LBtn(LeftButtonGPIO, Pi, "LBtn"), RBtn(RightButtonGPIO, Pi, "LBtn"),
        Pi(Pi) {
    setMode(Mode::Presets);
  }
  void tick() final;
};

#endif // __TWOBUTTONLOGIC_H__
