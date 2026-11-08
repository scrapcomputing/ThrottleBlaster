// Runs the real front panel logic classes together with serial commands.
#include "PotentiometerLogic.h"
#include "RotaryLogic.h"
#include "TwoButtonLogic.h"
#include <cstdio>
#include <string>
int Fail = 0, Pass = 0;
struct Rig {
  Pico Pi; Display D; FlashStorage F; PresetsTable P; DutyCycle DC{P}; Uart U;
  void boot() { DC.setKHz(P.getKHz()); DC.setPeriod(P.getPeriod()); }
};
static void ok(bool C, const std::string &N) {
  if (C) { ++Pass; return; }
  ++Fail; printf("FAIL %s\n", N.c_str());
}
template <class L> static std::string send(L &Ui, Rig &R, const std::string &C) {
  for (char B : C + "\r") R.U.Rx.push_back(B);
  while (!R.U.Rx.empty()) Ui.tickAll(R.U);
  std::string Out = R.U.Tx; R.U.Tx.clear();
  if (Out.size() >= 2) Out.resize(Out.size() - 2);
  return Out;
}
template <class L>
static void is(const char *Tag, L &Ui, Rig &R, const std::string &C, const std::string &E) {
  auto Got = send(Ui, R, C);
  ok(Got == E, std::string(Tag) + ": " + C + " got [" + Got + "] want [" + E + "]");
}
using RS = RotaryEncoder::State;
using BS = ButtonState;
static const std::string M200 = " MAX=200 MODE=";

