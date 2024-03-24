//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing

#ifndef __UART_H__
#define __UART_H__

#include <hardware/uart.h>
#include <vector>
#include <string>

class Uart {
  static const std::vector<uint8_t> Empty;
  /// The UART instance (either uart0 or uart1).
  uart_inst *Instance = nullptr;
  /// The baud rate actually set, which may differe from the requested.
  uint32_t ActualBaudrate = 0;

public:
  static const char *GetParityStr(uart_parity_t Parity);
  /// Valid GPIOs uart0: (GP0-1, GP12-13, GP16-17), uart1: (GP4-5 or GP8-9).
  /// Parity: UART_PARITY_NONE, UART_PARITY_EVEN, UART_PARITY_ODD
  Uart(int GPIO, uint32_t RequestedBaudrate, uint32_t DataBits,
       uint32_t StopBits, uart_parity_t Parity, bool FlowControl);
  ~Uart();

  void writeBlockingStr(const std::string &Str);
  void writeBlocking(const std::vector<uint8_t> &Bytes);

  std::vector<uint8_t> readNonBlocking();
};


#endif // __UART_H__
