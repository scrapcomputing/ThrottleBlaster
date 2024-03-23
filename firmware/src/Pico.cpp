//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#include "Pico.h"
#include "Debug.h"
#include "Utils.h"
#include "hardware/pll.h"
#include "hardware/structs/rosc.h"
#include "hardware/structs/scb.h"
#include <iomanip>

#ifndef NDEBUG
static uint32_t AlreadySetMask = 0u;
#endif // NDEBUG

PinRange::PinRange(uint32_t From, uint32_t To) : From(From), To(To) {
  Mask = ((1u << (To + 1 - From)) - 1) << From;
#ifndef NDEBUG
  if ((AlreadySetMask & Mask) != 0u)
    die("Some pins in range ", From, " to ", To, " are already set!");
  AlreadySetMask |= Mask;
#endif // NDEBUG
}

void PinRange::dump(std::ostream &OS) const {
  auto Flags = OS.flags();
  if (From == To)
    OS << std::setw(5) << std::setfill(' ') << From;
  else
    OS << std::setw(2) << From << "-" << std::setw(2) << To;
  OS << " Mask=";
  OS << "0x" << std::setw(8) << std::setfill('0') << std::hex << Mask;
  OS.flags(Flags);
}

void PinRange::dump() const { dump(std::cerr); }

Pico::Pico() {
  // Initialize stdio so that we can print debug messages.
  stdio_init_all();
  adc_init();
  // Initialize the Pico LED.
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  // Use pins 16-17 as debugging uart
  gpio_set_function(16, GPIO_FUNC_UART); // TX
  gpio_set_function(17, GPIO_FUNC_UART); // RX

  // Wait for a bit otherwise this does not show up during serial debug.
  sleep_ms(500);
  std::cerr << "+---------------------------------+\n";
  std::cerr << "|          " << PROJECT_NAME << "\n";
  std::cerr << "+---------------------------------+\n";
  std::cerr << "clk_sys = " << frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS)
            << "KHz\n";
  std::cerr << "clk_usb = " << frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB)
            << "KHz\n";
  std::cerr << "clk_peri = "
            << frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI) << "KHz\n";
  std::cerr << "clk_ref = " << frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_REF)
            << "KHz\n";
  std::cerr << "clk_adc = " << frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC)
            << "KHz\n";
  std::cerr << "clk_rtc = " << frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC)
            << "KHz\n";
}

void Pico::initGPIO(const PinRange &Pins, int Direction, Pull Pull,
                    const char *Descr) {
  DBG_PRINT(std::cout << "Setting up GPIO " << std::setw(5) << std::setfill(' ') << Descr
            << " ";)
  DBG_PRINT(Pins.dump(std::cout);)
  DBG_PRINT(std::cout << " " << (Direction == GPIO_IN ? "IN" : "OUT") << "\n";)
  for (uint32_t Pin = Pins.getFrom(), E = Pins.getTo(); Pin <= E; ++Pin) {
    gpio_init(Pin);
    gpio_set_dir(Pin, Direction);
    switch (Pull) {
    case Pull::Up:
      gpio_pull_up(Pin);
      break;
    case Pull::Down:
      gpio_pull_down(Pin);
      break;
    case Pull::None:
      gpio_disable_pulls(Pin);
      break;
    }
  }
}

void Pico::initADC(const PinRange &Pins, const char *Descr) {
  DBG_PRINT(std::cout << "Setting up ADC " << std::setw(5) << std::setfill(' ')
                      << Descr << " ";)
  DBG_PRINT(Pins.dump(std::cout);)
  DBG_PRINT(std::cout << "\n";)
  for (uint32_t Pin = Pins.getFrom(), E = Pins.getTo(); Pin <= E; ++Pin) {
    adc_gpio_init(Pin);
  }
}

uint16_t Pico::readADC(uint32_t GPIO) const {
  if (!(GPIO >= 26 && GPIO < 29)) {
    std::cerr << "Bad ADC Pin " << GPIO << ". Good values are 26..29 !\n";
    abort();
  }
  int AdcId = GPIO - 26;
  adc_select_input(AdcId);
  uint16_t Raw_12bit = adc_read();
  return Raw_12bit;
}
