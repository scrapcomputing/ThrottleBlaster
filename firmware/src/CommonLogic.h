//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __COMMONLOGIC_H__
#define __COMMONLOGIC_H__

#include "Button.h"
#include "Debug.h"
#include "Display.h"
#include "DutyCycle.h"
#include "Flash.h"
#include "Presets.h"
#include "Uart.h"
#include <chrono>
#include <optional>
#include <sstream>
#include <vector>

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
    Boot,
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
    case Mode::Boot: return "Boot";
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
  static constexpr const char *MsgResetDetected = "boot";

  int BeforeMaxMHz = 0;
  int BeforeActualKHz = 0;
  int BeforePeriod = 0;

  std::vector<Button</*OffVal=*/true, ButtonDebounceSz, ButtonLongPressCnt,
                     ButtonMedReleaseCnt>>
      PresetBtns;

  Button</*OffVal=*/true, ButtonDebounceSz, ButtonLongPressCnt,
         ButtonMedReleaseCnt>
      ResetSense;
  std::optional<std::chrono::time_point<std::chrono::system_clock>>
      ResetTimeOpt;
  int ResetSavedKHz = 0;
  int ResetSavedPeriod = 0;
  Mode ResetSavedMode;

  void printTxtAndSleep(const char *Str);

  void updateDisplay();

  static constexpr const size_t MaxUartStrSz = 10;
  std::string UartStr;
  void uartTick(Uart &Uart);
  void presetBtnsTick();
  void resetSenseTick();

  virtual void tick() = 0;

public:
  CommonLogic(int SamplePeriod, Display &Disp, DutyCycle &DC,
              PresetsTable &Presets, FlashStorage &Flash, Pico &Pi)
      : SamplePeriod(SamplePeriod), Disp(Disp), DC(DC), Presets(Presets),
        Flash(Flash), ResetSense(ResetSenseGPIO, Pi, "ResetSense") {
    for (int BtnIdx = 0, E = PresetBtnGPIOs.size(); BtnIdx != E; ++BtnIdx) {
      std::string BtnStr = "PresetBtn." + std::to_string(BtnIdx);
      PresetBtns.emplace_back(PresetBtnGPIOs[BtnIdx], Pi, BtnStr.c_str());
    }
  }
  void tickAll(Uart &Uart) {
    tick();
    uartTick(Uart);
    presetBtnsTick();
    resetSenseTick();
  }
};

#endif // __COMMONLOGIC_H__
