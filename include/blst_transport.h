#ifndef BLST_TRANSPORT_H
#define BLST_TRANSPORT_H

#include "blst_types.h"
#include "blst_serial.h"
#include "blst_udp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BLST_TRANSPORT_NONE = 0,
    BLST_TRANSPORT_SERIAL = 1,
    BLST_TRANSPORT_UDP = 2
} blst_transport_type_t;

typedef struct {
    blst_transport_type_t type;
    blst_serial_t serial;
    blst_udp_t udp;
} blst_transport_t;

int blst_transport_open_serial(blst_transport_t *transport, const char *port_name, int baud_rate);
int blst_transport_open_udp(blst_transport_t *transport, const char *remote_ip, uint16_t remote_port, uint16_t local_port);
int blst_transport_close(blst_transport_t *transport);
int blst_transport_send(blst_transport_t *transport, const uint8_t *data, size_t len);
int blst_transport_recv(blst_transport_t *transport, uint8_t *data, size_t max_len, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
