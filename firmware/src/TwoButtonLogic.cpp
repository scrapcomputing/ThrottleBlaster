//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#include "TwoButtonLogic.h"

void TwoButtonLogic::cyclePresets(ButtonState LBtnState,
                                  ButtonState RBtnState) {
  auto LongPress = [this]() {
    if (Presets.atMaxKHz()) {
      setMode(Mode::ConfigMaxMHz);
      printTxtAndSleep(MsgMaxMHz);
    } else {
      setMode(Mode::ConfigMHz);
      printTxtAndSleep(MsgActualFreq);
    }
    Disp.setFlash(true);
  };

  if (bothRelease(LBtnState, RBtnState)) {
    setMode(Mode::Manual);
    printTxtAndSleep(MsgManual);
    return;
  }
  if (bothLongPress(LBtnState, RBtnState)) {
    setMode(Mode::ResetToDefaults);
    printTxtAndSleep(MsgResetToDefaults);
    return;
  }
  // Skip if both buttons are pressed.
  if (LBtnState == ButtonState::LongPress && RBtnState == ButtonState::Pressed)
    return;
  if (RBtnState == ButtonState::LongPress && LBtnState == ButtonState::Pressed)
    return;

  switch (LBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.prev();
    DC.setKHz(Presets.getKHz());
    break;
  case ButtonState::LongPress:
    LongPress();
    break;
  }
  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.next();
    DC.setKHz(Presets.getKHz());
    break;
  case ButtonState::LongPress:
    LongPress();
    break;
  }
}

void TwoButtonLogic::manual(ButtonState LBtnState, ButtonState RBtnState) {
  if (bothRelease(LBtnState, RBtnState)) {
    setMode(Mode::Presets);
    printTxtAndSleep(MsgPresets);
    return;
  }
  switch (LBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    DC.decrMHz();
    break;
  case ButtonState::LongPress:
    break;
  }

  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    DC.incrMHz();
    break;
  case ButtonState::LongPress:
    break;
  }
}

void TwoButtonLogic::configPWM(ButtonState LBtnState, ButtonState RBtnState) {
  auto LongPress = [this]() {
    // Save
    tryWritePresetsToFlash();
    Disp.setFlash(false);
    printTxtAndSleep(MsgEnd);
    setMode(Mode::Presets);
  };

  if (bothRelease(LBtnState, RBtnState)) {
    // Cancel
    setMode(Mode::Presets);
    printTxtAndSleep(MsgEscape);
    Disp.setFlash(false);
    return;
  }
  switch (LBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.decrPWM();
    DC.setPeriod(Presets.getPWM());
    DBG_PRINT(std::cout << "PWM=" << Presets.getPWM() << "\n";)
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  }

  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.incrPWM();
    DC.setPeriod(Presets.getPWM());
    DBG_PRINT(std::cout << "PWM=" << Presets.getPWM() << "\n";)
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  }
}

void TwoButtonLogic::configResetToDefaults(ButtonState LBtnState,
                                           ButtonState RBtnState) {
  auto DontReset = [this] {
    tryWritePresetsToFlash();
    Disp.setFlash(false);
    printTxtAndSleep(MsgEscape);
    printTxtAndSleep(MsgPresets);
    setMode(Mode::Presets);
  };
  switch (LBtnState) {
  case ButtonState::Release:
    DontReset();
    break;
  }
  switch (RBtnState) {
  case ButtonState::Release:
    DontReset();
    break;
  }
  if (bothLongPress(LBtnState, RBtnState)) {
    Presets.resetToDefaults(Flash);
    Disp.setFlash(false);
    printTxtAndSleep(MsgEnd);
    setMode(Mode::Presets);
    return;
  }
}

void TwoButtonLogic::configMaxMHz(ButtonState LBtnState, ButtonState RBtnState) {
  auto LongPress = [this]() {
    // Save
    tryWritePresetsToFlash();
    setMode(Mode::Presets);
    Disp.setFlash(false);
    printTxtAndSleep(MsgEnd);
  };
  if (bothRelease(LBtnState, RBtnState)) {
    // Cancel
    setMode(Mode::Presets);
    printTxtAndSleep(MsgEscape);
    Disp.setFlash(false);
    return;
  }
  switch (LBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.decrMaxMHz(/*Step=*/1);
    DC.setKHz(Presets.getMaxKHz());
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  }

  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.incrMaxMHz(/*Step=*/TwoBtnMaxMHzStep, /*Wrap=*/true,
                       /*WrapLo=*/MHzLimitLo,
                       /*WrapHi=*/MHzLimitHi);
    DC.setKHz(Presets.getMaxKHz());
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  }
}

void TwoButtonLogic::configMHz(ButtonState LBtnState, ButtonState RBtnState) {
  auto LongPress = [this] () {
    // Proceed to PWM mode
    Disp.setFlash(false);
    printTxtAndSleep(MsgPWM);
    Disp.setFlash(true);
    setMode(Mode::ConfigPWM);
  };

  if (bothRelease(LBtnState, RBtnState)) {
    setMode(Mode::Presets);
    printTxtAndSleep(MsgEscape);
    Disp.setFlash(false);
    return;
  }
  switch (LBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.decrActualKHz();
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  }

  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.incrActualKHz();
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  }
}

void TwoButtonLogic::tick() {
 if (++LoopCntSinceSample != SamplePeriod)
    return;
  LoopCntSinceSample = 0;

  updateDisplay();

  auto LBtnState = LBtn.get();
  auto RBtnState = RBtn.get();

  switch (getMode()) {
  case Mode::Presets:
    cyclePresets(LBtnState, RBtnState);
    break;
  case Mode::Manual:
    manual(LBtnState, RBtnState);
    break;
  case Mode::ConfigMHz:
    configMHz(LBtnState, RBtnState);
    break;
  case Mode::ConfigPWM:
    configPWM(LBtnState, RBtnState);
    break;
  case Mode::ConfigMaxMHz:
    configMaxMHz(LBtnState, RBtnState);
    break;
  case Mode::ResetToDefaults:
    configResetToDefaults(LBtnState, RBtnState);
    break;
  }
  LastMode = getMode();
}
