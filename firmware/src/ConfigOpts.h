//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing

#ifndef __CONFIGOPTS_H__
#define __CONFIGOPTS_H__

#include <hardware/uart.h>
#include <vector>

// GPIOs
// Display
static constexpr const int DisplayClkGPIO = 0;
static constexpr const int DisplayDioGPIO = 1;
// Rotary Encoder
static constexpr const int RotaryClkGPIO = 2;
static constexpr const int RotaryDtGPIO = 3;
static constexpr const int RotarySwGPIO = 4;
// Buttons
static constexpr const int LeftButtonGPIO = RotarySwGPIO;
static constexpr const int RightButtonGPIO = 7;
// Buttons for presets
static std::vector<int> PresetBtnGPIOs = {10, 11, 12, 13, 18, 19, 20, 21};
// Reset sense
static constexpr const int ResetSenseGPIO = 22;
// Maximum speed for this many seconds.
static constexpr const int ResetMaxSpeedDuration = 20;

// Throttle pin
static constexpr const int ThrottleGPIO = 27;
// Jumpers
static constexpr const int ModeJP1GPIO = 5;
static constexpr const int ModeJP2GPIO = 6;
// Potentiometer
static constexpr const int PotentiometerGPIO = 28;
// UART
static constexpr const int UartGPIO = 8; // (also UartGPIO+1)
// Reverse knob direction.
static constexpr const int ReverseDirectionGPIO = 26;

// Uart configuration 9600 8N1
static constexpr const uint32_t UartRequestedBaud = 9600;
static constexpr const uint32_t UartDataBits = 8;
static constexpr const uart_parity_t UartParity = UART_PARITY_NONE;
static constexpr const uint32_t UartStopBits = 1;
static constexpr const uint32_t UartFlowControl = false;

static constexpr const char UartEOM = '\r';

// MHz Settings
static constexpr const int DefaultMaxMHz = 200;
// The maximum MHz we allow.
static constexpr const int MHzLimitHi = 5000;
// The minimum MHz we allow.
static constexpr const int MHzLimitLo = 1;
// This controls the PWM period, the higher the value the larger the period.
static constexpr const int PeriodLimitHi = 256;
static constexpr const int PeriodLimitLo = 1;

static constexpr const int TwoBtnMaxMHzStep = 50;

// Input Settings
// After how many loop iterations we are sampling the inputs.
// The lower the value the more responsive it is.
static constexpr const int PotSamplePeriod = 100;
static constexpr const int RotaryEncoderSamplePeriod = 1;
static constexpr const int ButtonSamplePeriod = 100;

// Turning the Rotary Encoder one step
static constexpr const int RotaryMaxMHzRightStep = 50;
static constexpr const int RotaryMaxMHzLeftStep = 1;

// Potentiometer settings.
static constexpr const int PotMoveAgainstSavedIdleLimit = 30;
static constexpr const int PotLeftPercent = 40;
static constexpr const int PotRightPercent = 60;
static constexpr const int PotentiometerRampUpIgnoreReadings = 50;
// A dead-zone for the min and max position of the potentiometer.
static constexpr const int ADCDeadZone = 20;
// ADC is noizy so take the average over this many measurements.
static constexpr const unsigned ADCDenoise = 32;

// Button debounce: Take the average of this many readings.
static constexpr const int ButtonDebounceSz = 2;

// Push button behavior.
static constexpr const int ButtonMedReleaseCnt = 20;
static constexpr const int ButtonLongPressCnt = 200;
static constexpr const int RotaryLongPressMillis = 1500;

// TM1637 Display Settings
// Flashing
static constexpr const int DisplayFlashOnPeriod = 100;
static constexpr const int DisplayFlashOffPeriod = 30;
// The delay after printing a message.
static constexpr const int PrintSleep = 1000;

// Other
// No need to update the PWM period on every UI tick. Skip this many.
static constexpr const int UpdatePWMSamplePeriod = 8;

// Factory defaults press button.
static constexpr const int ResetBtnPressMs = 2000;
static constexpr const int ResetBtnCheckMs = 50;
static constexpr const int ResetDefaultsFlashOnMs = 100;
static constexpr const uint32_t ResetDefaultFlashCnt = 20;

#endif // __CONFIGOPTS_H__
