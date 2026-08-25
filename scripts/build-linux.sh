#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_DIR}/build"

QT_PATH="${QT_PATH:-/home/shubham/Qt6.8.3-install/lib/cmake/Qt6}"
CMAKE_PREFIX=""
if [ -d "$QT_PATH" ]; then
    CMAKE_PREFIX="-DCMAKE_PREFIX_PATH=$QT_PATH"
fi

cmake -B "$BUILD_DIR" -S "$PROJECT_DIR" -DCMAKE_BUILD_TYPE=Release $CMAKE_PREFIX
cmake --build "$BUILD_DIR" -j"$(nproc)"

echo "Linux build complete: ${BUILD_DIR}/thermal_analyzer"
