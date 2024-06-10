#include "CommonLogic.h"
#include "Debug.h"
#include "pico/time.h"
#include <string>

void CommonLogic::setMode(Mode NewMode) {
  setModeInit(NewMode);

  BeforeMaxMHz = Presets.getMaxMHz();
  BeforePeriod = Presets.getPeriod();
  BeforeActualKHz = Presets.getActualKHz();

  CurrMode = NewMode;
  DBG_PRINT(std::cout << getModeStr(CurrMode) << "\n";)
}

void CommonLogic::tryWritePresetsToFlash() {
  if (Presets.getActualKHz() != BeforeActualKHz ||
      Presets.getPeriod() != BeforePeriod ||
      Presets.getMaxMHz() != BeforeMaxMHz) {
    DBG_PRINT(std::cout << "WriteToFlash:\n";)
    DBG_PRINT(std::cout << "           Before After\n";)
    DBG_PRINT(std::cout << "ActualKHz: " << BeforeActualKHz << " "
                        << Presets.getActualKHz() << "\n";)
    DBG_PRINT(std::cout << "Period:       " << BeforePeriod << " "
                        << Presets.getPeriod() << "\n";)
    DBG_PRINT(std::cout << "MaxMHz:    " << BeforeMaxMHz << " "
                        << Presets.getMaxMHz() << "\n";)
    Presets.writeToFlash(Flash);
  } else {
    DBG_PRINT(
        std::cout << "Not writing to flash (no change in KHz/Period/MaxMHz)\n";)
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
  case Mode::ConfigPeriod:
    Disp.printRaw(Presets.getPeriod());
    break;
  case Mode::Manual:
  case Mode::Uart:
    Disp.printKHz(DC.getKHz());
    break;
  case Mode::ConfigMaxMHz:
    Disp.printMHz(Presets.getMaxMHz());
    break;
  case Mode::ResetToDefaults:
    Disp.printTxt(MsgResetToDefaults);
    break;
  case Mode::Boot: {
    auto Now = std::chrono::system_clock::now();
    auto Seconds =
        std::chrono::duration_cast<std::chrono::seconds>(Now - *ResetTimeOpt);
    int Remaining = ResetMaxSpeedDuration - Seconds.count();
    Disp.printRaw(Remaining);
    break;
  }
  }
#endif
}

void CommonLogic::uartTick(Uart &Uart) {
  auto Bytes = Uart.readNonBlocking();
  auto NumBytes = Bytes.size();
  if (NumBytes == 0)
    return;
  if (NumBytes >= 1) {
    for (char Byte : Bytes) {
      // Skip some illegal chars.
      if (Byte == '\n')
        continue;
      UartStr += Byte;
    }
  }
  DBG_PRINT(std::cout << "UartStr: " << UartStr << "\n";)
  // Reject messages that are too long!
  if (UartStr.size() > MaxUartStrSz) {
    DBG_PRINT(std::cout << "Uart: Too many chars:'" << UartStr << "'\n";)
    printTxtAndSleep(MsgUartErr);
    UartStr.clear();
    return;
  }
  // Look for the EOM character.
  if (UartStr.back() != UartEOM)
    return;
  float MHz = 0;
  int Period = 0;
  sscanf(UartStr.c_str(), "F%fP%d\r", &MHz, &Period);
  int KHz = MHz * 1000;
  DBG_PRINT(std::cout << "UART: "
                      << "MHz=" << MHz << " KHz=" << KHz << " Period=" << Period
                      << "\n";)
  if (KHz == 0) {
    setMode(Mode::Uart);
    printTxtAndSleep(MsgUartMode);
    DC.setMHzToMax();
  } else if ((MHz >= MHzLimitLo && MHz <= MHzLimitHi) &&
             (Period >= PeriodLimitLo && Period <= PeriodLimitHi)) {
    setMode(Mode::Uart);
    DC.setKHz(KHz);
    DC.setPeriod(Period);
    printTxtAndSleep(MsgUartMode);
    // Uart.writeBlockingStr("OK\r\n");
  } else {
    printTxtAndSleep(MsgUartErr);
  }
  UartStr.clear();
}

void CommonLogic::resetSenseTick() {
  if (ResetTimeOpt) {
    auto Now = std::chrono::system_clock::now();
    auto Seconds =
        std::chrono::duration_cast<std::chrono::seconds>(Now - *ResetTimeOpt);
    if (Seconds.count() > ResetMaxSpeedDuration) {
      DBG_PRINT(std::cout << "End of Reset. SvKHz=" << ResetSavedKHz
                          << " SvPeriod=" << ResetSavedPeriod
                          << " SvMode=" << getModeStr(ResetSavedMode) << "\n";)
      ResetTimeOpt = std::nullopt;
      DC.setKHz(ResetSavedKHz);
      DC.setPeriodRaw(ResetSavedPeriod);
      printTxtAndSleep(MsgResetDetected);
      setMode(ResetSavedMode);
    }
  }
  // Button not pressed, skipping.
  if (ResetSense.get() != ButtonState::Release)
    return;
  // Nothing to do if already at max freq.
  if (DC.getKHz() == Presets.getMaxKHz())
    return;
  // If already in booting mode don't save freq/period/mode.
  if (getMode() != Mode::Boot) {
    ResetSavedKHz = DC.getKHz();
    ResetSavedPeriod = DC.getPeriod();
    ResetSavedMode = getMode();
  }
  DBG_PRINT(std::cout << "Reset detected! SvKHz=" << ResetSavedKHz
                      << " SvPeriod=" << ResetSavedPeriod
                      << " SvMode=" << getModeStr(ResetSavedMode) << "\n";)
  DC.setMHzToMax();
  printTxtAndSleep(MsgResetDetected);
  ResetTimeOpt = std::chrono::system_clock::now();
  setMode(Mode::Boot);
}

void CommonLogic::presetBtnsTick() {
  auto HandleBtn = [this](auto &Btn, int BtnIdx, bool IsLast) {
    switch (Btn.get()) {
    case ButtonState::Release:
    case ButtonState::MedRelease:
    case ButtonState::LongPress: {
      DBG_PRINT(std::cout << "PresetBtn " << BtnIdx << " IsLast=" << IsLast
                          << "\n";)
      if (!IsLast)
        Presets.setIdx(BtnIdx);
      else
        // The last preset always sets max frequency.
        Presets.cycleMax();
      int NewKHz = Presets.getActualKHz();
      int NewPeriod = Presets.getPeriod();
      DBG_PRINT(std::cout << "NewKHz=" << NewKHz << " NewPeriod=" << NewPeriod
                          << "\n";)
      DC.setKHz(NewKHz);
      DC.setPeriod(NewPeriod);
      DBG_PRINT(DC.dump();)
      break;
    }
    default:
      break;
    }
  };

  switch (CurrMode) {
  case Mode::Presets:
  case Mode::Manual:{
    for (int BtnIdx = 0, E = (int)PresetBtns.size(); BtnIdx != E; ++BtnIdx)
      HandleBtn(PresetBtns[BtnIdx], BtnIdx, /*IsLast=*/BtnIdx == E - 1);
    break;
  }
  default:
    break;
  }
}
