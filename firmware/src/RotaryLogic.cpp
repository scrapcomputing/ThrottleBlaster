//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#include "RotaryLogic.h"
#include "Debug.h"

void RotaryLogic::escapeReset() {
  setMode(Mode::Presets);
  Disp.setFlash(false);
  printTxtAndSleep(MsgEscape);
}

void RotaryLogic::onSwRelease() {
  switch (getMode()) {
  case Mode::Presets:
    setMode(Mode::Manual);
    printTxtAndSleep(MsgManual);
    break;
  case Mode::Manual:
    setMode(Mode::Presets);
    DC.setKHz(Presets.getActualKHz());
    printTxtAndSleep(MsgPresets);
    break;
  case Mode::ConfigMHz:
    setMode(Mode::ConfigPWM);
    Disp.setFlash(false);
    printTxtAndSleep(MsgPWM);
    Disp.setFlash(true);
    break;
  case Mode::ConfigPWM:
    setMode(Mode::Presets);
    tryWritePresetsToFlash();
    Disp.setFlash(false);
    printTxtAndSleep(MsgEnd);
    break;
  case Mode::ConfigMaxMHz:
    tryWritePresetsToFlash();
    setMode(Mode::Presets);
    Disp.setFlash(false);
    printTxtAndSleep(MsgEnd);
    break;
  case Mode::ResetToDefaults:
    escapeReset();
    break;
  }
}

void RotaryLogic::onSwLongPress() {
  switch (getMode()) {
  case Mode::Manual:
    if (Presets.atMaxKHz()) {
      setMode(Mode::ConfigMaxMHz);
      printTxtAndSleep(MsgMaxMHz);
    }
    break;
  case Mode::Presets:
    if (Presets.atMaxKHz()) {
      setMode(Mode::ConfigMaxMHz);
      printTxtAndSleep(MsgMaxMHz);
    } else {
      setMode(Mode::ConfigMHz);
      printTxtAndSleep(MsgActualFreq);
    }
    Disp.setFlash(true);
    break;
  case Mode::ConfigMHz:
  case Mode::ConfigPWM:
    setMode(Mode::Presets);
    printTxtAndSleep(MsgEscape);
    Disp.setFlash(false);
    break;
  case Mode::ConfigMaxMHz:
    setMode(Mode::ResetToDefaults);
    printTxtAndSleep(MsgResetToDefaults);
    Disp.setFlash(true);
    break;
  case Mode::ResetToDefaults:
    setMode(Mode::Presets);
    Presets.resetToDefaults(Flash);
    printTxtAndSleep(MsgEnd);
    Disp.setFlash(false);
    break;
  }
}

void RotaryLogic::onLeft() {
  switch (getMode()) {
  case Mode::Presets:
    Presets.prev();
    DC.setKHz(Presets.getActualKHz());
    DBG_PRINT(std::cout << "Presets=" << Presets.getKHz() << "\n";)
    break;
  case Mode::ConfigMHz:
    Presets.decrActualKHz();
    DC.setKHz(Presets.getActualKHz());
    DBG_PRINT(std::cout << "ActualKHz=" << Presets.getActualKHz() << "\n";)
    break;
  case Mode::ConfigPWM:
    Presets.decrPWM();
    DC.setPeriod(Presets.getPWM());
    DBG_PRINT(std::cout << "PWM=" << Presets.getPWM() << "\n";)
    break;
  case Mode::Manual:
    DC.decrMHz();
    DBG_PRINT(std::cout << DC.getKHz() << "\n";)
    break;
  case Mode::ConfigMaxMHz: {
    int PrevMHz = Presets.getMaxMHz() - RotaryMaxMHzLeftStep;
    if (PrevMHz < MHzLimitLo)
      PrevMHz = MHzLimitHi;
    Presets.updateMaxMHz(PrevMHz);
    DC.setKHz(Presets.getMaxKHz());
    DBG_PRINT(std::cout << "MaxMHz=" << Presets.getMaxMHz() << "\n";)
    break;
  }
  case Mode::ResetToDefaults:
    escapeReset();
    break;
  }
}

void RotaryLogic::onRight() {
  switch (getMode()) {
  case Mode::Presets:
    Presets.next();
    DC.setKHz(Presets.getActualKHz());
    DBG_PRINT(std::cout << "Presets=" << Presets.getKHz() << "\n";)
    break;
  case Mode::ConfigMHz:
    Presets.incrActualKHz();
    DC.setKHz(Presets.getActualKHz());
    DBG_PRINT(std::cout << "ActualKHz=" << Presets.getActualKHz() << "\n";)
    break;
  case Mode::ConfigPWM: {
    Presets.incrPWM();
    DC.setPeriod(Presets.getPWM());
    DBG_PRINT(std::cout << "PWM=" << Presets.getPWM() << "\n";)
    break;
  case Mode::Manual:
    DC.incrMHz();
    DBG_PRINT(std::cout << DC.getKHz() << "\n";)
    break;
  case Mode::ConfigMaxMHz: {
    int NextMHz = Presets.getMaxMHz() + RotaryMaxMHzRightStep;
    if (NextMHz > MHzLimitHi)
      NextMHz = MHzLimitLo;
    Presets.updateMaxMHz(NextMHz);
    DC.setKHz(Presets.getMaxKHz());
    DBG_PRINT(std::cout << "MaxMHz=" << Presets.getMaxMHz() << "\n";)
    break;
  }
  case Mode::ResetToDefaults:
    setMode(Mode::Presets);
    Disp.setFlash(false);
    printTxtAndSleep(MsgEscape);
    break;
  }
  }
}

void RotaryLogic::tick() {
  if (++LoopCntSinceSample != SamplePeriod)
    return;
  LoopCntSinceSample = 0;

  updateDisplay();

  switch (RotEnc.get()) {
  case RotaryEncoder::State::SwRelease: {
    DBG_PRINT(std::cout << "SwRelase\n";)
    onSwRelease();
    break;
  }
  case RotaryEncoder::State::SwLongPress: {
    DBG_PRINT(std::cout << "SwLongPress\n";)
    onSwLongPress();
    break;
  }
  case RotaryEncoder::State::Left: {
    DBG_PRINT(std::cout << "Left\n";)
    onLeft();
    break;
  }
  case RotaryEncoder::State::Right: {
    DBG_PRINT(std::cout << "Right\n";)
    onRight();
    break;
  }
  }
}