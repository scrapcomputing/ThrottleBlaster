//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#include "ConfigOpts.h"
#include "RotaryEncoder.h"

class RotaryEncoder;

class RotaryEncoder *RE;

/// Helps skip multicore_lockout_start when core 1 is not running.
bool Core1Enabled = false;

static void core1_tick() {
  Core1Enabled = true;
  // We need to be able to stop the core when writing to Flash.
  multicore_lockout_victim_init();

  while (true) {
    RE->tick();
  }
}

RotaryEncoder::RotaryEncoder(int ClkPin, int DtPin, int SwPin, Pico &Pico,
                             bool ReverseDirection)
    : ClkPin(ClkPin), DtPin(DtPin), SwPin(SwPin), PICO(Pico),
      ReverseDirection(ReverseDirection) {
  PICO.initGPIO(ClkPin, GPIO_IN, Pico::Pull::Up, "Rotary.Clk");
  PICO.initGPIO(DtPin, GPIO_IN, Pico::Pull::Up, "Rotary.Dt");
  PICO.initGPIO(SwPin, GPIO_IN, Pico::Pull::Up, "Rotary.Sw");

  critical_section_init(&Lock);

  // Use the second core for sampling the GPIOs.
  RE = this;
  multicore_launch_core1(core1_tick);
}

void RotaryEncoder::registerState(RotaryEncoder::State TheState) {
  if (TheState != State::None) {
    critical_section_enter_blocking(&Lock);
    static constexpr const uint LIST_SZ_LIMIT = 16;
    if (StateList.size() > LIST_SZ_LIMIT) {
      StateList.pop_front();
      std::cout << "WARNING: List Size Limit!!!\n";
    }
    StateList.push_back(TheState);
    DBG_PRINT(std::cout << "registerState: " << stateToStr(TheState) << "\n";)
    critical_section_exit(&Lock);
  }
}

void RotaryEncoder::tick() {
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

  State TheState = State::None;
  if (!Sw && LastSw) {
    PressStart = std::chrono::high_resolution_clock::now();
    TheState = State::SwPress;
  } else if (Sw && !LastSw) {
    if (!IgnoreRelease)
      TheState = State::SwRelease;
    IgnoreRelease = false;
  } else if (!Sw && !LastSw) {
    auto Now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> Diff = Now - PressStart;
    int DiffMillis = Diff.count() * 1024;
    if (DiffMillis > RotaryLongPressMillis) {
      IgnoreRelease = true;
      TheState = State::SwLongPress;
      PressStart = Now;
    }
  }
  // Push button has higher priority than rotary.
  if (TheState == State::None) {
    if (A && !B) {
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
  }

  LastA = A;
  LastB = B;
  LastSw = Sw;

  if (IntState[0] == InternalState::A0B1 &&
      IntState[1] == InternalState::A0B0 &&
      IntState[2] == InternalState::A1B0 &&
      IntState[3] == InternalState::A1B1) {
    IntState.clear();
    TheState = ReverseDirection ? State::Right : State::Left;
  }
  else if (IntState[0] == InternalState::A1B0 &&
      IntState[1] == InternalState::A0B0 &&
      IntState[2] == InternalState::A0B1 &&
      IntState[3] == InternalState::A1B1) {
    IntState.clear();
    TheState = ReverseDirection ? State::Left : State::Right;
  }
  registerState(TheState);
  return;
}


RotaryEncoder::State RotaryEncoder::get() {
  State RetState = State::None;
  critical_section_enter_blocking(&Lock);
  if (!StateList.empty()) {
    RetState = StateList.back();
    StateList.pop_back();
  }
  critical_section_exit(&Lock);
  return RetState;
}
