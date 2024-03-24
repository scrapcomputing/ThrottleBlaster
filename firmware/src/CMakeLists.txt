cmake_minimum_required(VERSION 3.13)

# Build
# -----
# $ mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DPICO_SDK_PATH=/path/to/pico-sdk/ ../src/ && make -j
# For 7-segment display support you need to specify the path to the
# Pico-TM1637 library (https://github.com/wahlencraft/TM1637-pico)
# -DPICO_TM1637_PATH=/path/to/TM1637-pico/
#
# Options
# -------
# o -DDISPLAY_SHIFT_LEFT=<num> If the numbers displayed need shifting
# o -DDISPLAY_BRIGHTNESS=<num> (0-7) Sets the brightness 7 is max, default: 1.
# o -DDISABLE_PICO_LED=on to disable the Pico's blinking LED.
# o -DDBGPRINT=on to enable debug messages
#
# This will place the firmware into: build/ThrottleBlaster.uf2
#
# For example: minicom -b 115200 -D /dev/ttyACM0. Serial connection: 115200 8N1

set(REVISION_MAJOR 0)
set(REVISION_MINOR 3)

message("PICO_SDK_PATH = ${PICO_SDK_PATH}")

# set(PICO_EXTRAS_PATH "$ENV{HOME}/pico-extras/")

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

set(PROJECT_NAME ThrottleBlaster)
project(
  ${PROJECT_NAME}
  LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
include_directories("${PICO_SDK_PATH}/src/common/pico_stdlib/include")
include_directories("${PICO_SDK_PATH}/src/common/pico_base/include")
include_directories("${PICO_SDK_PATH}/src/rp2_common/hardware_adc/include")
include_directories("${PICO_SDK_PATH}/src/rp2_common/pico_multicore/include")
include_directories("${PROJECT_BINARY_DIR}/") # for build/config.h
if (DEFINED PICO_TM1637_PATH)
  include("${PICO_TM1637_PATH}/PicoTM1637.cmake")
  set(PICO_TM1637 1)
endif ()

if (NOT DEFINED DISPLAY_SHIFT_LEFT)
  set(DISPLAY_SHIFT_LEFT 0)
endif()

if (NOT DEFINED DISPLAY_BRIGHTNESS)
  set(DISPLAY_BRIGHTNESS 1)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror ${EXTRA_DBG_FLAGS}")

# rest of your project
file(GLOB SOURCES *.c *.cpp *.h *.def)
add_executable(${PROJECT_NAME} ${SOURCES})

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/Pwm.pio)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

set(LIBS
  pico_stdlib
  hardware_adc
  pico_multicore
  hardware_pio
  )
if (DEFINED PICO_TM1637_PATH)
  set(LIBS PicoTM1637 ${LIBS})
endif ()
target_link_libraries(${PROJECT_NAME} ${LIBS})


message("")
message("+---------------+")
message("| Configuration |")
message("+---------------+")
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
if (DISABLE_USB_DBG STREQUAL "0")
  pico_enable_stdio_usb(${PROJECT_NAME} 0)
  pico_enable_stdio_uart(${PROJECT_NAME} 0)
endif ()
message("DISABLE_USB_DBG = ${DISABLE_USB_DBG}")

message("PICO_LED = ${PICO_LED}")
message("PICO_TM1637_PATH = ${PICO_TM1637_PATH}")
message("LCD_SHIFT_LEFT = ${LCD_SHIFT_LEFT}")
message("MHZ = ${MHZ}")

# End of configuration
message("")

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

# Create map/bin/hex/uf2 in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})
