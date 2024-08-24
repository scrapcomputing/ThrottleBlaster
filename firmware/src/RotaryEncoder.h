//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __ROTARYENCODER_H__
#define __ROTARYENCODER_H__

#include "Debug.h"
#include "Pico.h"
#include "Utils.h"
#include <chrono>
#include <list>
#include <pico/multicore.h>

class RotaryEncoder {
  int ClkPin;
  int DtPin;
  int SwPin;

  static constexpr const unsigned DebounceSz = 4;
  static constexpr const unsigned SwitchDebounceSz = 16;
  Buffer<bool, DebounceSz, 0> BuffA;
  Buffer<bool, DebounceSz, 0> BuffB;
  Buffer<bool, SwitchDebounceSz, 1> BuffC;

public:
  enum class State {
    SwPress,
    SwRelease,
    SwLongPress,
    Left,
    Right,
    None,
  };

#ifdef DBGPRINT
  static const char *stateToStr(State S) {
    switch (S) {
    case State::SwPress:
      return "SwPress";
    case State::SwRelease:
      return "SwRelease";
    case State::SwLongPress:
      return "SwLongPress";
    case State::Left:
      return "Left";
    case State::Right:
      return "Right";
    case State::None:
      return "None";
    }
    return "UNKNOWN";
  }
#endif // DBGPRINT

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

  std::chrono::time_point<std::chrono::high_resolution_clock> PressStart;

  critical_section Lock;

  std::list<State> StateList;

  void registerState(State);

public:
  RotaryEncoder(int ClkPin, int DtPin, int SwPin, Pico &Pico,
                bool ReverseDirection = false);

  void tick();

  State get();
};

#endif // __ROTARYENCODER_H__
