#ifndef BLST_GIMBAL_H
#define BLST_GIMBAL_H

#include "blst_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 3.1 云台载荷协议：请求类消息 */
#define BLST_MSG_ID_GIMBAL_STATUS        0x000001u
#define BLST_MSG_ID_GIMBAL_ATTITUDE      0x000002u
#define BLST_MSG_ID_GIMBAL_CONTROL       0x000010u
#define BLST_MSG_ID_GIMBAL_ANGLE         0x000012u
#define BLST_MSG_ID_GIMBAL_GYRO_CALIB    0x000013u
#define BLST_MSG_ID_GIMBAL_MODE_SET      0x000014u
#define BLST_MSG_ID_GIMBAL_SPEED_SET     0x000017u
#define BLST_MSG_ID_GIMBAL_VERSION_GET   0x000018u
#define BLST_MSG_ID_GIMBAL_POINT_ALIGN   0x00002Cu
#define BLST_MSG_ID_GIMBAL_SERVO_OFF     0x00002Du
#define BLST_MSG_ID_GIMBAL_LINEAR_CALIB  0x00002Eu
#define BLST_MSG_ID_GIMBAL_SOFT_REBOOT   0x00002Fu
#define BLST_MSG_ID_GIMBAL_FAKE_ATTITUDE 0x000030u
#define BLST_MSG_ID_GIMBAL_ACCEL_CALIB   0x000031u
#define BLST_MSG_ID_GIMBAL_FC_INFO       0x000032u
#define BLST_MSG_ID_GIMBAL_STABILIZE_SET 0x000033u
#define BLST_MSG_ID_GIMBAL_MECH_ZERO_CALIB 0x000034u
#define BLST_MSG_ID_GIMBAL_OUTER_HALL_ZERO_CALIB 0x000035u

#define BLST_GIMBAL_CONTROL_PAYLOAD_LEN      4u
#define BLST_GIMBAL_ANGLE_PAYLOAD_LEN        7u
#define BLST_GIMBAL_SIMPLE_PAYLOAD_LEN       2u
#define BLST_GIMBAL_MODE_PAYLOAD_LEN         2u
#define BLST_GIMBAL_SPEED_PAYLOAD_LEN        3u
#define BLST_GIMBAL_VERSION_GET_PAYLOAD_LEN  2u
#define BLST_GIMBAL_POINT_ALIGN_PAYLOAD_LEN  7u
#define BLST_GIMBAL_FC_INFO_PAYLOAD_LEN      15u
#define BLST_GIMBAL_STABILIZE_PAYLOAD_LEN    2u
#define BLST_GIMBAL_MECH_ZERO_CALIB_PAYLOAD_LEN 2u
#define BLST_GIMBAL_OUTER_HALL_ZERO_CALIB_PAYLOAD_LEN 2u

typedef enum {
    BLST_GIMBAL_QUICK_NONE = 0x00,
    BLST_GIMBAL_QUICK_CENTER = 0x01,
    BLST_GIMBAL_QUICK_DOWN_90 = 0x02
} blst_gimbal_quick_mode_t;

typedef enum {
    BLST_GIMBAL_YAW_LEFT = 0x00,
    BLST_GIMBAL_YAW_RIGHT = 0x01,
    BLST_GIMBAL_YAW_STOP = 0x02
} blst_gimbal_yaw_dir_t;

typedef enum {
    BLST_GIMBAL_PITCH_UP = 0x00,
    BLST_GIMBAL_PITCH_DOWN = 0x01,
    BLST_GIMBAL_PITCH_STOP = 0x02
} blst_gimbal_pitch_dir_t;

typedef enum {
    BLST_GIMBAL_ANGLE_UP_OR_LEFT = 0x00,
    BLST_GIMBAL_ANGLE_DOWN_OR_RIGHT = 0x01,
    BLST_GIMBAL_ANGLE_NO_MOVE = 0x02
} blst_gimbal_angle_dir_t;

typedef enum {
    BLST_GIMBAL_MODE_FOLLOW = 0x01,
    BLST_GIMBAL_MODE_LOCK = 0x02,
    BLST_GIMBAL_MODE_JOINT_LOCK = 0x03,
    BLST_GIMBAL_MODE_THREE_AXIS_LOCK = 0x04
} blst_gimbal_mode_t;

typedef enum {
    BLST_GIMBAL_LENS_TELE = 0x00,
    BLST_GIMBAL_LENS_WIDE = 0x01,
    BLST_GIMBAL_LENS_IR = 0x02
} blst_gimbal_lens_t;

typedef enum {
    BLST_GIMBAL_STABILIZATION_OFF = 0x00,
    BLST_GIMBAL_STABILIZATION_ON = 0x01
} blst_gimbal_stabilization_switch_t;

typedef struct {
    int16_t expected_yaw_deg100;
    int16_t pitch_deg100;
    int16_t roll_deg100;
    int16_t yaw_deg100;
    uint8_t takeoff_valid_flags;
} blst_gimbal_fc_info_t;

typedef struct {
    uint8_t raw_status;
    uint8_t gimbal_link_abnormal;
    uint8_t camera_link_abnormal;
    uint8_t self_test;
    uint8_t ir_ok;
    uint8_t tele_visible_ok;
    uint8_t wide_visible_ok;
    uint8_t laser_ok;
    uint8_t tele_motor_ok;
    uint8_t gimbal_model;
    uint8_t stabilization_status;
    uint8_t install_status;
} blst_gimbal_status_t;

typedef struct {
    int16_t joint_yaw_deg100;
    int16_t joint_roll_deg100;
    int16_t joint_pitch_deg100;
    int16_t attitude_yaw_deg100;
    int16_t attitude_roll_deg100;
    int16_t attitude_pitch_deg100;
    int16_t yaw_speed_deg100_s;
    int16_t pitch_speed_deg100_s;
    int16_t roll_speed_deg100_s;
} blst_gimbal_attitude_t;

/* Payload builders are SDK-internal helpers. Normal users should include blst_sdk.h. */
int blst_gimbal_build_control_payload(blst_gimbal_quick_mode_t quick_mode, blst_gimbal_yaw_dir_t yaw_dir, blst_gimbal_pitch_dir_t pitch_dir, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_build_angle_payload(blst_gimbal_angle_dir_t pitch_dir, uint16_t pitch_angle_deg100, blst_gimbal_angle_dir_t yaw_dir, uint16_t yaw_angle_deg100, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_build_simple_2_payload(uint8_t byte1, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_build_mode_payload(blst_gimbal_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_build_speed_payload(uint8_t pitch_speed, uint8_t yaw_speed, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_build_point_align_payload(blst_gimbal_lens_t lens, uint16_t zoom_x10, uint16_t x, uint16_t y, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_build_fc_info_payload(const blst_gimbal_fc_info_t *info, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_build_stabilization_payload(blst_gimbal_stabilization_switch_t sw, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_build_mech_zero_calib_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_build_outer_hall_zero_calib_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_gimbal_parse_status_payload(const uint8_t *payload, uint8_t payload_len, blst_gimbal_status_t *out_status);
int blst_gimbal_parse_attitude_payload(const uint8_t *payload, uint8_t payload_len, blst_gimbal_attitude_t *out_attitude);

#ifdef __cplusplus
}
#endif

#endif
