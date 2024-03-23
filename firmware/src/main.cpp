//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#include "ConfigOpts.h"
#include "Debug.h"
#include "DutyCycle.h"
#include "Flash.h"
#include "Pico.h"
#include "PotentiometerLogic.h"
#include "Pwm.pio.h"
#include "RotaryEncoder.h"
#include "RotaryLogic.h"
#include "TwoButtonLogic.h"
#include "hardware/pio.h"
#include <bsp/board.h>
#include <chrono>
#include <config.h>
#include <hardware/watchdog.h>
#include <memory>
#include <pico/multicore.h>

static void toggleLed(Pico &Pico) {
  static bool ON;
  static volatile unsigned Cnt;
  ++Cnt;
  if (Cnt != MainLEDToggleCnt)
    return;
  if (ON)
    Pico.ledOFF();
  else
    Pico.ledON();
  ON = !ON;
  Cnt = 0;
}

PresetsTable Presets;


static void pio_pwm_set_period(PIO Pio, uint SM, uint32_t Period) {
  pio_sm_set_enabled(Pio, SM, false);
  pio_sm_put_blocking(Pio, SM, Period);
  pio_sm_exec(Pio, SM, pio_encode_pull(false, false));
  pio_sm_exec(Pio, SM, pio_encode_out(pio_isr, 32));
  pio_sm_set_enabled(Pio, SM, true);
}

static void pio_pwm_set_level(PIO Pio, uint SM, uint32_t Level) {
  pio_sm_put_blocking(Pio, SM, Level);
}

// Core 1 is focused on toggling the throttle pin.
static void _main_loop() {
  DBG_PRINT(std::cout << "Core1 starting...\n";)

}

enum class UIMode {
  Rotary,
  TwoButton,
  Button,
  ButtonWithPot,
};

const char *getUIModeStr(UIMode Mode) {
  switch (Mode) {
  case UIMode::Rotary:
    return "Rotary";
  case UIMode::TwoButton:
    return "TwoButton";
  case UIMode::Button:
    return "Button";
  case UIMode::ButtonWithPot:
    return "ButtonWithPot";
  }
  return "UNKNOWN";
}

static UIMode getUIMode(Pico &Pico) {
  Pico.readGPIO();
  bool JP1 = Pico.getGPIO(ModeJP1GPIO);
  bool JP2 = Pico.getGPIO(ModeJP2GPIO);
  if (JP1 && JP2)
    return UIMode::Button;
  if (JP1 && !JP2)
    return UIMode::ButtonWithPot;
  if (!JP1 && !JP2)
    return UIMode::TwoButton;
  if (!JP1 && JP2)
    return UIMode::Rotary;
  std::cerr << "Bad Mode!\n";
  exit(1);
}

class ThrottlePin {
  Pico &Pi;
  DutyCycle &DC;
  PIO Pio;
  int SM;
  int LastLevel = 0;
  int LastPeriod = 0;
  int CntTicks = 0;

public:
  ThrottlePin(Pico &Pi, DutyCycle &DC) : Pi(Pi), DC(DC) {
    // Start PIO for the throttle pin
    Pi.setGPIO(ThrottleGPIO, true);
    Pio = pio1;
    SM = pio_claim_unused_sm(Pio, true);
    uint ProgramOffset = pio_add_program(Pio, &pwm_program);
    pwm_program_init(Pio, SM, ProgramOffset, ThrottleGPIO);
    DBG_PRINT(DC.dump();)
  }
  void updatePWM() {
    if (++CntTicks != UpdatePWMSamplePeriod)
      return;
    CntTicks = 0;

    int Level = DC.getLevel();
    int Period = DC.getPeriod();
    if (Level != LastLevel || Period != LastPeriod) {
      DBG_PRINT(DC.dump();)
      if (Level == 0) {
        pio_sm_set_enabled(Pio, SM, false);
        Pi.setGPIO(ThrottleGPIO, false);
      } else if (Level == Period) {
        pio_sm_set_enabled(Pio, SM, false);
        Pi.setGPIO(ThrottleGPIO, true);
      } else {
        if (LastLevel == 0 || LastLevel == Period)
          pio_sm_set_enabled(Pio, SM, true);

        pio_pwm_set_period(Pio, SM, Period);
        pio_pwm_set_level(Pio, SM, Level);
      }
    }
    LastLevel = Level;
    LastPeriod = Period;
  }
};

// Core 0 is handling the UI.
static void core0_main_loop(Pico &Pico, Display &Disp, FlashStorage &Flash,
                            DutyCycle &DC) {
  int LoopCntSinceRotary = 0;

  std::unique_ptr<CommonLogic> UI;
  auto Mode = getUIMode(Pico);
  DBG_PRINT(std::cout << "Mode=" << getUIModeStr(Mode) << "\n";)
  switch (Mode) {
  case UIMode::Rotary:
    UI = std::make_unique<RotaryLogic>(RotaryClkGPIO, RotaryDtGPIO,
                                       RotarySwGPIO, RotaryEncoderSamplePeriod,
                                       Pico, Disp, Presets, DC, Flash);
    break;
  case UIMode::Button:
  case UIMode::ButtonWithPot:
    UI = std::make_unique<PotentiometerLogic>(
        PotentiometerGPIO, LeftButtonGPIO, PotSamplePeriod, Pico, Disp, DC,
        Presets, Flash, /*EnablePot=*/Mode == UIMode::ButtonWithPot);
    break;
  case UIMode::TwoButton:
    UI = std::make_unique<TwoButtonLogic>(LeftButtonGPIO, RightButtonGPIO,
                                          ButtonSamplePeriod, Pico, Disp, DC,
                                          Presets, Flash);
    break;
  }

  ThrottlePin TPin(Pico, DC);

  while (true) {
    // The main entry point for the UI.
    UI->tick();
    // Update the Throttle pin PWM if needed.
    TPin.updatePWM();
#ifndef DISALBE_PICO_LED
    toggleLed(Pico);
#endif
  }
}

// Hack for linking error: undefined reference to `__dso_handle'
static void *__dso_handle = 0;

int main() {
  (void)__dso_handle;
  Pico Pico;
  DBG_PRINT(sleep_ms(1000);)
  DBG_PRINT(std::cout << "ThrottleBlaster rev." << REVISION_MAJOR << "."
                      << REVISION_MINOR << "\n";)
  Pico.initGPIO(PICO_DEFAULT_LED_PIN, GPIO_OUT, Pico::Pull::Down, "LED");
  Pico.initGPIO(ThrottleGPIO, GPIO_OUT, Pico::Pull::Down, "Throttle");

  Pico.initGPIO(ModeJP1GPIO, GPIO_IN, Pico::Pull::Up, "ModeJP1");
  Pico.initGPIO(ModeJP2GPIO, GPIO_IN, Pico::Pull::Up, "ModeJP2");

  Display Disp(DisplayClkGPIO, DisplayDioGPIO);

  // Read Presets from flash.
  FlashStorage Flash;
  if (Flash.valid())
    Presets.readFromFlash(Flash);
  else
    DBG_PRINT(std::cout << "Flash not valid!\n";)

  // DutyCycle needs an updated Presets.
  DutyCycle DC(Presets);

  core0_main_loop(Pico, Disp, Flash, DC);
  return 0;
}
