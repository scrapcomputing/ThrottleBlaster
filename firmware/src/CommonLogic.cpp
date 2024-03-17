#include "CommonLogic.h"
#include "Debug.h"
#include "pico/time.h"

void CommonLogic::setMode(Mode NewMode) {
  BeforeMaxMHz = Presets.getMaxMHz();
  BeforePWM = Presets.getPWM();
  BeforeActualKHz = Presets.getActualKHz();

  CurrMode = NewMode;
  DBG_PRINT(std::cout << getModeStr(CurrMode) << "\n";)
}

void CommonLogic::tryWritePresetsToFlash() {
  if (Presets.getActualKHz() != BeforeActualKHz ||
      Presets.getPWM() != BeforePWM || Presets.getMaxMHz() != BeforeMaxMHz) {
    DBG_PRINT(std::cout << "WriteToFlash:\n";)
    DBG_PRINT(std::cout << "           Before After\n";)
    DBG_PRINT(std::cout << "ActualKHz: " << BeforeActualKHz << " "
                        << Presets.getActualKHz() << "\n";)
    DBG_PRINT(std::cout << "PWM:       " << BeforePWM << " " << Presets.getPWM()
                        << "\n";)
    DBG_PRINT(std::cout << "MaxMHz:    " << BeforeMaxMHz << " "
                        << Presets.getMaxMHz() << "\n";)
    Presets.writeToFlash(Flash);
  }
}

void CommonLogic::printTxtAndSleep(const char *Str) {
  Disp.printTxt(Str);
  sleep_ms(PrintSleep);
}

void CommonLogic::updateDisplay() {
#ifdef PICO_TM1637
  switch (getMode()) {
  case Mode::Presets:
    Disp.printKHz(Presets.getKHz());
    break;
  case Mode::ConfigMHz:
    Disp.printKHz(Presets.getActualKHz());
    break;
  case Mode::ConfigPWM:
    Disp.printRaw(Presets.getPWM());
    break;
  case Mode::Manual:
    Disp.printKHz(DC.getKHz());
    break;
  case Mode::ConfigMaxMHz:
    Disp.printMHz(Presets.getMaxMHz());
    break;
  case Mode::ResetToDefaults:
    Disp.printTxt(MsgResetToDefaults);
    break;
  }
#endif
}
