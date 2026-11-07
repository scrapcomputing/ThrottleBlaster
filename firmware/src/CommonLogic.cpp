#include "CommonLogic.h"
#include "Debug.h"
#include "pico/time.h"
#include <cctype>
#include <cmath>
#include <cstdio>
#include <string>

void CommonLogic::setMode(Mode NewMode) {
  setModeInit(NewMode);

  CurrMode = NewMode;
  DBG_PRINT(std::cout << getModeStr(CurrMode) << "\n";)
}

void CommonLogic::tryWritePresetsToFlash() {
  DBG_PRINT(std::cout << "WriteToFlash:\n";)
  DBG_PRINT(std::cout << "           Before After\n";)
  DBG_PRINT(std::cout << "ActualKHz: " << Presets.getActualKHz() << "\n";)
  DBG_PRINT(std::cout << "Period:       " << Presets.getPeriod() << "\n";)
  DBG_PRINT(std::cout << "MaxMHz:    " << Presets.getMaxMHz() << "\n";)
  DBG_PRINT(std::cout << "Deleted:    " << Presets.isDeleted() << "\n";)
  Presets.writeToFlash(Flash);
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
  case Mode::DeletePreset:
    Disp.printTxt(Presets.isDeleted() ? MsgYes : MsgNo);
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
    if (Seconds.count() == 0) {
      Disp.printTxt(MsgResetDetected);
    } else {
      int Remaining = ResetMaxSpeedDuration - Seconds.count();
      Disp.printRaw(Remaining);
    }
    break;
  }
  }
#endif
}

void CommonLogic::sendUartStatus(Uart &Uart, const char *Prefix) {
  const char *ModeStr = "CONFIG";
  switch (getMode()) {
  case Mode::Presets: ModeStr = "PRESETS"; break;
  case Mode::Manual: ModeStr = "MANUAL"; break;
  case Mode::Uart: ModeStr = "UART"; break;
  case Mode::Boot: ModeStr = "BOOT"; break;
  default: break;
  }
  int KHz = DC.getKHz();
  // DC stores the raw PIO period (256 * Period - 1).
  int Period = (DC.getPeriod() + 1) / 256;
  char Buff[64];
  snprintf(Buff, sizeof(Buff), "%sF=%d.%02d P=%d MAX=%d MODE=%s\r\n", Prefix,
           KHz / 1000, (KHz % 1000) / 10, Period, Presets.getMaxMHz(),
           ModeStr);
  Uart.writeBlockingStr(Buff);
}

