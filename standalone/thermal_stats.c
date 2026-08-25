/*
 * thermal_stats.c
 *
 * Standalone C program that loads a 16-bit grayscale PNG thermal image
 * and prints temperature statistics plus the full temperature matrix.
 *
 * Assumption: pixel values are stored as centi-degrees Celsius.
 *             temperature_C = pixel_value / 100.0
 *
 * Build:
 *     gcc -std=c99 -O2 thermal_stats.c -o thermal_stats $(pkg-config --cflags --libs libpng)
 *
 * Usage:
 *     ./thermal_stats image.png
 *
 * If the image is not a 16-bit grayscale PNG, the program prints an error
 * because the temperature matrix cannot be read directly.
 */

#include <png.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_SCALE 0.01f

static void print_usage(const char *prog)
{
    fprintf(stderr, "Usage: %s <16-bit-grayscale-thermal.png>\n", prog);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *path = argv[1];
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Error: cannot open file '%s'\n", path);
        return 1;
    }

    /* Verify PNG signature. */
    unsigned char sig[8];
    if (fread(sig, 1, 8, fp) != 8 || png_sig_cmp(sig, 0, 8)) {
        fprintf(stderr, "Error: '%s' is not a PNG file\n", path);
        fclose(fp);
        return 1;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fprintf(stderr, "Error: png_create_read_struct failed\n");
        fclose(fp);
        return 1;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        fprintf(stderr, "Error: png_create_info_struct failed\n");
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        return 1;
    }

    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Error: libpng reported an error while reading '%s'\n", path);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 1;
    }

    png_init_io(png, fp);
    png_set_sig_bytes(png, 8);
    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    /* We need a direct temperature matrix. Only 16-bit grayscale gives us that. */
    if (color_type != PNG_COLOR_TYPE_GRAY) {
        fprintf(stderr, "Error: image is not grayscale (color_type=%d). Cannot read temperature matrix directly.\n", color_type);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 1;
    }

    if (bit_depth != 16) {
        fprintf(stderr, "Error: image bit depth is %d, expected 16-bit grayscale for temperature matrix.\n", bit_depth);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 1;
    }

    /* Convert to host byte order. */
    png_set_swap(png);

    png_bytepp rows = (png_bytepp)malloc(sizeof(png_bytep) * height);
    if (!rows) {
        fprintf(stderr, "Error: out of memory allocating row pointers\n");
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 1;
    }

    /* 16-bit grayscale = 2 bytes per pixel. */
    for (int y = 0; y < height; ++y) {
        rows[y] = (png_bytep)malloc(png_get_rowbytes(png, info));
        if (!rows[y]) {
            fprintf(stderr, "Error: out of memory allocating row %d\n", y);
            for (int k = 0; k < y; ++k) free(rows[k]);
            free(rows);
            png_destroy_read_struct(&png, &info, NULL);
            fclose(fp);
            return 1;
        }
    }

    png_read_image(png, rows);
    png_read_end(png, NULL);
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);

    /* Compute statistics. */
    float min_temp = 1e38f;
    float max_temp = -1e38f;
    double sum = 0.0;
    int min_x = -1, min_y = -1;
    int max_x = -1, max_y = -1;

    for (int y = 0; y < height; ++y) {
        const uint16_t *row = (const uint16_t *)rows[y];
        for (int x = 0; x < width; ++x) {
            float temp = (float)row[x] * TEMP_SCALE;
            sum += temp;

            if (temp < min_temp) {
                min_temp = temp;
                min_x = x;
                min_y = y;
            }
            if (temp > max_temp) {
                max_temp = temp;
                max_x = x;
                max_y = y;
            }
        }
    }

    int total_pixels = width * height;
    float avg_temp = (float)(sum / total_pixels);
    float diff_temp = max_temp - min_temp;

    printf("[Temperature Statistics]\n");
    printf("Lowest Temperature:  %.2f C @ (%d, %d)\n", min_temp, min_x, min_y);
    printf("Highest Temperature: %.2f C @ (%d, %d)\n", max_temp, max_x, max_y);
    printf("Average Temperature: %.2f C\n", avg_temp);
    printf("Temperature Difference: %.2f C\n", diff_temp);
    printf("\n");

    /* Print the full temperature matrix. */
    printf("[Temperature Matrix] (%d x %d)\n", width, height);
    for (int y = 0; y < height; ++y) {
        const uint16_t *row = (const uint16_t *)rows[y];
        for (int x = 0; x < width; ++x) {
            float temp = (float)row[x] * TEMP_SCALE;
            printf("%.2f", temp);
            if (x + 1 < width) printf(" ");
        }
        printf("\n");
    }

    /* Cleanup. */
    for (int y = 0; y < height; ++y)
        free(rows[y]);
    free(rows);

    return 0;
}
