//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "Pico.h"
#include "Utils.h"

enum class ButtonState {
  LongPress,
  MedRelease,
  Release,
  None,
};

static constexpr const char *getButtonState(ButtonState State) {
  switch (State) {
  case ButtonState::LongPress: return "LongPress";
  case ButtonState::MedRelease: return "MedRelease";
  case ButtonState::Release: return "Release";
  case ButtonState::None: return "None";
  }
}

template <bool OffVal, int DebounceSz, int LongPressCntVal,
          int MedReleaseCntVal = LongPressCntVal / 4>
class Button {
  static constexpr const bool OnVal = !OffVal;
  int GPIO;
  Pico &Pi;
  Buffer<bool, DebounceSz, OffVal> Buff;
  bool LastVal = OffVal;
  int LongPressCnt = 0;
  bool IgnoreRelease = false;

public:
  Button(int GPIO, Pico &Pi, const char *Name) : GPIO(GPIO), Pi(Pi) {
    auto Pull = OffVal == true ? Pico::Pull::Up : Pico::Pull::Down;
    Pi.initGPIO(GPIO, GPIO_IN, Pull, Name);
  }
  ButtonState get() {
    Pi.readGPIO();
    Buff.append(Pi.getGPIO(GPIO));
    bool Val = Buff.getMean();

    auto GetState = [this](bool Val) {
      if (Val == OnVal && LastVal == OffVal) {
         LongPressCnt = 0;
      } else if (Val == OffVal && LastVal == OnVal) {
        if (!IgnoreRelease) {
          if (LongPressCnt >= MedReleaseCntVal)
            return ButtonState::MedRelease;
          else
            return ButtonState::Release;
        }
        IgnoreRelease = false;
      } else if (Val == OnVal && LastVal == OnVal) {
        if (++LongPressCnt == LongPressCntVal) {
          IgnoreRelease = true;
          return ButtonState::LongPress;
        }
      }
      return ButtonState::None;
    };
    auto NewState = GetState(Val);
    LastVal = Val;
    return NewState;
  }
};

#endif // __BUTTON_H__
