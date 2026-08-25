#ifndef BLST_PROTOCOL_H
#define BLST_PROTOCOL_H

#include "blst_types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint16_t blst_crc_calculate(const uint8_t *buffer, uint16_t length);

int blst_protocol_pack(
    uint8_t target_system_id,
    uint8_t target_component_id,
    uint8_t sequence,
    uint8_t source_system_id,
    uint8_t source_component_id,
    uint32_t msg_id,
    const uint8_t *payload,
    uint8_t payload_len,
    uint8_t *out_frame,
    size_t out_capacity,
    size_t *out_len
);

int blst_protocol_parse_frame(
    const uint8_t *data,
    size_t data_len,
    blst_frame_t *out_frame,
    size_t *consumed_len
);

#ifdef __cplusplus
}
#endif

#endif
