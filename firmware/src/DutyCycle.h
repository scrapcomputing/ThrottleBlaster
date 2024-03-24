//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing

#ifndef __DUTYCYCLE_H__
#define __DUTYCYCLE_H__

#include <algorithm>
#include <cstdint>
#include "Presets.h"
#include "ConfigOpts.h"

class DutyCycle {
  int Period = 0;
  int KHz = 0;
  const PresetsTable &Presets;

public:
  DutyCycle(PresetsTable &Presets)
      : Period(1u << 12), KHz(Presets.getMaxKHz()), Presets(Presets) {}
  void setMHzToMax() { setKHz(Presets.getMaxKHz()); }
  void incrMHz() {
    int Step = 1000;
    int NextKHz = (KHz / 1000) * 1000 + Step;
    setKHz(std::min(Presets.getMaxKHz(), NextKHz));
  }
  void decrMHz() {
    int Step = 1000;
    int PrevKHz = (KHz / 1000) * 1000 - Step;
    setKHz(std::max(0, PrevKHz));
  }
  int getMHz() const { return (int64_t)KHz * 1000; }
  int getKHz() const { return KHz; }
  void setKHz(int NewKHz) { KHz = NewKHz; }
  void setPeriod(int Num) { Period = (1u << 8) * Num - 1; }
  int getPeriod() const { return Period; }
  int getLevel() const {
    return Period - (int64_t)KHz * Period / Presets.getMaxKHz();
  }
  int getMaxLevel() const { return Period; }
  void dump() const {
    std::cout << "KHz=" << KHz << " Level=" << getLevel()
              << " Period=" << getPeriod() << "\n";
  }
};

#endif // __DUTYCYCLE_H__
