#ifndef BLST_TYPES_H
#define BLST_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLST_STX                 0xFDu
#define BLST_MAX_PAYLOAD_LEN     255u
#define BLST_FRAME_HEADER_LEN    10u
#define BLST_FRAME_CRC_LEN       2u
#define BLST_MAX_FRAME_LEN       (BLST_FRAME_HEADER_LEN + BLST_MAX_PAYLOAD_LEN + BLST_FRAME_CRC_LEN)

#define BLST_DEFAULT_SRC_SYS_ID  0x01u
#define BLST_DEFAULT_SRC_COMP_ID 0x01u

#define BLST_DEFAULT_DST_SYS_ID  0x04u
#define BLST_DEFAULT_DST_COMP_ID 0x01u

#define BLST_MSG_CLASS_REQ       0x000000u
#define BLST_MSG_CLASS_ACK       0x010000u
#define BLST_MSG_CLASS_STATUS    0x020000u
#define BLST_MSG_BASE_MASK       0x00FFFFu
#define BLST_MSG_CLASS_MASK      0xFF0000u

typedef enum {
    BLST_OK = 0,
    BLST_ERR_INVALID_PARAM = -1,
    BLST_ERR_BUFFER_TOO_SMALL = -2,
    BLST_ERR_CRC = -3,
    BLST_ERR_NEED_MORE = -4,
    BLST_ERR_NOT_FOUND = -5,
    BLST_ERR_IO = -6,
    BLST_ERR_TIMEOUT = -7,
    BLST_ERR_UNSUPPORTED = -8,
    BLST_ERR_NOT_OPEN = -9
} blst_result_t;

typedef enum {
    BLST_ACK_OK = 0x0000,
    BLST_ACK_FAILED = 0x0001,
    BLST_ACK_UNKNOWN = 0x0002,
    BLST_ACK_CRC_FAILED = 0x0003,
    BLST_ACK_TIMEOUT = 0x0004,
    BLST_ACK_MD5_FAILED = 0x0005,
    BLST_ACK_NO_SPACE = 0x0006,
    BLST_ACK_LENGTH_MISMATCH = 0x0007,
    BLST_ACK_IN_PROGRESS = 0x0008,
    BLST_ACK_FILE_NOT_FOUND = 0x0009,
    BLST_ACK_FILE_MERGE_ERROR = 0x000A,

    BLST_ACK_POD_RECORDING = 0x0201,
    BLST_ACK_POD_CAMERA_OPEN_FAILED = 0x0202,
    BLST_ACK_POD_PHOTO_IN_PROGRESS = 0x0203,
    BLST_ACK_POD_NO_SD_CARD = 0x0204
} blst_ack_code_t;

typedef struct {
    uint8_t target_system_id;
    uint8_t target_component_id;
    uint8_t sequence;
    uint8_t source_system_id;
    uint8_t source_component_id;
    uint32_t msg_id;
    uint8_t payload_len;
    uint8_t payload[BLST_MAX_PAYLOAD_LEN];
} blst_frame_t;

#ifdef __cplusplus
}
#endif

#endif
