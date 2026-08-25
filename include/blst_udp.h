#ifndef BLST_UDP_H
#define BLST_UDP_H

#include "blst_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
#ifdef _WIN32
    void *socket_handle;
#else
    int sockfd;
#endif
    int is_open;
    char remote_ip[64];
    uint16_t remote_port;
    uint16_t local_port;
} blst_udp_t;

int blst_udp_open(blst_udp_t *udp, const char *remote_ip, uint16_t remote_port, uint16_t local_port);
int blst_udp_close(blst_udp_t *udp);
int blst_udp_send(blst_udp_t *udp, const uint8_t *data, size_t len);
int blst_udp_recv(blst_udp_t *udp, uint8_t *data, size_t max_len, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
