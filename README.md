# K40T Thermal Analyser

A Qt 6.8 C++ desktop application for visualizing and analyzing thermal images
from the **K40T camera**.

## Features

- **Multi-image loading** via a single file dialog (hold Ctrl/Cmd to select multiple files)
- **Reads the actual temperature matrix** from K40T IR JPEGs using the BLST SDK
- **Color palettes**: Original, White Hot, Black Hot, Iron Bow, Rainbow, Arctic, Lava, Sepia
- **Original view** — displays the embedded image unchanged
- **Min / Max temperature markers** overlaid on the image
- **Hover** any pixel to see its temperature in Celsius and Fahrenheit
- **Persistent measurement points**: left-click the image to place a marker; listed in the side panel
- **Clear / remove last point** from the side panel or View menu
- **Export with overlays**:
  - One loaded image: save-file dialog with points + min/max overlays
  - Multiple loaded images: select which images to export, then choose an output directory
- **Manual or auto temperature range**
- **Metadata panel** showing image info, points list, and camera metadata
- **Pan / zoom** with mouse wheel and right/middle-drag
- **Synthetic test image generator** for quick UI testing

## Build

```bash
cd /home/shubham/Desktop/BLST_SDK_release_v1.1_linux_x86_64/thermal_analyzer
mkdir -p build
cmake -S . -B build -DCMAKE_PREFIX_PATH=/home/shubham/Qt6.8.3-install/lib/cmake/Qt6
cmake --build build -j$(nproc)
```

Run:

```bash
./build/thermal_analyzer
```

Load images directly from the command line:

```bash
./build/thermal_analyzer /path/to/image1.jpg /path/to/image2.jpg
```

Headless test (no GUI, prints stats):

```bash
./build/thermal_analyzer -t /path/to/image.jpg
```

Headless batch export test (no GUI, exports overlays):

```bash
./build/thermal_analyzer --export-batch-test /tmp/outdir /path/to/image.jpg
```

## Supported Formats

| Format | Extension | Notes |
|---|---|---|
| K40T IR JPEG | `.jpg`, `.jpeg` | **Full support.** Reads the proprietary temperature matrix directly using the BLST SDK. |
| 16-bit grayscale PNG/TIFF | `.png`, `.tif`, `.tiff` | Pixel values are interpreted as centi-Celsius (`value / 100`). Override with a sidecar JSON. |
| Other JPEG | `.jpg`, `.jpeg` | Returns an error if no embedded radiometric data is found. |

### Sidecar JSON

Place a `.json` file next to your thermal image with the same base name to
override scale/offset/unit interpretation:

```json
{
  "scale": 0.01,
  "offset": 0.0,
  "unit": "Celsius (centi-degrees)"
}
```

Example: `image_001.png` + `image_001.json`.

## Project Structure

```text
thermal_analyzer/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── cmake/
│   └── mingw-w64-toolchain.cmake   # Local MinGW cross-compile toolchain
├── include/
│   └── blst_*.h                    # BLST SDK headers
├── lib/
│   ├── linux-x86_64/libblstsdk.a   # Linux BLST SDK static library
│   └── windows-x86_64/blstsdk64.lib # Windows BLST SDK library (MSVC)
├── resources/
│   ├── k40t-thermal-analyser.desktop
│   └── k40t-thermal-analyser.png
├── samples/
│   └── 19700101_001348_I.jpg       # Sample K40T IR image
├── scripts/
│   ├── build-linux.sh
│   ├── build-windows.sh
│   └── package-appimage.sh
├── src/
│   ├── main.cpp
│   ├── MainWindow.h/.cpp/.ui       # Main UI
│   ├── ImageView.h/.cpp            # Interactive image viewer
│   ├── ThermalDataModel.h/.cpp     # Raw temperature grid + metadata
│   ├── ColorMap.h/.cpp             # Palette rendering
│   ├── MatrixDialog.h/.cpp         # Temperature matrix viewer
│   ├── ThermalImageLoader.h        # Loader interface
│   ├── PngTiffLoader.h/.cpp        # 16-bit PNG/TIFF loader
│   ├── DjiRjpgLoader.h/.cpp        # Radiometric JPEG loader (best effort)
│   ├── BlstIrLoader.h/.cpp         # K40T IR JPEG loader (SDK-based)
│   ├── BatchProcessor.h/.cpp       # Multi-file loading
│   └── OverlayExporter.h/.cpp      # Render & save images with overlays
└── standalone/
    └── thermal_stats_blst.c        # Standalone BLST stats tool
```

## Architecture

1. A `ThermalImageLoader` parses a file into a `ThermalDataModel`.
   - `PngTiffLoader` handles 16-bit grayscale PNG/TIFF.
   - `BlstIrLoader` uses the BLST SDK (`blst_ir_file_*`) to decode proprietary K40T IR JPEGs.
   - `DjiRjpgLoader` attempts to extract embedded radiometric data from other JPEGs.
2. `ThermalDataModel` holds a 2D `QVector<float>` of Celsius temperatures plus metadata.
3. `ColorMap::render()` converts the raw grid into an RGB `QImage` using the selected palette.
   - The **Original** palette maps temperature directly to luminance without false colors when no embedded image is available.
4. `ImageView` displays the rendered image, handles zoom/pan, and overlays min/max markers.
5. `MainWindow` ties everything together: file dialogs, batch list, metadata tree, palette/range controls.

## BLST SDK Integration

The app links against the BLST SDK to decode proprietary K40T IR JPEGs
(e.g. `19700101_001348_I.jpg`). The SDK is now bundled inside the repository:

```text
include/blst_ir_file.h
lib/linux-x86_64/libblstsdk.a
lib/windows-x86_64/blstsdk64.lib
```

The Linux build uses `lib/linux-x86_64/libblstsdk.a` automatically.
The Windows MSVC build uses `lib/windows-x86_64/blstsdk64.lib` automatically.
The local MinGW cross-build cannot link the MSVC `.lib`, so K40T IR JPEG support
is disabled there unless a MinGW-compatible library (`libblstsdk64.a` or
`libblstsdk64.dll.a` + `blstsdk64.dll`) is provided.

If you also want live IR capture or device control, use the BLST SDK APIs in a
dedicated capture module.

## Keyboard Shortcuts

| Action | Shortcut |
|---|---|
| Open images | `Ctrl+O` |
| Export with overlays | `Ctrl+E` |
| Zoom in | Toolbar button |
| Zoom out | Toolbar button |
| Reset zoom | Toolbar button |
| Remove last point | View menu |
| Clear all points | View menu |

## Copyright

Copyright &copy; Indrones Solution Pvt. Ltd.  
[https://indrones.com/](https://indrones.com/)
