#ifndef BLST_IR_FILE_H
#define BLST_IR_FILE_H

#include <stddef.h>
#include <stdint.h>

#include "blst_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int blst_ir_file_get_info(const char *file_path,
                          uint16_t *out_width,
                          uint16_t *out_height,
                          size_t *out_temperature_count);

int blst_ir_file_parse_temperature_matrix(const char *file_path,
                                          float *out_temperatures,
                                          size_t out_count);

#ifdef __cplusplus
}
#endif

#endif
