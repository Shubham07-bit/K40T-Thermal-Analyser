#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_DIR}/build-windows"

# Adjust these paths for your local MinGW + Windows Qt setup.
MINGW_TOOLCHAIN="${MINGW_TOOLCHAIN:-${PROJECT_DIR}/cmake/mingw-w64-toolchain.cmake}"
QT_HOST_PATH="${QT_HOST_PATH:-/home/shubham/Qt6.8.3-install}"

cmake -B "$BUILD_DIR" -S "$PROJECT_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$MINGW_TOOLCHAIN" \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_HOST_PATH="$QT_HOST_PATH"

cmake --build "$BUILD_DIR" -j"$(nproc)"

echo "Windows build complete: ${BUILD_DIR}/thermal_analyzer.exe"
