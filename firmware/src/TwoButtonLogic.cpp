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
    if (Presets.atMaxKHz()) {
      setMode(Mode::ResetToDefaults);
      printTxtAndSleep(MsgResetToDefaults);
    }
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
    DC.setPeriod(Presets.getPeriod());
    break;
  case ButtonState::LongPress:
    LongPress();
    break;
  default:
    break;
  }
  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.next();
    DC.setKHz(Presets.getKHz());
    DC.setPeriod(Presets.getPeriod());
    break;
  case ButtonState::LongPress:
    LongPress();
    break;
  default:
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
  default:
    break;
  }

  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    DC.incrMHz();
    break;
  case ButtonState::LongPress:
    break;
  default:
    break;
  }
}

void TwoButtonLogic::uart(ButtonState LBtnState, ButtonState RBtnState) {
  if (LBtnState != ButtonState::None || RBtnState != ButtonState::None) {
    setMode(Mode::Presets);
    printTxtAndSleep(MsgPresets);
  }
}

void TwoButtonLogic::configPeriod(ButtonState LBtnState, ButtonState RBtnState) {
  auto LongPress = [this]() {
    // Save
    setMode(Mode::DeletePreset);
    printTxtAndSleep(MsgDeletePreset);
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
    Presets.decrPeriod();
    DC.setPeriod(Presets.getPeriod());
    DBG_PRINT(std::cout << "Period=" << Presets.getPeriod() << "\n";)
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  default:
    break;
  }

  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.incrPeriod();
    DC.setPeriod(Presets.getPeriod());
    DBG_PRINT(std::cout << "Period=" << Presets.getPeriod() << "\n";)
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  default:
    break;
  }
}

void TwoButtonLogic::configDeletePreset(ButtonState LBtnState, ButtonState RBtnState) {
  auto LongPress = [this]() {
    // Save
    tryWritePresetsToFlash();
    Disp.setFlash(false);
    printTxtAndSleep(MsgConfirm);
    setMode(Mode::Presets);
    if (Presets.isDeleted())
      Presets.next();
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
    Presets.toggleDeleted();
    DBG_PRINT(std::cout << "Deleted=" << Presets.isDeleted() << "\n";)
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  default:
    break;
  }

  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.toggleDeleted();
    DBG_PRINT(std::cout << "Deleted=" << Presets.isDeleted() << "\n";)
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  default:
    break;
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
  default:
    break;
  }
  switch (RBtnState) {
  case ButtonState::Release:
    DontReset();
    break;
  default:
    break;
  }
  if (bothLongPress(LBtnState, RBtnState)) {
    Presets.resetToDefaults(Flash);
    Disp.setFlash(false);
    printTxtAndSleep(MsgConfirm);
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
    printTxtAndSleep(MsgConfirm);
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
  default:
    break;
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
  default:
    break;
  }
}

void TwoButtonLogic::configMHz(ButtonState LBtnState, ButtonState RBtnState) {
  auto LongPress = [this] () {
    // Proceed to Period mode
    Disp.setFlash(false);
    printTxtAndSleep(MsgPeriod);
    Disp.setFlash(true);
    setMode(Mode::ConfigPeriod);
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
  default:
    break;
  }

  switch (RBtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    Presets.incrActualKHz();
    return;
  case ButtonState::LongPress:
    LongPress();
    return;
  default:
    break;
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
  case Mode::ConfigPeriod:
    configPeriod(LBtnState, RBtnState);
    break;
  case Mode::DeletePreset:
    configDeletePreset(LBtnState, RBtnState);
    break;
  case Mode::ConfigMaxMHz:
    configMaxMHz(LBtnState, RBtnState);
    break;
  case Mode::ResetToDefaults:
    configResetToDefaults(LBtnState, RBtnState);
    break;
  case Mode::Uart:
    uart(LBtnState, RBtnState);
    break;
  case Mode::Boot:
    break;
  }
  LastMode = getMode();
}
