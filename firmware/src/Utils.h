//-*- C++ -*-
//
// Copyright (C) 2024 Scarp Computing
//

#ifndef __SRC_UTILS_H__
#define __SRC_UTILS_H__

#include <cstdint>
#include <iostream>
#include <vector>

#define DUMP_METHOD __attribute__((noinline)) __attribute__((__used__))

static inline void sleep_nops(int nops) {
  for (int i = 0; i != nops; ++i)
    asm volatile("nop:"); /* 8ns each 1 cycle @125MHz */
}

#define sleep_80ns()                                                           \
  asm volatile("nop\n");                                                        \
  asm volatile("nop\n");                                                        \
  asm volatile("nop\n");                                                        \
  asm volatile("nop\n");                                                        \
  asm volatile("nop\n");                                                        \
  asm volatile("nop\n");                                                        \
  asm volatile("nop\n");                                                        \
  asm volatile("nop\n");                                                        \
  asm volatile("nop\n");                                                        \
  asm volatile("nop\n");

#define sleep_800ns()                                                          \
  sleep_80ns();                                                                \
  sleep_80ns();                                                                \
  sleep_80ns();                                                                \
  sleep_80ns();                                                                \
  sleep_80ns();                                                                \
  sleep_80ns();                                                                \
  sleep_80ns();                                                                \
  sleep_80ns();                                                                \
  sleep_80ns();                                                                \
  sleep_80ns();

template <typename T>
__attribute__((noreturn)) static inline void dieBase(const T &Val) {
  std::cerr << Val << "\n";
  exit(1);
}

/// Print arguments and exit with exit code 1.
template <typename T, typename... Ts>
__attribute__((noreturn)) static inline void dieBase(const T &Val1,
                                                     const Ts... Vals) {
  std::cerr << Val1;
  dieBase(Vals...);
}

/// Exit the program, reporting a bug.
#define die(...)                                                               \
  dieBase("Error in ", __FILE__, ":", __LINE__, " ", __FUNCTION__,             \
          "(): ", __VA_ARGS__, "\n")

#define unreachable(...)                        \
  dieBase("Unreachable in ", __FILE__, ":", __LINE__, " ", __FUNCTION__, \
          "(): ", __VA_ARGS__, "\n")

/// Holds the last several entries.
/// Can return the mean.
template<typename T, unsigned Sz, T InitVal>
class Buffer {
  std::vector<T> Vec;

public:
  Buffer() {
    Vec.resize(Sz);
    for (unsigned Cnt = 0; Cnt != Sz; ++Cnt)
      Vec[Cnt] = InitVal;
  }
  void append(T Val) {
    // Shift left
    for (unsigned Cnt = 0; Cnt + 1 < Sz; ++Cnt)
      Vec[Cnt] = Vec[Cnt + 1];
    Vec[Sz - 1] = Val;
  }
  T getMean() const {
    int Sum = 0;
    int CurrSz = 0;
    for (int Val : Vec) {
      Sum += Val;
      CurrSz += 1;
    }
    return (T)(Sum / CurrSz);
  }
  T operator[](unsigned Idx) const { return Vec[Idx]; }
  T back() const { return Vec.back(); }
  void clear() { std::fill(Vec.begin(), Vec.end(), InitVal); }
};

#endif // __SRC_UTILS_H__
