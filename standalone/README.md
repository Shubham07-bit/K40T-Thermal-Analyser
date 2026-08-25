# Standalone K40T Thermal Stats Tools

Two minimal C programs for reading temperature matrices from K40T thermal images.

## 1. `thermal_stats_blst` — for K40T IR JPEGs

Uses the BLST SDK (`blst_ir_file_*` functions) to read the proprietary
temperature matrix stored inside K40T IR image files such as
`19700101_001348_I.jpg`.

### Build

```bash
cd thermal_analyzer/standalone
make thermal_stats_blst
```

### Usage

```bash
./thermal_stats_blst 19700101_001348_I.jpg
```

### Example output

```text
[Image Info]
Width:  640
Height: 512
Total Pixels: 327680

[Temperature Statistics]
Lowest Temperature:  10.30 C @ (85, 503)
Highest Temperature: 21.40 C @ (525, 376)
Average Temperature: 17.30 C
Temperature Difference: 11.10 C

[Temperature Matrix] (640 x 512)
11.70 11.90 11.90 ...
...
```

If the file is not a valid BLST IR image, it prints an error and exits.

---

## 2. `thermal_stats` — for 16-bit grayscale PNG/TIFF

Loads a 16-bit grayscale PNG and prints the same statistics.
Pixel values are interpreted as **centi-degrees Celsius**:

```text
temperature_C = pixel_value / 100.0
```

If the image is not 16-bit grayscale, the program prints an error.

### Build

```bash
make thermal_stats
```

### Usage

```bash
./thermal_stats ../samples/thermal_test.png
```
