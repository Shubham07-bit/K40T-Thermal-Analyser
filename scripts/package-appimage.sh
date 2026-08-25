#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_DIR}/build"
APPDIR="${PROJECT_DIR}/AppDir"

"${SCRIPT_DIR}/build-linux.sh"

rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin"
cp "${BUILD_DIR}/thermal_analyzer" "$APPDIR/usr/bin/"
cp "${PROJECT_DIR}/resources/k40t-thermal-analyser.png" "$APPDIR/k40t-thermal-analyser.png"
cp "${PROJECT_DIR}/resources/k40t-thermal-analyser.desktop" "$APPDIR/k40t-thermal-analyser.desktop"

if ! command -v linuxdeployqt >/dev/null 2>&1; then
    echo "linuxdeployqt not found in PATH. Please install it or download the AppImage."
    exit 1
fi

linuxdeployqt "${APPDIR}/usr/bin/thermal_analyzer" -qmake="${QT_PATH}/../../../bin/qmake" -appimage

echo "AppImage created."
