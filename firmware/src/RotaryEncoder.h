//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __ROTARYENCODER_H__
#define __ROTARYENCODER_H__

#include "Pico.h"
#include "Utils.h"

class RotaryEncoder {
  int ClkPin;
  int DtPin;
  int SwPin;

  static constexpr const unsigned DebounceSz = 2;
  Buffer<bool, DebounceSz, 0> BuffA;
  Buffer<bool, DebounceSz, 0> BuffB;
  Buffer<bool, DebounceSz, 1> BuffC;

public:
  enum class State {
    SwPress,
    SwRelease,
    SwLongPress,
    Left,
    Right,
    None,
  };

private:
  bool LastA = false;
  bool LastB = false;
  bool LastSw = true;
  bool IgnoreRelease = false;

  enum class InternalState {
    A0B0,
    A0B1,
    A1B1,
    A1B0,
  };
  static constexpr const int BuffSz = 4;
  Buffer<InternalState, BuffSz, InternalState::A0B0> IntState;

  Pico &PICO;
  bool ReverseDirection;

  int LongPressCnt = 0;

public:
  RotaryEncoder(int ClkPin, int DtPin, int SwPin, Pico &Pico,
                bool ReverseDirection = false)
      : ClkPin(ClkPin), DtPin(DtPin), SwPin(SwPin), PICO(Pico),
        ReverseDirection(ReverseDirection) {
    PICO.initGPIO(ClkPin, GPIO_IN, Pico::Pull::Up, "Rotary.Clk");
    PICO.initGPIO(DtPin, GPIO_IN, Pico::Pull::Up, "Rotary.Dt");
    PICO.initGPIO(SwPin, GPIO_IN, Pico::Pull::Up, "Rotary.Sw");
  }

  State get() {
    PICO.readGPIO();
    BuffA.append(PICO.getGPIO(ClkPin));
    BuffB.append(PICO.getGPIO(DtPin));
    BuffC.append(PICO.getGPIO(SwPin));
    bool A = BuffA.getMean();
    bool B = BuffB.getMean();
    bool Sw = BuffC.getMean();

    // bool A = PICO.getGPIO(ClkPin);
    // bool B = PICO.getGPIO(DtPin);
    // bool Sw = PICO.getGPIO(SwPin);

    auto GetState = [this](bool A, bool B, bool Sw) {
      if (!Sw && LastSw) {
        LongPressCnt = 0;
        return State::SwPress;
      } else if (Sw && !LastSw) {
        if (!IgnoreRelease)
          return State::SwRelease;
        IgnoreRelease = false;
      } else if (!Sw && !LastSw) {
        if (++LongPressCnt == RotaryLongPressCnt) {
          IgnoreRelease = true;
          return State::SwLongPress;
        }
      } else if (A && !B) {
        if (IntState.back() != InternalState::A1B0)
          IntState.append(InternalState::A1B0);
      } else if (!A && B) {
        if (IntState.back() != InternalState::A0B1)
          IntState.append(InternalState::A0B1);
      } else if (A && B) {
        if (IntState.back() != InternalState::A1B1)
          IntState.append(InternalState::A1B1);
      } else if (!A && !B) {
        if (IntState.back() != InternalState::A0B0)
          IntState.append(InternalState::A0B0);
      }
      return State::None;
    };

    auto RetState = GetState(A, B, Sw);

    LastA = A;
    LastB = B;
    LastSw = Sw;

    if (IntState[0] == InternalState::A0B1 &&
        IntState[1] == InternalState::A0B0 &&
        IntState[2] == InternalState::A1B0 &&
        IntState[3] == InternalState::A1B1) {
      IntState.clear();
      return ReverseDirection ? State::Right : State::Left;
    }
    if (IntState[0] == InternalState::A1B0 &&
        IntState[1] == InternalState::A0B0 &&
        IntState[2] == InternalState::A0B1 &&
        IntState[3] == InternalState::A1B1) {
      IntState.clear();
      return ReverseDirection ? State::Left : State::Right;
    }
    return RetState;
  }
};

#endif // __ROTARYENCODER_H__
