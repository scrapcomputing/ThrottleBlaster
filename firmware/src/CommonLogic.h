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
#include "Uart.h"
#include <sstream>

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
    ConfigPeriod,
    Manual,
    ConfigMaxMHz,
    ResetToDefaults,
    Uart,
  };

  Mode CurrMode = Mode::Presets;

  static constexpr const char *getModeStr(Mode Mode) {
    switch (Mode) {
    case Mode::Presets: return "Presets";
    case Mode::ConfigMHz: return "ConfigMHz";
    case Mode::ConfigPeriod: return "ConfigPeriod";
    case Mode::Manual: return "Manual";
    case Mode::ConfigMaxMHz: return "ConfigMaxMHz";
    case Mode::ResetToDefaults: return "ResetToDefaults";
    case Mode::Uart: return "UART";
    }
    return "BAD";
  }
  /// Called by setMode(). Can be overriden by implementations.
  virtual void setModeInit(Mode NewMode) {}
  void setMode(Mode NewMode);
  void tryWritePresetsToFlash();
  Mode getMode() const { return CurrMode; }

  static constexpr const char *MsgActualFreq = "FrE";
  static constexpr const char *MsgPeriod = "PEr";
  static constexpr const char *MsgMaxMHz = "CPUF";
  static constexpr const char *MsgResetToDefaults = "RES";
  static constexpr const char *MsgEscape = "ESC";
  static constexpr const char *MsgConfirm = "done";
  static constexpr const char *MsgPresets = "PRE";
  static constexpr const char *MsgManual = "1-1";
  static constexpr const char *MsgUartErr = "UErr";
  static constexpr const char *MsgUartMode = "UArt";

  int BeforeMaxMHz = 0;
  int BeforeActualKHz = 0;
  int BeforePeriod = 0;

  void printTxtAndSleep(const char *Str);

  void updateDisplay();

  static constexpr const size_t MaxUartStrSz = 10;
  std::string UartStr;
  void uartTick(Uart &Uart);

  virtual void tick() = 0;

public:
  CommonLogic(int SamplePeriod, Display &Disp, DutyCycle &DC,
              PresetsTable &Presets, FlashStorage &Flash)
      : SamplePeriod(SamplePeriod), Disp(Disp), DC(DC), Presets(Presets),
        Flash(Flash) {}
  void tickAll(Uart &Uart) {
    tick();
    uartTick(Uart);
  }
};

#endif // __COMMONLOGIC_H__
