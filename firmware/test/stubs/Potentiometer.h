#pragma once
#include "Pico.h"
inline int PotVal = 0;
template <int MaxVal, int D, int Z> class Potentiometer {
public:
  Potentiometer(int, Pico &, bool = false) {}
  static constexpr int getMax() { return MaxVal; }
  int get() { return PotVal; }
};
