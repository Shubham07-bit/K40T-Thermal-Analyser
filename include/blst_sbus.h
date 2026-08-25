#ifndef BLST_SBUS_H
#define BLST_SBUS_H

#include "blst_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 3.3.2 SBUS通道协议 */
#define BLST_MSG_ID_SBUS_RANGE_SET        0x000500u
#define BLST_MSG_ID_SBUS_CHANNEL_CONFIG   0x000501u
#define BLST_MSG_ID_SBUS_CONFIG_GET       0x000502u
#define BLST_MSG_ID_SBUS_SWITCH           0x000503u
#define BLST_MSG_ID_SBUS_ZOOM_CHANNEL_SET 0x000504u
#define BLST_MSG_ID_SBUS_ZOOM_CHANNEL_GET 0x000505u

#define BLST_SBUS_RANGE_PAYLOAD_LEN       5u
#define BLST_SBUS_CHANNEL_CONFIG_PAYLOAD_LEN 8u
#define BLST_SBUS_SIMPLE_PAYLOAD_LEN      2u
#define BLST_SBUS_CONFIG_ACK_PAYLOAD_LEN  14u
#define BLST_SBUS_ZOOM_CHANNEL_ACK_PAYLOAD_LEN 3u
#define BLST_SBUS_CHANNEL_UNUSED          16u
#define BLST_SBUS_CHANNEL_MAX_VALUE       2047u

typedef enum {
    BLST_SBUS_SWITCH_ON = 0x01,
    BLST_SBUS_SWITCH_OFF = 0x02
} blst_sbus_switch_t;

typedef struct {
    uint8_t stream_channel;
    uint8_t zoom_channel;
    uint8_t photo_channel;
    uint8_t record_channel;
    uint8_t gimbal_pitch_channel;
    uint8_t gimbal_yaw_channel;
    uint8_t gimbal_center_channel;
    uint8_t ir_pseudo_color_channel;
} blst_sbus_channel_config_t;

typedef struct {
    uint16_t response_code;
    uint16_t max_value;
    uint16_t min_value;
    uint8_t stream_channel;
    uint8_t zoom_channel;
    uint8_t photo_channel;
    uint8_t record_channel;
    uint8_t gimbal_pitch_channel;
    uint8_t gimbal_yaw_channel;
    uint8_t gimbal_center_channel;
    uint8_t sbus_enabled;
    uint8_t ir_pseudo_color_channel;
    uint8_t raw_byte14;
} blst_sbus_config_ack_t;

typedef struct {
    uint16_t response_code;
    uint8_t zoom_channel;
} blst_sbus_zoom_channel_ack_t;

int blst_sbus_build_range_payload(uint16_t max_value, uint16_t min_value, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_sbus_build_channel_config_payload(const blst_sbus_channel_config_t *config, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_sbus_build_config_get_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_sbus_build_switch_payload(blst_sbus_switch_t sw, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_sbus_build_zoom_channel_set_payload(uint8_t channel, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_sbus_build_zoom_channel_get_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_sbus_parse_config_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_sbus_config_ack_t *out_config);
int blst_sbus_parse_zoom_channel_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_sbus_zoom_channel_ack_t *out_ack);

#ifdef __cplusplus
}
#endif

#endif
