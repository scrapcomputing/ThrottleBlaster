#include "CommonLogic.h"
#include <cstdio>
#include <string>
struct T : CommonLogic {
  T(Display &D, DutyCycle &DC, PresetsTable &P, FlashStorage &F, Pico &Pi)
      : CommonLogic(1, D, DC, P, F, Pi) {}
  void tick() override {}
  void boot() { CurrMode = Mode::Boot; }
  void manual() { CurrMode = Mode::Manual; }
  void config() { CurrMode = Mode::ConfigMHz; }
};
int Fail = 0, Pass = 0;
static void ok(bool C, const std::string &N) {
  if (C) { ++Pass; return; }
  ++Fail; printf("FAIL %s\n", N.c_str());
}
int main() {
  Pico Pi; Display D; FlashStorage F; PresetsTable P; DutyCycle DC(P);
  T L(D, DC, P, F, Pi); Uart U;
  DC.setKHz(P.getKHz()); DC.setPeriod(P.getPeriod());
  auto Send = [&](const std::string &C) {
    for (char B : C + "\r") U.Rx.push_back(B);
    while (!U.Rx.empty()) L.tickAll(U);
    std::string R = U.Tx; U.Tx.clear();
    if (R.size() >= 2) R.resize(R.size() - 2);
    return R;
  };
  auto Is = [&](const std::string &C, const std::string &E) {
    auto R = Send(C);
    ok(R == E, C + " got '" + R + "' want '" + E + "'");
  };
  const std::string T200 = " MAX=200 MODE=";
  Is("?", "F=200.00 P=8" + T200 + "PRESETS");
  Is("F4.5P16", "OK F=4.50 P=16" + T200 + "UART");
  Is("F10", "OK F=10.00 P=16" + T200 + "UART");
  int W0 = F.Writes;
  Is("W4.77", "OK F=10.00 P=16" + T200 + "PRESETS");
  ok(F.Writes == W0 + 1, "W writes flash once");
  Is("S8", "OK F=8.00 P=8" + T200 + "PRESETS");
  Is("S4.77", "OK F=10.00 P=16" + T200 + "PRESETS");
  W0 = F.Writes;
  for (auto C : {"W200", "D200", "W99", "D99"}) Is(C, "ERR NOPRESET");
  for (auto C : {"W", "W8X", "DX", "D"}) Is(C, "ERR SYNTAX");
  ok(F.Writes == W0, "rejected W/D do not write flash");
  Is("S8", "OK F=8.00 P=8" + T200 + "PRESETS");
  Is("D8", "OK DELETED F=10.00 P=8" + T200 + "PRESETS");
  Is("S8", "ERR NOPRESET");
  Is("W8", "ERR NOPRESET");
  { PresetsTable Q; Q.readFromFlash(F);
    ok(Q.findIdxByKHz(8000) == -1, "delete persisted");
    ok(Q.findIdxByKHz(8000, true) == 1, "deleted still findable");
    Q.setIdx(0);
    ok(Q.getActualKHz() == 10000 && Q.getPeriod() == 16, "tuning persisted"); }
  Is("D8", "OK RESTORED F=8.00 P=8" + T200 + "PRESETS");
  Is("S8", "OK F=8.00 P=8" + T200 + "PRESETS");
  L.boot();
  Is("W33", "ERR BOOT");
  Is("?", "F=8.00 P=8" + T200 + "BOOT");
  L.manual(); DC.setKHz(0);
  Is("W33", "ERR RANGE");
  Is("S33", "OK F=33.00 P=8" + T200 + "PRESETS");
  L.config(); D.Flash = true;
  Is("?", "F=33.00 P=8" + T200 + "CONFIG");
  ok(D.Flash, "query keeps menu flashing");
  Is("Q", "ERR SYNTAX");
  ok(D.Flash, "error keeps menu flashing");
  for (auto C : {"R", "S66", "M200", "F5P8", "F0", "W66", "D25", "X"}) {
    L.config(); D.Flash = true;
    auto R = Send(C);
    ok(R.rfind("OK ", 0) == 0, std::string(C) + " in menu got '" + R + "'");
    ok(!D.Flash, std::string(C) + " clears flashing");
  }
  Is("?", "F=200.00 P=8" + T200 + "PRESETS");
  Is("S4.77", "OK F=4.77 P=8" + T200 + "PRESETS");
  Is("S25", "OK F=25.00 P=8" + T200 + "PRESETS");
  Is("M300", "OK F=300.00 P=8 MAX=300 MODE=PRESETS");
  { PresetsTable Q; Q.readFromFlash(F); ok(Q.getMaxMHz() == 300, "M persisted"); }
  Is("M0", "ERR RANGE");
  Is("F9999", "ERR RANGE");
  Is("F1P1", "ERR PERIOD");
  Is("s33", "OK F=33.00 P=8 MAX=300 MODE=PRESETS");
  Is("F12345678901234567", "ERR TOOLONG");
  Is("?", "F=33.00 P=8 MAX=300 MODE=PRESETS");
  // Null bytes, like those some serial ports send when opened, are ignored.
  Is(std::string(2, '\0'), "");
  Is(std::string(2, '\0') + "?", "F=33.00 P=8 MAX=300 MODE=PRESETS");
  printf("passed=%d failed=%d\n", Pass, Fail);
  return Fail != 0;
}
