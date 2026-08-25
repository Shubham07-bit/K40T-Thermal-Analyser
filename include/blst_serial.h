#ifndef BLST_SERIAL_H
#define BLST_SERIAL_H

#include "blst_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
#ifdef _WIN32
    void *handle;
#else
    int fd;
#endif
    int is_open;
} blst_serial_t;

int blst_serial_open(blst_serial_t *serial, const char *port_name, int baud_rate);
int blst_serial_close(blst_serial_t *serial);
int blst_serial_write(blst_serial_t *serial, const uint8_t *data, size_t len);
int blst_serial_read(blst_serial_t *serial, uint8_t *data, size_t max_len, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
