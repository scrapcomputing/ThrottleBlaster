//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing

#include "Presets.h"
#include "Debug.h"
#include "Flash.h"

void PresetsTable::dump() const {
  for (const auto &E : Table)
    E.dump();
  std::cout << "\n";
}

void PresetsTable::incrActualKHz() {
  int &Val = Table[Idx].ActualKHz;
  Val = std::min(maxKHz(), Val + 1000);
}

void PresetsTable::decrActualKHz() {
  int &Val = Table[Idx].ActualKHz;
  Val = std::max(1000, Val - 1000);
}

void PresetsTable::incrPeriod() {
  int &Val = Table[Idx].Period;
  Val = std::min(PeriodLimitHi, Val + 1);
}

void PresetsTable::decrPeriod() {
  int &Val = Table[Idx].Period;
  Val = std::max(PeriodLimitLo, Val - 1);
}

void PresetsTable::cyclePrev() {
  if (Idx == 0)
    Idx = getMaxIdx();
  else
    prev();
}

void PresetsTable::cycleNext() {
  if (Idx == getMaxIdx())
    Idx = 0;
  else
    next();
}

void PresetsTable::cycleMax() { Idx = getMaxIdx(); }

void PresetsTable::updateMaxMHz(int NewMaxMHz) {
  MaxMHz = NewMaxMHz;
  int MaxKHz = MaxMHz * 1000;
  // If the last entry is not in the immutable table, then it is a custom
  // MaxMHz entry. Drop it as we are going to replace it with a new one.
  if (!Table.empty()) {
    int TableIdx = Table.size() - 1;
    if (Table.size() > ImmutableTable.size() ||
        Table.back().PresetKHz != ImmutableTable[TableIdx].PresetKHz)
      Table.pop_back();
  }
  // Drop Table entries with KHz > MaxKHz
  while (!Table.empty() && Table.back().PresetKHz > MaxKHz)
    Table.pop_back();
  // Copy any missing entries from ImmutableMHzTable to MHzTable.
  for (const auto &Entry : ImmutableTable) {
    if ((Table.empty() || Entry.PresetKHz > Table.back().PresetKHz) &&
        Entry.PresetKHz <= MaxKHz)
      Table.push_back(Entry);
  }
  // Add a custom entry with the new Max value if required.
  if (Table.empty() || Table.back().PresetKHz != MaxKHz)
    Table.emplace_back(MaxKHz, MaxKHz, DefaultPeriod);
  // Adjust the Idx to point to the last entry.
  Idx = (int)Table.size() - 1;
  // dump();
}

void PresetsTable::incrMaxMHz(int Step, bool Wrap, int WrapLo, int WrapHi) {
  int NewMax;
  if (Wrap && getMaxMHz() == WrapHi)
    NewMax = WrapLo;
  else
    NewMax = std::min(WrapHi, getMaxMHz() + Step);
  updateMaxMHz(NewMax);
}

void PresetsTable::decrMaxMHz(int Step) {
  int NewMax = std::max(MHzLimitLo, getMaxMHz() - Step);
  updateMaxMHz(NewMax);
}

void PresetsTable::writeToFlash(FlashStorage &Flash) const {
  std::vector<int> FlashValues;
  // First write MaxMHz.
  FlashValues.push_back(MaxMHz);
  // Next the size of Table.
  FlashValues.push_back((int)Table.size());
  // Next the entries one by one.
  for (const auto &Entry : Table)
    Entry.appendToVec(FlashValues);
  // Finally add a checksum.
  FlashValues.push_back(checksum(MaxMHz, Table));

  DBG_PRINT(std::cout << "Presets: Writing to Flash\n";)
  Flash.write(FlashValues);
  DBG_PRINT(std::cout << "Presets: Done writing to Flash\n";)
}

int PresetsTable::checksum(int MHz, const std::vector<Entry> &Entries) const {
  unsigned Checksum = 0;
  Checksum ^= MHz;
  for (const auto &E : Entries)
    Checksum ^= E.checksum();
  return (int)Checksum;
}

void PresetsTable::readFromFlash(FlashStorage &Flash) {
  if (!Flash.valid())
    return;
  DBG_PRINT(std::cout << "Presets: Reading from Flash\n";)
  int FlashOffset = 0;
  // 1. First read MaxMHz.
  int FlashMaxMHz = Flash.read(FlashOffset++);
  if (FlashMaxMHz > MHzLimitHi || FlashMaxMHz < MHzLimitLo) {
    DBG_PRINT(std::cout << "Bad FlashMaxMHz=" << FlashMaxMHz << "\n";)
    return;
  }
  updateMaxMHz(FlashMaxMHz);
  // 2. Next read the size of Table.
  int TableSz = Flash.read(FlashOffset++);
  if (TableSz > ImmutableTable.size() + 1) {
    DBG_PRINT(std::cout << "Bad TableSz=" << TableSz << "\n";)
    return;
  }
  // 3. Next read the entries one by one.
  std::vector<Entry> FlashTable;
  for (int EntryIdx = 0; EntryIdx != TableSz; ++EntryIdx)
    FlashTable.emplace_back(Flash, FlashOffset);
  // 4. Check checksum.
  int ReadChecksum = Flash.read(FlashOffset++);
  int ExpectedChecksum = checksum(FlashMaxMHz, FlashTable);
  if (ReadChecksum != ExpectedChecksum) {
    DBG_PRINT(std::cout << "Checksum error: Got:" << ReadChecksum
                        << " Expected:" << ExpectedChecksum << "\n";)
    return;
  }
  Table = std::move(FlashTable);
  DBG_PRINT(std::cout << "Presets: Done reading from Flash\n";)
  DBG_PRINT(dump();)
}

void PresetsTable::resetToDefaults(FlashStorage &Flash) {
  DBG_PRINT(std::cout << "Presets: Reset to defaults...\n";)
  Table = ImmutableTable;
  updateMaxMHz(DefaultMaxMHz);
  writeToFlash(Flash);
}
