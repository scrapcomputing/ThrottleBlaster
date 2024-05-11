//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#ifndef __POTENTIOMETER_H__
#define __POTENTIOMETER_H__

#include "Pico.h"
#include "Utils.h"

template <int MaxVal, int DenoiseBuffSz, int DeadZone> class Potentiometer {
  Buffer<int, DenoiseBuffSz, 0> MeanADCVals;
  int GPIO;
  Pico &Pi;
  bool ReverseDirection;

public:
  Potentiometer(int GPIO, Pico &Pi, bool ReverseDirection = false,
                const char *Name = "Potentiometer")
      : GPIO(GPIO), Pi(Pi), ReverseDirection(ReverseDirection) {
    Pi.initADC(GPIO, Name);
  }
  static constexpr const int getMax() { return MaxVal; }
  int get() {
    MeanADCVals.append(Pi.readADC(GPIO));
    int ADCVal = MeanADCVals.getMean();
    int Val = std::min(MaxVal, std::max(0, ADCVal - DeadZone) * MaxVal /
                                   (Pico::ADCMax - 2 * DeadZone));
    return !ReverseDirection ? Val : MaxVal - Val;
  }
};

#endif // __POTENTIOMETER_H__
