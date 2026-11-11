#pragma once
#include <vector>
class FlashStorage {
  std::vector<int> V;
public:
  int Writes = 0;
  void write(const std::vector<int> &N) { V = N; ++Writes; }
  int read(int I) const { return V[I]; }
  bool valid() const { return !V.empty(); }
};