void CommonLogic::handleUartCmd(Uart &Uart, const std::string &Cmd) {
  auto Error = [this, &Uart](const char *Reason) {
    DBG_PRINT(std::cout << "Uart: ERR " << Reason << "\n";)
    printTxtAndSleep(MsgUartErr);
    Uart.writeBlockingStr(std::string("ERR ") + Reason + "\r\n");
  };
  const char *Str = Cmd.c_str();

  // ? : Query. Reply immediately, nothing changes.
  if (Cmd == "?") {
    sendUartStatus(Uart, "");
    return;
  }

  // R : Return to the presets, like a short push in Manual mode.
  if (Cmd == "R") {
    setMode(Mode::Presets);
    DC.setKHz(Presets.getActualKHz());
    DC.setPeriod(Presets.getPeriod());
    printTxtAndSleep(MsgPresets);
    sendUartStatus(Uart, "OK ");
    return;
  }

  // SMAX : Select the max frequency preset, like the last preset button.
  // S<MHz> : Select the preset with nominal frequency <MHz>, using its tuned
  //          frequency and period, like the preset buttons.
  if (Cmd[0] == 'S') {
    if (Cmd == "SMAX") {
      applyPreset(0, /*IsLast=*/true);
    } else {
      float MHz = 0;
      int Len = 0;
      if (sscanf(Str, "S%f%n", &MHz, &Len) != 1 || Str[Len] != '\0')
        return Error("SYNTAX");
      int Idx = Presets.findIdxByKHz((int)lroundf(MHz * 1000));
      if (Idx < 0)
        return Error("NOPRESET");
      applyPreset(Idx, /*IsLast=*/false);
    }
    setMode(Mode::Presets);
    printTxtAndSleep(MsgPresets);
    sendUartStatus(Uart, "OK ");
    return;
  }

  // M<MHz> : Set the maximum CPU frequency and save it to flash, like
  //          confirming the CPUF setting.
  if (Cmd[0] == 'M') {
    int MaxMHz = 0;
    int Len = 0;
    if (sscanf(Str, "M%d%n", &MaxMHz, &Len) != 1 || Str[Len] != '\0')
      return Error("SYNTAX");
    if (MaxMHz < MHzLimitLo || MaxMHz > MHzLimitHi)
      return Error("RANGE");
    // This also selects the max frequency preset.
    Presets.updateMaxMHz(MaxMHz);
    DC.setKHz(Presets.getActualKHz());
    DC.setPeriod(Presets.getPeriod());
    tryWritePresetsToFlash();
    setMode(Mode::Presets);
    printTxtAndSleep(MsgConfirm);
    sendUartStatus(Uart, "OK ");
    return;
  }

  // F<MHz>[P<Period>] : Set a raw frequency. F0 sets the max frequency.
  //                     If P is missing, the current period is kept.
  if (Cmd[0] == 'F') {
    float MHz = 0;
    int Len = 0;
    if (sscanf(Str, "F%f%n", &MHz, &Len) != 1)
      return Error("SYNTAX");
    const char *Rest = Str + Len;
    int Period = (DC.getPeriod() + 1) / 256;
    bool HasPeriod = false;
    if (*Rest == 'P') {
      int PLen = 0;
      if (sscanf(Rest, "P%d%n", &Period, &PLen) != 1)
        return Error("SYNTAX");
      Rest += PLen;
      HasPeriod = true;
    }
    if (*Rest != '\0')
      return Error("SYNTAX");
    int KHz = (int)lroundf(MHz * 1000);
    if (HasPeriod && (Period < PeriodLimitLo || Period > PeriodLimitHi))
      return Error("RANGE");
    if (KHz == 0) {
      setMode(Mode::Uart);
      printTxtAndSleep(MsgUartMode);
      DC.setMHzToMax();
      sendUartStatus(Uart, "OK ");
      return;
    }
    if (MHz < MHzLimitLo || KHz > Presets.getMaxKHz())
      return Error("RANGE");
    // Reject combinations where the CPU would never run (it would freeze).
    int RawPeriod = (1 << 8) * Period - 1;
    if ((int64_t)KHz * RawPeriod / Presets.getMaxKHz() == 0)
      return Error("PERIOD");
    setMode(Mode::Uart);
    DC.setKHz(KHz);
    DC.setPeriod(Period);
    printTxtAndSleep(MsgUartMode);
    sendUartStatus(Uart, "OK ");
    return;
  }

  Error("SYNTAX");
}

void CommonLogic::uartTick(Uart &Uart) {
  auto Bytes = Uart.readNonBlocking();
  if (Bytes.empty())
    return;
  for (char Byte : Bytes) {
    // Skip some illegal chars.
    if (Byte == '\n')
      continue;
    if (Byte != UartEOM) {
      if (!UartDiscard)
        UartStr += (char)toupper((unsigned char)Byte);
      // Reject messages that are too long!
      if (UartStr.size() > MaxUartStrSz) {
        DBG_PRINT(std::cout << "Uart: Too many chars:'" << UartStr << "'\n";)
        UartStr.clear();
        UartDiscard = true;
      }
      continue;
    }
    // Got the EOM character.
    DBG_PRINT(std::cout << "UartStr: " << UartStr << "\n";)
    if (UartDiscard) {
      printTxtAndSleep(MsgUartErr);
      Uart.writeBlockingStr("ERR TOOLONG\r\n");
    } else if (!UartStr.empty()) {
      handleUartCmd(Uart, UartStr);
    }
    UartStr.clear();
    UartDiscard = false;
  }
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
      setMode(ResetSavedMode);
    }
  }
  // Button not pressed, skipping.
  if (ResetSense.get() != ButtonState::Pressed)
    return;
  // Nothing to do if already at max freq.
  if (DC.getKHz() == Presets.getMaxKHz() && !ResetTimeOpt)
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
  ResetTimeOpt = std::chrono::system_clock::now();
  setMode(Mode::Boot);
}

void CommonLogic::applyPreset(int Idx, bool IsLast) {
  if (!IsLast)
    Presets.setIdx(Idx);
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
}

void CommonLogic::presetBtnsTick() {
  auto HandleBtn = [this](auto &Btn, int BtnIdx, bool IsLast) {
    switch (Btn.get()) {
    case ButtonState::Release:
    case ButtonState::MedRelease:
    case ButtonState::LongPress:
      DBG_PRINT(std::cout << "PresetBtn " << BtnIdx << " IsLast=" << IsLast
                          << "\n";)
      applyPreset(BtnIdx, IsLast);
      break;
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