static void rotary() {
  const char *N = "rotary";
  Rig R; RotaryLogic Ui(2, 3, 4, 1, R.Pi, R.D, R.P, R.DC, R.F, false); R.boot();
  auto Ev = [&](RS S) { RotQ.push_back(S); Ui.tickAll(R.U); };
  is(N, Ui, R, "?", "F=200.00 P=8" + M200 + "PRESETS");
  Ev(RS::Left);
  is(N, Ui, R, "?", "F=133.00 P=8" + M200 + "PRESETS");
  Ev(RS::SwLongPress);
  is(N, Ui, R, "?", "F=133.00 P=8" + M200 + "CONFIG");
  ok(R.D.Flash, "rotary: menu flashes");
  is(N, Ui, R, "S33", "OK F=33.00 P=8" + M200 + "PRESETS");
  ok(!R.D.Flash, "rotary: S closes menu");
  Ev(RS::Right);
  is(N, Ui, R, "?", "F=66.00 P=8" + M200 + "PRESETS");
  Ev(RS::SwRelease);
  is(N, Ui, R, "?", "F=66.00 P=8" + M200 + "MANUAL");
  Ev(RS::Right);
  is(N, Ui, R, "?", "F=67.00 P=8" + M200 + "MANUAL");
  is(N, Ui, R, "R", "OK F=66.00 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "F10P16", "OK F=10.00 P=16" + M200 + "UART");
  Ev(RS::Left);
  is(N, Ui, R, "?", "F=66.00 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "S10", "OK F=10.00 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "D8", "OK DELETED F=10.00 P=8" + M200 + "PRESETS");
  Ev(RS::Left);
  is(N, Ui, R, "?", "F=4.77 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "D8", "OK RESTORED F=8.00 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "F24P16", "OK F=24.00 P=16" + M200 + "UART");
  is(N, Ui, R, "W25", "OK F=24.00 P=16" + M200 + "PRESETS");
  is(N, Ui, R, "S33", "OK F=33.00 P=8" + M200 + "PRESETS");
  Ev(RS::Left);
  is(N, Ui, R, "?", "F=24.00 P=16" + M200 + "PRESETS");
  is(N, Ui, R, "SMAX", "OK F=200.00 P=8" + M200 + "PRESETS");
  Ev(RS::SwLongPress);
  ok(R.D.Flash, "rotary: max menu flashes");
  is(N, Ui, R, "M300", "OK F=300.00 P=8 MAX=300 MODE=PRESETS");
  ok(!R.D.Flash, "rotary: M closes menu");
  Ev(RS::Left);
  is(N, Ui, R, "?", "F=133.00 P=8 MAX=300 MODE=PRESETS");
  is(N, Ui, R, "X", "OK F=200.00 P=8" + M200 + "PRESETS");
  Ev(RS::Left);
  is(N, Ui, R, "?", "F=133.00 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "S25", "OK F=25.00 P=8" + M200 + "PRESETS");
}

static void twoButton() {
  const char *N = "twobutton";
  BtnQ.clear();
  Rig R; TwoButtonLogic Ui(4, 7, 1, R.Pi, R.D, R.DC, R.P, R.F); R.boot();
  auto Ev = [&](BS L, BS Rt) { BtnQ[4].push_back(L); BtnQ[7].push_back(Rt); Ui.tickAll(R.U); };
  is(N, Ui, R, "?", "F=200.00 P=8" + M200 + "PRESETS");
  Ev(BS::Release, BS::None);
  is(N, Ui, R, "?", "F=133.00 P=8" + M200 + "PRESETS");
  Ev(BS::LongPress, BS::None);
  is(N, Ui, R, "?", "F=133.00 P=8" + M200 + "CONFIG");
  ok(R.D.Flash, "twobutton: menu flashes");
  is(N, Ui, R, "S33", "OK F=33.00 P=8" + M200 + "PRESETS");
  ok(!R.D.Flash, "twobutton: S closes menu");
  Ev(BS::None, BS::Release);
  is(N, Ui, R, "?", "F=66.00 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "F10P16", "OK F=10.00 P=16" + M200 + "UART");
  Ev(BS::Release, BS::None);
  auto Got = send(Ui, R, "?");
  ok(Got.find("MODE=PRESETS") != std::string::npos, "twobutton: button leaves UART, got " + Got);
  is(N, Ui, R, "R", "OK F=66.00 P=8" + M200 + "PRESETS");
  Ev(BS::Release, BS::Release);
  is(N, Ui, R, "?", "F=66.00 P=8" + M200 + "MANUAL");
  is(N, Ui, R, "S25", "OK F=25.00 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "S10", "OK F=10.00 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "D8", "OK DELETED F=10.00 P=8" + M200 + "PRESETS");
  Ev(BS::Release, BS::None);
  is(N, Ui, R, "?", "F=4.77 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "D8", "OK RESTORED F=8.00 P=8" + M200 + "PRESETS");
}

static void pot() {
  const char *N = "pot";
  BtnQ.clear(); PotVal = 500;
  Rig R; PotentiometerLogic Ui(28, 4, 1, R.Pi, R.D, R.DC, R.P, R.F, true, false); R.boot();
  auto Ticks = [&](int C) { for (int I = 0; I != C; ++I) Ui.tickAll(R.U); };
  Ticks(60);
  is(N, Ui, R, "?", "F=97.65 P=8" + M200 + "MANUAL");
  is(N, Ui, R, "S33", "OK F=33.00 P=8" + M200 + "PRESETS");
  Ticks(20);
  is(N, Ui, R, "?", "F=33.00 P=8" + M200 + "PRESETS");
  PotVal = 515; Ticks(20);
  is(N, Ui, R, "?", "F=33.00 P=8" + M200 + "PRESETS");
  PotVal = 700; Ticks(5);
  is(N, Ui, R, "?", "F=136.71 P=8" + M200 + "MANUAL");
  is(N, Ui, R, "R", "OK F=33.00 P=8" + M200 + "PRESETS");
  Ticks(20);
  is(N, Ui, R, "?", "F=33.00 P=8" + M200 + "PRESETS");
  is(N, Ui, R, "F10P16", "OK F=10.00 P=16" + M200 + "UART");
  Ticks(20);
  is(N, Ui, R, "?", "F=10.00 P=16" + M200 + "UART");
  BtnQ[4].push_back(BS::Release); Ticks(2);
  auto Got = send(Ui, R, "?");
  ok(Got.find("MODE=PRESETS") != std::string::npos, "pot: button leaves UART, got " + Got);
  is(N, Ui, R, "F10P16", "OK F=10.00 P=16" + M200 + "UART");
  PotVal = 300; Ticks(5);
  Got = send(Ui, R, "?");
  ok(Got.find("MODE=MANUAL") != std::string::npos, "pot: moving takes over, got " + Got);
  is(N, Ui, R, "M300", "OK F=300.00 P=8 MAX=300 MODE=PRESETS");
  Ticks(20);
  is(N, Ui, R, "?", "F=300.00 P=8 MAX=300 MODE=PRESETS");

  BtnQ.clear(); PotVal = 100;
  Rig R2; PotentiometerLogic Ui2(28, 4, 1, R2.Pi, R2.D, R2.DC, R2.P, R2.F, false, false); R2.boot();
  for (int I = 0; I != 60; ++I) Ui2.tickAll(R2.U);
  is("nopot", Ui2, R2, "?", "F=200.00 P=8" + M200 + "PRESETS");
  is("nopot", Ui2, R2, "S33", "OK F=33.00 P=8" + M200 + "PRESETS");
  PotVal = 900;
  for (int I = 0; I != 20; ++I) Ui2.tickAll(R2.U);
  is("nopot", Ui2, R2, "?", "F=33.00 P=8" + M200 + "PRESETS");
}

// A factory reset from the front panel must update the running values too.
static void panelReset() {
  const std::string Want = "F=200.00 P=8" + M200 + "PRESETS";
  const std::string At1000 = "OK F=1000.00 P=8 MAX=1000 MODE=PRESETS";
  {
    const char *N = "reset-rotary";
    RotQ.clear();
    Rig R; RotaryLogic Ui(2, 3, 4, 1, R.Pi, R.D, R.P, R.DC, R.F, false); R.boot();
    auto Ev = [&](RS S) { RotQ.push_back(S); Ui.tickAll(R.U); };
    is(N, Ui, R, "M1000", At1000);
    for (int I = 0; I != 3; ++I) Ev(RS::SwLongPress);
    is(N, Ui, R, "?", Want);
    ok(!R.D.Flash, "reset-rotary: flashing stopped");
    // Running below the maximum, from Manual mode.
    is(N, Ui, R, "M1000", At1000);
    Ev(RS::SwRelease);
    Ev(RS::Left);
    is(N, Ui, R, "?", "F=999.00 P=8 MAX=1000 MODE=MANUAL");
    for (int I = 0; I != 3; ++I) Ev(RS::SwLongPress);
    is(N, Ui, R, "?", Want);
  }
  {
    const char *N = "reset-pot";
    BtnQ.clear(); PotVal = 100;
    Rig R; PotentiometerLogic Ui(28, 4, 1, R.Pi, R.D, R.DC, R.P, R.F, false, false); R.boot();
    is(N, Ui, R, "M1000", At1000);
    for (int I = 0; I != 3; ++I) { BtnQ[4].push_back(BS::LongPress); Ui.tickAll(R.U); }
    is(N, Ui, R, "?", Want);
    ok(!R.D.Flash, "reset-pot: flashing stopped");
  }
  {
    const char *N = "reset-twobutton";
    BtnQ.clear();
    Rig R; TwoButtonLogic Ui(4, 7, 1, R.Pi, R.D, R.DC, R.P, R.F); R.boot();
    is(N, Ui, R, "M1000", At1000);
    for (int I = 0; I != 2; ++I) {
      BtnQ[4].push_back(BS::LongPress); BtnQ[7].push_back(BS::Pressed);
      Ui.tickAll(R.U);
    }
    is(N, Ui, R, "?", Want);
    ok(!R.D.Flash, "reset-twobutton: flashing stopped");
  }
}

int main() {
  rotary(); twoButton(); pot(); panelReset();
  printf("passed=%d failed=%d\n", Pass, Fail);
  return Fail != 0;
}
