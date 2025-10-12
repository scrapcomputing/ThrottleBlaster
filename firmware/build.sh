#!/bin/bash
#
# Copyright (C) 2025 Scrap Computing
#
# A helper script for building the release firmwares for both Pico 1 and Pico 2
#

set -e

if [ $# -lt 1 ]; then
    echo "USAGE: $(basename ${0}) <path/to/pico-sdk> [TM1637_PATH]"
    echo "Example: $(basename ${0}) ~/pico-sdk ~/TM1637-pico-1.2.1"
    exit 1
fi

PICO_SDK_PATH=${1}
TM1637_PATH=${2}

echo "PICO_SDK_PATH=${PICO_SDK_PATH}"

BUILD_PICO1=build_pico1
BUILD_PICO2=build_pico2
for dir in "${BUILD_PICO1}" "${BUILD_PICO2}"; do
    rm -rf ${dir}
    mkdir -p ${dir}
done

SRC_DIR=src/
CMAKE_COMMON="-DCMAKE_BUILD_TYPE=Release -DPICO_SDK_PATH=${PICO_SDK_PATH} -DPICO_TM1637_PATH=${TM1637_PATH}"

echo "=== Pico 1 ==="
cmake -B ${BUILD_PICO1} ${CMAKE_COMMON} -DPICO_BOARD=pico ${SRC_DIR}
make -C ${BUILD_PICO1} -j

echo "=== Pico 2 ==="
cmake -B ${BUILD_PICO2} ${CMAKE_COMMON} -DPICO_BOARD=pico2 ${SRC_DIR}
make -C ${BUILD_PICO2} -j

echo "Done!"
for dir in "${BUILD_PICO1}" "${BUILD_PICO2}"; do
    ls ${dir}/*.uf2
done
