//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing

#include "Uart.h"
#include "Debug.h"
#include "Pico.h"
#include "Utils.h"
#include <iostream>

const char *Uart::GetParityStr(uart_parity_t Parity) {
  switch (Parity) {
  case UART_PARITY_NONE:
    return "UART_PARITY_NONE";
  case UART_PARITY_EVEN:
    return "UART_PARITY_EVEN";
  case UART_PARITY_ODD:
    return "UART_PARITY_ODD";
  }
  unreachable("Bad Parity");
}

Uart::Uart(int GPIO, uint32_t RequestedBaudrate, uint32_t DataBits,
           uint32_t StopBits, uart_parity_t Parity, bool FlowControl) {
  /// Instance: is uart0 (GP0-1, GP12-13, GP16-17) or uart1 (GP4-5 or GP8-9).
  switch (GPIO) {
  case 0:
  case 12:
  case 16:
    Instance = uart0;
    break;
  case 4:
  case 8:
    Instance = uart1;
    break;
  default:
    std::cerr << "UART error: Bad GPIO=" << GPIO << "\n";
  }
  gpio_set_function(GPIO, GPIO_FUNC_UART);
  gpio_set_function(GPIO + 1, GPIO_FUNC_UART);
  ActualBaudrate = uart_init(Instance, RequestedBaudrate);
  uart_set_hw_flow(Instance, /*cts=*/FlowControl, /*rts=*/FlowControl);
  uart_set_format(Instance, /*data_bits=*/DataBits, /*stop_bits=*/StopBits,
                  /*parity=*/Parity);
  uart_set_fifo_enabled(Instance, false);
  DBG_PRINT(std::cerr << "Actual Baudrate=" << ActualBaudrate << "\n";
            std::cerr << "DataBits=" << DataBits << "\n";
            std::cerr << "StopBits=" << StopBits << "\n";
            std::cerr << "Parity=" << GetParityStr(Parity) << "\n";
            std::cerr << "FlowControl=" << FlowControl << "\n";)
}
Uart::~Uart() { uart_deinit(Instance); }

void Uart::writeBlockingStr(const std::string &Str) {
  uart_write_blocking(Instance, (const uint8_t *)Str.c_str(), Str.size());
}

void Uart::writeBlocking(const std::vector<uint8_t> &Bytes) {
  uart_write_blocking(Instance, Bytes.data(), Bytes.size());
}

std::vector<uint8_t> Uart::readNonBlocking() {
  size_t ReadyBytes = uart_is_readable(Instance);
  if (ReadyBytes == 0)
    return Empty;
  std::vector<uint8_t> Bytes;
  Bytes.resize(ReadyBytes);
  uart_read_blocking(Instance, Bytes.data(), Bytes.size());
  return Bytes;
}

const std::vector<uint8_t> Uart::Empty;
