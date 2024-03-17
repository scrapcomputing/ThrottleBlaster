//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __COMMONLOGIC_H__
#define __COMMONLOGIC_H__

#include "Debug.h"
#include "Display.h"
#include "DutyCycle.h"
#include "Flash.h"
#include "Presets.h"

class CommonLogic {
protected:
  int SamplePeriod;
  int LoopCntSinceSample = 0;

  Display &Disp;
  DutyCycle &DC;
  PresetsTable &Presets;
  FlashStorage &Flash;

  enum class Mode {
    Presets,
    ConfigMHz,
    ConfigPWM,
    Manual,
    ConfigMaxMHz,
    ResetToDefaults,
  };

  Mode CurrMode = Mode::Presets;

  static constexpr const char *getModeStr(Mode Mode) {
    switch (Mode) {
    case Mode::Presets: return "Presets";
    case Mode::ConfigMHz: return "ConfigMHz";
    case Mode::ConfigPWM: return "ConfigPWM";
    case Mode::Manual: return "Manual";
    case Mode::ConfigMaxMHz: return "ConfigMaxMHz";
    case Mode::ResetToDefaults: return "ResetToDefaults";
    }
    return "BAD";
  }

  void setMode(Mode NewMode);
  void tryWritePresetsToFlash();
  Mode getMode() const { return CurrMode; }

  static constexpr const char *MsgActualFreq = "FrE";
  static constexpr const char *MsgPWM = "PEr";
  static constexpr const char *MsgMaxMHz = "CPUF";
  static constexpr const char *MsgResetToDefaults = "RES";
  static constexpr const char *MsgEscape = "ESC";
  static constexpr const char *MsgEnd = "END";
  static constexpr const char *MsgPresets = "PRE";
  static constexpr const char *MsgManual = "1-1";

  int BeforeMaxMHz = 0;
  int BeforeActualKHz = 0;
  int BeforePWM = 0;

  void printTxtAndSleep(const char *Str);
  static constexpr const int PrintSleep = 1000;

  void updateDisplay();

public:
  CommonLogic(int SamplePeriod, Display &Disp, DutyCycle &DC,
              PresetsTable &Presets, FlashStorage &Flash)
      : SamplePeriod(SamplePeriod), Disp(Disp), DC(DC), Presets(Presets),
        Flash(Flash) {}
  virtual void tick() = 0;
};

#endif // __COMMONLOGIC_H__
