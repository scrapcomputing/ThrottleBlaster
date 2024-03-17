//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing

#ifndef __PRESETS_H__
#define __PRESETS_H__

#include "ConfigOpts.h"
#include "Flash.h"
#include <algorithm>
#include <iostream>
#include <vector>

class PresetsTable {
  int MaxMHz = DefaultMaxMHz;

  struct Entry {
    int PresetKHz;
    int ActualKHz;
    int PWM;
    Entry(int PresetKHz, int ActualKHz, int PWM)
        : PresetKHz(PresetKHz), ActualKHz(ActualKHz), PWM(PWM) {}
    Entry(FlashStorage &Flash, int &Offset) {
      PresetKHz = Flash.read(Offset++);
      ActualKHz = Flash.read(Offset++);
      PWM = Flash.read(Offset++);
    }
    void dump() const {
      std::cout << PresetKHz << ", " << ActualKHz << ", " << PWM << "\n";
    }
    /// Helper for writing to Flash.
    void appendToVec(std::vector<int> &Vec) const {
      Vec.push_back(PresetKHz);
      Vec.push_back(ActualKHz);
      Vec.push_back(PWM);
    }
    unsigned checksum() const {
      return (unsigned)PresetKHz ^ (unsigned)ActualKHz ^ (unsigned)PWM;
    }
  };

  static constexpr const int DefaultPeriod = 8; // ~50us

  // const std::vector<Entry> ImmutableTable = {
  //     {4770, 4770, DefaultPWM},     {8000, 8000, DefaultPWM},
  //     {10000, 10000, DefaultPWM},   {25000, 25000, DefaultPWM},
  //     {33000, 33000, DefaultPWM},   {66000, 66000, DefaultPWM},
  //     {100000, 100000, DefaultPWM}, {133000, 133000, DefaultPWM},
  //     {266000, 266000, DefaultPWM}, {450000, 450000, DefaultPWM},
  //     {733000, 733000, DefaultPWM}, {1000000, 1000000, DefaultPWM},
  //     {1666000, 1666000, DefaultPWM}, {3200000, 3200000, DefaultPWM}};
  const std::vector<Entry> ImmutableTable = {
      {4770, 4770, DefaultPeriod},     {8000, 8000, DefaultPeriod},
      {33000, 33000, DefaultPeriod},   {66000, 66000, DefaultPeriod},
      {133000, 133000, DefaultPeriod}, {450000, 450000, DefaultPeriod},
      {733000, 733000, DefaultPeriod}};

  std::vector<Entry> Table;

  int Idx;

  int maxKHz() const { return Table.back().PresetKHz; }
  int checksum(int MHz, const std::vector<Entry> &Entries) const;
  int getMaxIdx() const { return (int)Table.size() - 1; }

public:
  PresetsTable() { updateMaxMHz(MaxMHz); }
  void dump() const;
  int getMaxMHz() const { return MaxMHz; }
  int getMaxKHz() const { return MaxMHz * 1000; }
  int getMinKHz() const { return Table[0].PresetKHz; }
  int getKHz() const { return Table[Idx].PresetKHz; }
  bool atMaxKHz() const { return getMaxKHz() == getKHz(); }
  int getActualKHz() const { return Table[Idx].ActualKHz; }
  void incrActualKHz();
  void decrActualKHz();
  int getPWM() const { return Table[Idx].PWM; }
  void incrPWM();
  void decrPWM();
  void prev() { Idx = std::max(0, Idx - 1); }
  void next() { Idx = std::min(getMaxIdx(), Idx + 1); }
  void cyclePrev();
  void cycleNext();
  void cycleMax();
  void updateMaxMHz(int NewMaxMHz);
  void incrMaxMHz(int Step = 1, bool Wrap = false, int WrapLo = MHzLimitLo,
                  int WrapHi = MHzLimitHi);
  void decrMaxMHz(int Step = 1);
  void writeToFlash(FlashStorage &Flash) const;
  void readFromFlash(FlashStorage &Flash);
  void resetToDefaults(FlashStorage &Flash);
};

#endif // __PRESETS_H__
