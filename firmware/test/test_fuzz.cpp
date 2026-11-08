// Random commands and knob events. Checks invariants after every step.
#include "RotaryLogic.h"
#include <cstdio>
#include <random>
#include <string>
#include <vector>
template <size_t N> static std::string pick(std::mt19937 &G, const char *const (&A)[N]) { return A[G() % N]; }
static const char *const Fq[] = {"1","2.5","4.77","8","10","25","33","66","100","133","150","200","300","1000","5000","0","0.0001","9999"};
static const char *const Ps[] = {"4.77","8","10","25","33","66","100","133","150","200","300","77"};
static const char *const Pd[] = {"1","2","8","16","256","0","300"};
static const char *const Mx[] = {"1","33","50","100","133","150","200","300","733","1000","5000","0","6000"};
static std::string gen(std::mt19937 &G) {
  switch (G() % 11) {
  case 0: return "F" + pick(G, Fq) + "P" + pick(G, Pd);
  case 1: return "F" + pick(G, Fq);
  case 2: case 3: return "S" + pick(G, Ps);
  case 4: return "SMAX";
  case 5: return "M" + pick(G, Mx);
  case 6: return "R";
  case 7: return "W" + pick(G, Ps);
  case 8: return "D" + pick(G, Ps);
  case 9: return G() % 8 ? "?" : "X";
  default: return "?";
  }
}
struct Stat { int Runs = 0, Bad = 0, Printed = 0; };
static void run(const char *Name, bool Knob, int Seeds, int Steps, Stat &St) {
  for (int Seed = 0; Seed != Seeds; ++Seed) {
    std::mt19937 G(Seed);
    Pico Pi; Display D; FlashStorage F; PresetsTable P; DutyCycle DC(P); Uart U;
    RotaryLogic Ui(2, 3, 4, 1, Pi, D, P, DC, F, false);
    DC.setKHz(P.getKHz()); DC.setPeriod(P.getPeriod());
    RotQ.clear();
    std::vector<std::string> Hist;
    ++St.Runs;
    for (int I = 0; I != Steps; ++I) {
      std::string What;
      if (Knob && G() % 3 == 0) {
        static const RotaryEncoder::State E[] = {RotaryEncoder::State::Left, RotaryEncoder::State::Right, RotaryEncoder::State::SwRelease, RotaryEncoder::State::SwLongPress};
        auto Ev = E[G() % 4];
        RotQ.push_back(Ev); Ui.tickAll(U);
        What = "<knob " + std::to_string((int)Ev) + ">";
        if (!U.Tx.empty()) What += " UNEXPECTED";
      } else {
        What = gen(G);
        for (char B : What + "\r") U.Rx.push_back(B);
        while (!U.Rx.empty()) Ui.tickAll(U);
        size_t Lines = 0;
        for (size_t Pos = 0; (Pos = U.Tx.find("\r\n", Pos)) != std::string::npos; Pos += 2) ++Lines;
        if (Lines != 1) What += " REPLIES=" + std::to_string(Lines);
        What += " -> " + U.Tx.substr(0, U.Tx.size() >= 2 ? U.Tx.size() - 2 : 0);
        U.Tx.clear();
      }
      Hist.push_back(What);
      std::string Err;
      if (What.find("REPLIES=") != std::string::npos || What.find("UNEXPECTED") != std::string::npos) Err = "reply count";
      else if (DC.getLevel() < 0 || DC.getLevel() > DC.getMaxLevel()) Err = "PWM level out of range KHz=" + std::to_string(DC.getKHz()) + " Max=" + std::to_string(P.getMaxKHz()) + " Level=" + std::to_string(DC.getLevel());
      else if (DC.getKHz() > P.getMaxKHz() || DC.getKHz() < 0) Err = "KHz out of range";
      else if ((DC.getPeriod() + 1) / 256 < 1 || (DC.getPeriod() + 1) / 256 > 256) Err = "Period out of range";
      else if (P.findIdxByKHz(P.getMaxKHz()) < 0) Err = "max preset missing or deleted";
      else if (F.Writes) { PresetsTable Q; Q.readFromFlash(F); if (Q.getMaxMHz() < 1) Err = "bad flash reload"; }
      if (!Err.empty()) {
        ++St.Bad;
        if (St.Printed++ < 4) {
          printf("[%s seed %d step %d] %s\n", Name, Seed, I, Err.c_str());
          for (size_t H = Hist.size() > 8 ? Hist.size() - 8 : 0; H != Hist.size(); ++H)
            printf("    %s\n", Hist[H].c_str());
        }
        break;
      }
    }
  }
  printf("%s: runs=%d failing=%d\n", Name, St.Runs, St.Bad);
}
static void bytes(int Seeds, int &Bad) {
  for (int Seed = 0; Seed != Seeds; ++Seed) {
    std::mt19937 G(Seed);
    Pico Pi; Display D; FlashStorage F; PresetsTable P; DutyCycle DC(P); Uart U;
    RotaryLogic Ui(2, 3, 4, 1, Pi, D, P, DC, F, false);
    int Want = 0, Len = 0;
    for (int I = 0; I != 4000; ++I) {
      unsigned char B = G() % 4 == 0 ? "\r\nFSMWDXRP?.0123456789"[G() % 21] : (unsigned char)G();
      if (B == '\n') { U.Rx.push_back(B); continue; }
      if (B == '\r') { if (Len) ++Want; Len = 0; } else ++Len;
      U.Rx.push_back(B);
    }
    while (!U.Rx.empty()) Ui.tickAll(U);
    int Got = 0;
    for (size_t Pos = 0; (Pos = U.Tx.find("\r\n", Pos)) != std::string::npos; Pos += 2) ++Got;
    if (Got != Want) { ++Bad; if (Bad < 4) printf("[bytes seed %d] replies=%d lines=%d\n", Seed, Got, Want); }
  }
  printf("bytes: runs=%d failing=%d\n", Seeds, Bad);
}
int main() {
  Stat A, B; int C = 0;
  run("serial-only", false, 300, 1500, A);
  run("serial+knob", true, 300, 1500, B);
  bytes(200, C);
  return (A.Bad || B.Bad || C) != 0;
}
