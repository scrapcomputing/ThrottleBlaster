#pragma once
#include "ConfigOpts.h"
class Display {
public:
  bool Flash = false;
  void printRaw(int) {} void printMHz(int) {} void printKHz(int) {}
  void printTxt(const char *) {}
  void setFlash(bool S) { Flash = S; }
};
