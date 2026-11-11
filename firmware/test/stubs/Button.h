#pragma once
#include "Pico.h"
#include <deque>
#include <map>
enum class ButtonState { LongPress, MedRelease, Release, Pressed, None };
bool bothRelease(ButtonState BS1, ButtonState BS2);
bool bothLongPress(ButtonState BS1, ButtonState BS2);
inline std::map<int, std::deque<ButtonState>> BtnQ;
template <bool O, int D, int L, int M> class Button {
  int GPIO;
public:
  Button(int G, Pico &, const char *) : GPIO(G) {}
  ButtonState get() {
    auto &Q = BtnQ[GPIO];
    if (Q.empty()) return ButtonState::None;
    auto S = Q.front(); Q.pop_front();
    return S;
  }
};
