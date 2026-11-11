#pragma once
#include <cstdint>
#include <deque>
#include <string>
#include <vector>
class Uart {
public:
  std::deque<uint8_t> Rx;
  std::string Tx;
  void writeBlockingStr(const std::string &S) { Tx += S; }
  std::vector<uint8_t> readNonBlocking() {
    if (Rx.empty()) return {};
    uint8_t B = Rx.front(); Rx.pop_front();
    return {B};
  }
};
