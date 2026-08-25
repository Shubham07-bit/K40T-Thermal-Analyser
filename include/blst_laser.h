#ifndef BLST_LASER_H
#define BLST_LASER_H

#include "blst_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 3.3.1 激光载荷协议 */
#define BLST_MSG_ID_LASER_RANGE_SET           0x000400u
#define BLST_MSG_ID_LASER_PERIODIC_RANGE_SET  0x000406u

#define BLST_LASER_SIMPLE_PAYLOAD_LEN            2u
#define BLST_LASER_RANGE_ACK_PAYLOAD_LEN         4u
#define BLST_LASER_PERIODIC_STATUS_PAYLOAD_LEN   4u

typedef enum {
    BLST_LASER_RANGE_OFF = 0x00,
    BLST_LASER_RANGE_SINGLE = 0x01
} blst_laser_range_mode_t;

typedef enum {
    BLST_LASER_PERIODIC_RANGE_OFF = 0x00,
    BLST_LASER_PERIODIC_RANGE_ON = 0x01
} blst_laser_periodic_range_mode_t;

typedef struct {
    uint16_t response_code;
    uint16_t distance_dm;
} blst_laser_range_ack_t;

typedef struct {
    uint16_t response_code;
    uint16_t distance_dm;
} blst_laser_periodic_range_status_t;

int blst_laser_build_range_payload(blst_laser_range_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_laser_build_periodic_range_payload(blst_laser_periodic_range_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_laser_parse_range_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_laser_range_ack_t *out_ack);
int blst_laser_parse_periodic_range_status_payload(const uint8_t *payload, uint8_t payload_len, blst_laser_periodic_range_status_t *out_status);

#ifdef __cplusplus
}
#endif

#endif
