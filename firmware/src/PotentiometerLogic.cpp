#include "PotentiometerLogic.h"
#include "Debug.h"
#include "config.h"

void PotentiometerLogic::manual(int PotVal, ButtonState BtnState) {
  switch (BtnState) {
  case ButtonState::Release:
    savePotVal(PotVal);
    setMode(Mode::Presets);
    printTxtAndSleep(MsgPresets);
    return;
  case ButtonState::LongPress:
    if (Presets.atMaxKHz()) {
      setMode(Mode::ConfigMHz);
      printTxtAndSleep(MsgActualFreq);
    } else {
      setMode(Mode::ConfigMaxMHz);
      printTxtAndSleep(MsgMaxMHz);
    }
    savePotVal(PotVal);
    Disp.setFlash(true);
    return;
  }
  int MaxKHz = Presets.getMaxKHz();
  DC.setKHz(PotVal * MaxKHz / PotMaxVal);
}

void PotentiometerLogic::cyclePresets(int PotVal, ButtonState BtnState) {
  if (EnablePot && movedPotComparedToSaved(PotVal)) {
    setMode(Mode::Manual);
    return;
  }
  switch (BtnState) {
  case ButtonState::Release:
    Presets.cyclePrev();
    DC.setKHz(Presets.getKHz());
    break;
  case ButtonState::MedRelease:
    Presets.cycleMax();
    DC.setKHz(Presets.getKHz());
    break;
  case ButtonState::LongPress:
    if (Presets.atMaxKHz()) {
      setMode(Mode::ConfigMaxMHz);
      printTxtAndSleep(MsgMaxMHz);
    } else {
      setMode(Mode::ConfigMHz);
      printTxtAndSleep(MsgActualFreq);
    }
    savePotVal(PotVal);
    Disp.setFlash(true);
    break;
  }
}

void PotentiometerLogic::configPWM(int PotVal, ButtonState BtnState) {
  switch (BtnState) {
  case ButtonState::Release:
    Presets.incrPWM();
    DC.setPeriod(Presets.getPWM());
    DBG_PRINT(std::cout << "PWM=" << Presets.getPWM() << "\n";)
    return;
  case ButtonState::MedRelease:
    Presets.decrPWM();
    DC.setPeriod(Presets.getPWM());
    DBG_PRINT(std::cout << "PWM=" << Presets.getPWM() << "\n";)
    return;
  case ButtonState::LongPress:
    tryWritePresetsToFlash();
    Disp.setFlash(false);
    printTxtAndSleep(MsgEnd);
    savePotVal(PotVal);
    setMode(Mode::Presets);
    return;
  }

  // If we moved the potentiometer enough, use it to control the value.
  if (EnablePot && movedPotComparedToSaved(PotVal)) {
    switch (getPotDir(PotVal)) {
    case PotDir::Right:
      Presets.incrPWM();
      DC.setPeriod(Presets.getPWM());
      break;
    case PotDir::Left:
      Presets.decrPWM();
      DC.setPeriod(Presets.getPWM());
      break;
    }
  }
}

void PotentiometerLogic::configResetToDefaults(int PotVal,
                                               ButtonState BtnState) {
  auto DontReset = [this, PotVal]() {
    tryWritePresetsToFlash();
    Disp.setFlash(false);
    printTxtAndSleep(MsgEscape);
    printTxtAndSleep(MsgPresets);
    savePotVal(PotVal);
    setMode(Mode::Presets);
  };
  switch (BtnState) {
  case ButtonState::Release:
  case ButtonState::MedRelease:
    DontReset();
    break;
  case ButtonState::LongPress:
    Presets.resetToDefaults(Flash);
    Disp.setFlash(false);
    printTxtAndSleep(MsgEnd);
    savePotVal(PotVal);
    setMode(Mode::Presets);
    break;
  }
  if (EnablePot && movedPotComparedToSaved(PotVal))
    DontReset();
}

void PotentiometerLogic::configMaxMHz(int PotVal, ButtonState BtnState) {
  switch (BtnState) {
  case ButtonState::LongPress:
    setMode(Mode::ResetToDefaults);
    printTxtAndSleep(MsgResetToDefaults);
    return;
  case ButtonState::Release: {
    int Base = Presets.getMaxMHz() - Presets.getMaxMHz() % 100;
    Presets.incrMaxMHz(/*Step=*/1, /*Wrap=*/true, /*WrapLo=*/Base,
                       /*WrapHi=*/Base + 99);
    DC.setKHz(Presets.getMaxKHz());
    return;
  }
  case ButtonState::MedRelease: {
    Presets.incrMaxMHz(/*Step=*/100, /*Wrap=*/true);
    DC.setKHz(Presets.getMaxKHz());
    return;
  }
  }
  // If we moved the potentiometer enough, use it to control the value.
  if (EnablePot && movedPotComparedToSaved(PotVal)) {
    switch (getPotDir(PotVal)) {
    case PotDir::Right:
      Presets.incrMaxMHz();
      break;
    case PotDir::Left:
      Presets.decrMaxMHz();
      break;
    }
  }
}

void PotentiometerLogic::configMHz(int PotVal, ButtonState BtnState) {
  switch (BtnState) {
  case ButtonState::Release:
    Presets.incrActualKHz();
    return;
  case ButtonState::MedRelease:
    Presets.decrActualKHz();
    return;
  case ButtonState::LongPress:
    Disp.setFlash(false);
    printTxtAndSleep(MsgPWM);
    Disp.setFlash(true);
    setMode(Mode::ConfigPWM);
    return;
  }
  // If we moved the potentiometer enough, use it to control the value.
  if (EnablePot && movedPotComparedToSaved(PotVal)) {
    switch (getPotDir(PotVal)) {
    case PotDir::Right:
      Presets.incrActualKHz();
      break;
    case PotDir::Left:
      Presets.decrActualKHz();
      break;
    }
  }
}

void PotentiometerLogic::tick() {
  if (++LoopCntSinceSample != SamplePeriod)
    return;
  LoopCntSinceSample = 0;

  updateDisplay();

  auto BtnState = Btn.get();
  int PotVal = Pot.get();

  if (EnablePot && InitRampUp) {
    // There is a small capacitor connected to the potentiometer for
    // stability, but it causes the values to ramp-up. Ignore them.
    if (++InitRampUpCnt < PotentiometerRampUpIgnoreReadings)
      return;
    InitRampUp = false;
    LastPotVal = PotVal;
    savePotVal(PotVal);
  }

  switch (getMode()) {
  case Mode::Manual:
    manual(PotVal, BtnState);
    break;
  case Mode::Presets:
    cyclePresets(PotVal, BtnState);
    break;
  case Mode::ConfigMHz:
    configMHz(PotVal, BtnState);
    break;
  case Mode::ConfigPWM:
    configPWM(PotVal, BtnState);
    break;
  case Mode::ConfigMaxMHz:
    configMaxMHz(PotVal, BtnState);
    break;
  case Mode::ResetToDefaults:
    configResetToDefaults(PotVal, BtnState);
    break;
  }
  LastMode = getMode();
  LastPotVal = PotVal;
}
