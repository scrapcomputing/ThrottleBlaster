//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __POTENTIOMETERLOGIC_H__
#define __POTENTIOMETERLOGIC_H__

#include "Button.h"
#include "CommonLogic.h"
#include "ConfigOpts.h"
#include "Display.h"
#include "Flash.h"
#include "Pico.h"
#include "Potentiometer.h"
#include "Presets.h"

class PotentiometerLogic : public CommonLogic {
  static constexpr const int PotMaxVal = 1024;
  Button</*OffVal=*/true, ButtonDebounceSz, ButtonLongPressCnt,
         ButtonMedReleaseCnt>
      Btn;
  Potentiometer<PotMaxVal, ADCDenoise, ADCDeadZone> Pot;
  Pico &Pi;
  int LoopCntSinceSample = 0;

  static constexpr const int LeftPotLimit = PotLeftPercent * PotMaxVal / 100;
  static constexpr const int RightPotLimit = PotRightPercent * PotMaxVal / 100;

  CommonLogic::Mode LastMode = Mode::Manual;

  int LastPotVal = -1;
  /// The capacitor attached to the potentiometer causes it to ramp-up.
  /// We need to ignore these values.
  bool InitRampUp = true;
  int InitRampUpCnt = 0;

  void manual(int PotVal, ButtonState BtnState);

  void uart(int PotVal, ButtonState BtnState);

  void cyclePresets(int PotVal, ButtonState BtnState);

  void configPeriod(int PotVal, ButtonState BtnState);

  void configResetToDefaults(int PotVal, ButtonState BtnState);

  void configMaxMHz(int PotVal, ButtonState BtnState);

  void configMHz(int PotVal, ButtonState BtnState);

  int SvPotVal = 0;
  bool MovedPot = false;

  void savePotVal(int PotVal) {
    SvPotVal = PotVal;
    MovedPot = false;
  }
  bool movedPotComparedToSaved(int PotVal) {
    int PotDiff = std::abs(PotVal - SvPotVal);
    if (PotDiff > PotMoveAgainstSavedIdleLimit)
      MovedPot = true;
    return MovedPot;
  }
  enum class PotDir {
    Right,
    Mid,
    Left,
  };
  PotDir getPotDir(int PotVal) const {
    if (PotVal > RightPotLimit)
      return PotDir::Right;
    if (PotVal < LeftPotLimit)
      return PotDir::Left;
    return PotDir::Mid;
  }

  bool EnablePot = false;;

public:
  PotentiometerLogic(int PotGPIO, int ButtonGPIO, int SamplePeriod, Pico &Pi,
                     Display &Disp, DutyCycle &DC, PresetsTable &Presets,
                     FlashStorage &Flash, bool EnablePot)
      : Btn(ButtonGPIO, Pi, "Pot.Btn"), Pot(PotGPIO, Pi), Pi(Pi),
        CommonLogic(SamplePeriod, Disp, DC, Presets, Flash),
        EnablePot(EnablePot) {
    setMode(Mode::Presets);
  }
  void tick() final;
};

#endif // __POTENTIOMETERLOGIC_H__
