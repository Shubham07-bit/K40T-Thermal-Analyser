/*
 * thermal_stats_blst.c
 *
 * Standalone C program that uses the BLST SDK to parse the proprietary
 * temperature matrix stored inside BLST IR image files (e.g. *_I.jpg).
 *
 * Build:
 *     gcc -std=c99 -O2 thermal_stats_blst.c \
 *         -I../../include \
 *         ../../lib/linux-x86_64/libblstsdk.a \
 *         -o thermal_stats_blst -lm
 *
 * Usage:
 *     ./thermal_stats_blst 19700101_001348_I.jpg
 */

#include <blst_ir_file.h>
#include <stdio.h>
#include <stdlib.h>

static void print_usage(const char *prog)
{
    fprintf(stderr, "Usage: %s <blst_ir_image>\n", prog);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *path = argv[1];

    uint16_t width = 0;
    uint16_t height = 0;
    size_t count = 0;

    int ret = blst_ir_file_get_info(path, &width, &height, &count);
    if (ret != 0) {
        fprintf(stderr, "Error: blst_ir_file_get_info failed for '%s' (ret=%d)\n", path, ret);
        fprintf(stderr, "       The file is not a supported BLST IR image or the SDK could not read it.\n");
        return 1;
    }

    if (count == 0 || width == 0 || height == 0) {
        fprintf(stderr, "Error: invalid image info (width=%u height=%u count=%zu)\n",
                (unsigned)width, (unsigned)height, count);
        return 1;
    }

    float *temps = (float *)malloc(sizeof(float) * count);
    if (!temps) {
        fprintf(stderr, "Error: out of memory allocating %zu floats\n", count);
        return 1;
    }

    ret = blst_ir_file_parse_temperature_matrix(path, temps, count);
    if (ret != 0) {
        fprintf(stderr, "Error: blst_ir_file_parse_temperature_matrix failed (ret=%d)\n", ret);
        free(temps);
        return 1;
    }

    float min_temp = temps[0];
    float max_temp = temps[0];
    double sum = 0.0;
    size_t min_idx = 0;
    size_t max_idx = 0;

    for (size_t i = 0; i < count; ++i) {
        float t = temps[i];
        sum += t;
        if (t < min_temp) {
            min_temp = t;
            min_idx = i;
        }
        if (t > max_temp) {
            max_temp = t;
            max_idx = i;
        }
    }

    int min_x = (int)(min_idx % width);
    int min_y = (int)(min_idx / width);
    int max_x = (int)(max_idx % width);
    int max_y = (int)(max_idx / width);
    float avg_temp = (float)(sum / count);
    float diff_temp = max_temp - min_temp;

    printf("[Image Info]\n");
    printf("Width:  %u\n", (unsigned)width);
    printf("Height: %u\n", (unsigned)height);
    printf("Total Pixels: %zu\n", count);
    printf("\n");

    printf("[Temperature Statistics]\n");
    printf("Lowest Temperature:  %.2f C @ (%d, %d)\n", min_temp, min_x, min_y);
    printf("Highest Temperature: %.2f C @ (%d, %d)\n", max_temp, max_x, max_y);
    printf("Average Temperature: %.2f C\n", avg_temp);
    printf("Temperature Difference: %.2f C\n", diff_temp);
    printf("\n");

    printf("[Temperature Matrix] (%u x %u)\n", (unsigned)width, (unsigned)height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            printf("%.2f", temps[y * width + x]);
            if (x + 1 < width) printf(" ");
        }
        printf("\n");
    }

    free(temps);
    return 0;
}
