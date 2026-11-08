#pragma once
#include "Pico.h"
#include <deque>
class RotaryEncoder {
public:
  enum class State { SwPress, SwRelease, SwLongPress, Left, Right, None };
  RotaryEncoder(int, int, int, Pico &, bool) {}
  State get();
};
inline std::deque<RotaryEncoder::State> RotQ;
inline RotaryEncoder::State RotaryEncoder::get() {
  if (RotQ.empty()) return State::None;
  auto S = RotQ.front(); RotQ.pop_front();
  return S;
}
