#ifndef BLST_SDK_H
#define BLST_SDK_H

#include "blst_types.h"
#include "blst_transport.h"
#include "blst_gimbal.h"
#include "blst_camera.h"
#include "blst_laser.h"
#include "blst_sbus.h"
#include "blst_ir_file.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BLST_STATE_NONE = 0,
    BLST_STATE_GIMBAL_STATUS = 1,
    BLST_STATE_GIMBAL_ATTITUDE = 2,
    BLST_STATE_CAMERA_SYSTEM_STATUS = 3,
    BLST_STATE_IR_CAMERA_STATUS = 4,
    BLST_STATE_VISIBLE_CAMERA_STATUS = 5,
    BLST_STATE_CAMERA_UPGRADE_STATUS = 6,
    BLST_STATE_VISIBLE_NIGHT_MODE_STATUS = 7,
    BLST_STATE_CAMERA_PHOTO_STATUS = 8,
    BLST_STATE_CAMERA_RECORD_STATUS = 9,
    BLST_STATE_CAMERA_HYBRID_ZOOM_STATUS = 10,
    BLST_STATE_CAMERA_TF_FORMAT_STATUS = 11,
    BLST_STATE_CAMERA_AI_DETECTION_STATUS = 12,
    BLST_STATE_CAMERA_TARGET_GPS_STATUS = 13,
    BLST_STATE_CAMERA_TRACK_TARGET_STATUS = 14,
    BLST_STATE_CAMERA_MEDIA_PLAYBACK_STATUS = 15,
    BLST_STATE_LASER_PERIODIC_RANGE_STATUS = 16
} blst_state_type_t;

typedef struct {
    blst_state_type_t type;
    uint32_t msg_id;
    union {
        blst_gimbal_status_t gimbal_status;
        blst_gimbal_attitude_t gimbal_attitude;
        blst_camera_system_status_t camera_system_status;
        blst_ir_camera_status_t ir_camera_status;
        blst_visible_camera_status_t visible_camera_status;
        blst_camera_upgrade_status_t camera_upgrade_status;
        blst_visible_night_mode_status_t visible_night_mode_status;
        blst_camera_photo_status_t camera_photo_status;
        blst_camera_record_status_t camera_record_status;
        blst_camera_hybrid_zoom_status_t camera_hybrid_zoom_status;
        blst_camera_tf_format_status_t camera_tf_format_status;
        blst_camera_ai_detection_status_t camera_ai_detection_status;
        blst_camera_target_gps_info_t camera_target_gps_status;
        blst_camera_ai_detection_status_t camera_track_target_status;
        blst_camera_media_playback_status_t camera_media_playback_status;
        blst_laser_periodic_range_status_t laser_periodic_range_status;
    } data;
} blst_state_t;

typedef void (*blst_sdk_frame_callback_t)(const blst_frame_t *frame, void *user_data);
typedef void (*blst_sdk_ack_callback_t)(uint32_t base_msg_id, blst_ack_code_t ack, const blst_frame_t *frame, void *user_data);
typedef void (*blst_sdk_state_callback_t)(const blst_state_t *state, void *user_data);

typedef enum {
    BLST_LOG_LEVEL_NONE = 0,
    BLST_LOG_LEVEL_ERROR = 1,
    BLST_LOG_LEVEL_WARN = 2,
    BLST_LOG_LEVEL_INFO = 3,
    BLST_LOG_LEVEL_DEBUG = 4
} blst_log_level_t;

/*
 * Log callback ABI is stable for current SDK line.
 * By default, message is prefixed by SDK with: [YYYY-MM-DD HH:MM:SS.mmm][LEVEL] .
 * Build with -DBLST_SDK_LOG_ENABLE_TIMESTAMP=0 to keep the original raw message text.
 */
typedef void (*blst_sdk_log_callback_t)(blst_log_level_t level, const char *function_name, int result, const char *message, void *user_data);

typedef struct {
    uint8_t source_system_id;
    uint8_t source_component_id;
    uint8_t target_system_id;
    uint8_t target_component_id;
    uint8_t sequence;
    blst_transport_t transport;
    uint8_t rx_buffer[1024];
    size_t rx_len;
    uint32_t last_ack_base_msg_id;
    blst_ack_code_t last_ack_code;
    blst_state_t last_state;
    uint32_t state_count;
    int debug_enabled;
    blst_log_level_t log_level;
    blst_sdk_log_callback_t log_callback;
    void *log_callback_user_data;
    blst_sdk_frame_callback_t frame_callback;
    void *frame_callback_user_data;
    blst_sdk_ack_callback_t ack_callback;
    void *ack_callback_user_data;
    blst_sdk_state_callback_t state_callback;
    void *state_callback_user_data;
} blst_sdk_t;

/* Section 2: protocol/transport SDK foundation */
int blst_sdk_init_serial(blst_sdk_t *sdk, const char *port_name, int baud_rate, uint8_t target_system_id, uint8_t target_component_id);
int blst_sdk_init_udp(blst_sdk_t *sdk, const char *remote_ip, uint16_t remote_port, uint16_t local_port, uint8_t target_system_id, uint8_t target_component_id);
int blst_sdk_deinit(blst_sdk_t *sdk);
void blst_sdk_set_debug(blst_sdk_t *sdk, int enabled);
void blst_sdk_set_log_callback(blst_sdk_t *sdk, blst_sdk_log_callback_t callback, void *user_data);
void blst_sdk_set_log_level(blst_sdk_t *sdk, blst_log_level_t level);
void blst_sdk_set_global_log_callback(blst_sdk_log_callback_t callback, void *user_data);
void blst_sdk_set_global_log_level(blst_log_level_t level);
void blst_sdk_set_frame_callback(blst_sdk_t *sdk, blst_sdk_frame_callback_t callback, void *user_data);
void blst_sdk_set_ack_callback(blst_sdk_t *sdk, blst_sdk_ack_callback_t callback, void *user_data);
void blst_sdk_set_state_callback(blst_sdk_t *sdk, blst_sdk_state_callback_t callback, void *user_data);
int blst_sdk_get_last_state(const blst_sdk_t *sdk, blst_state_t *out_state);
int blst_sdk_send_message(blst_sdk_t *sdk, uint32_t msg_id, const uint8_t *payload, uint8_t payload_len);
int blst_sdk_send_command(blst_sdk_t *sdk, uint32_t msg_id, const uint8_t *payload, uint8_t payload_len);
blst_ack_code_t blst_sdk_send_command_wait_ack(blst_sdk_t *sdk, uint32_t msg_id, const uint8_t *payload, uint8_t payload_len, uint32_t timeout_ms);
int blst_sdk_input(blst_sdk_t *sdk, const uint8_t *data, size_t len);
int blst_sdk_poll(blst_sdk_t *sdk, uint32_t timeout_ms);
blst_ack_code_t blst_sdk_wait_ack(blst_sdk_t *sdk, uint32_t base_msg_id, uint32_t timeout_ms);
const char *blst_result_to_string(int result);
const char *blst_ack_to_string(blst_ack_code_t ack);
const char *blst_log_level_to_string(blst_log_level_t level);

/* Section 3.1: gimbal payload protocol. All angles use protocol unit: degree * 100. */
int blst_sdk_gimbal_control(blst_sdk_t *sdk, blst_gimbal_quick_mode_t quick_mode, blst_gimbal_yaw_dir_t yaw_dir, blst_gimbal_pitch_dir_t pitch_dir);
blst_ack_code_t blst_sdk_gimbal_control_wait_ack(blst_sdk_t *sdk, blst_gimbal_quick_mode_t quick_mode, blst_gimbal_yaw_dir_t yaw_dir, blst_gimbal_pitch_dir_t pitch_dir, uint32_t timeout_ms);
int blst_sdk_gimbal_center(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_center_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_look_down_90(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_look_down_90_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_move_yaw_left(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_move_yaw_left_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_move_yaw_right(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_move_yaw_right_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_move_pitch_up(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_move_pitch_up_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_move_pitch_down(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_move_pitch_down_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_stop(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_stop_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_set_angle(blst_sdk_t *sdk, blst_gimbal_angle_dir_t pitch_dir, uint16_t pitch_angle_deg100, blst_gimbal_angle_dir_t yaw_dir, uint16_t yaw_angle_deg100);
blst_ack_code_t blst_sdk_gimbal_set_angle_wait_ack(blst_sdk_t *sdk, blst_gimbal_angle_dir_t pitch_dir, uint16_t pitch_angle_deg100, blst_gimbal_angle_dir_t yaw_dir, uint16_t yaw_angle_deg100, uint32_t timeout_ms);
int blst_sdk_gimbal_gyro_calibrate(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_gyro_calibrate_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_set_mode(blst_sdk_t *sdk, blst_gimbal_mode_t mode);
blst_ack_code_t blst_sdk_gimbal_set_mode_wait_ack(blst_sdk_t *sdk, blst_gimbal_mode_t mode, uint32_t timeout_ms);
int blst_sdk_gimbal_set_speed(blst_sdk_t *sdk, uint8_t pitch_speed, uint8_t yaw_speed);
blst_ack_code_t blst_sdk_gimbal_set_speed_wait_ack(blst_sdk_t *sdk, uint8_t pitch_speed, uint8_t yaw_speed, uint32_t timeout_ms);
int blst_sdk_gimbal_get_version(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_get_version_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_point_align(blst_sdk_t *sdk, blst_gimbal_lens_t lens, uint16_t zoom_x10, uint16_t x, uint16_t y);
blst_ack_code_t blst_sdk_gimbal_point_align_wait_ack(blst_sdk_t *sdk, blst_gimbal_lens_t lens, uint16_t zoom_x10, uint16_t x, uint16_t y, uint32_t timeout_ms);
int blst_sdk_gimbal_servo_off(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_servo_off_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_linear_calibrate(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_linear_calibrate_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_soft_reboot(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_soft_reboot_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_use_fake_attitude(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_use_fake_attitude_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_accel_calibrate(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_accel_calibrate_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_send_fc_info(blst_sdk_t *sdk, const blst_gimbal_fc_info_t *info);
blst_ack_code_t blst_sdk_gimbal_send_fc_info_wait_ack(blst_sdk_t *sdk, const blst_gimbal_fc_info_t *info, uint32_t timeout_ms);
int blst_sdk_gimbal_set_stabilization(blst_sdk_t *sdk, blst_gimbal_stabilization_switch_t sw);
blst_ack_code_t blst_sdk_gimbal_set_stabilization_wait_ack(blst_sdk_t *sdk, blst_gimbal_stabilization_switch_t sw, uint32_t timeout_ms);
int blst_sdk_gimbal_calibrate_mechanical_zero(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_calibrate_mechanical_zero_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_gimbal_calibrate_outer_hall_zero(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_gimbal_calibrate_outer_hall_zero_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);

/* Section 3.2.2: infrared camera setting messages */
int blst_sdk_ir_camera_get_all_params(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_ir_camera_get_all_params_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
/* Uses normal zoom speed in Byte2 for compatibility. */
int blst_sdk_ir_camera_set_digital_zoom(blst_sdk_t *sdk, uint8_t zoom);
blst_ack_code_t blst_sdk_ir_camera_set_digital_zoom_wait_ack(blst_sdk_t *sdk, uint8_t zoom, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_pseudo_color(blst_sdk_t *sdk, uint8_t pseudo_color);
blst_ack_code_t blst_sdk_ir_camera_set_pseudo_color_wait_ack(blst_sdk_t *sdk, uint8_t pseudo_color, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_temperature_measure(blst_sdk_t *sdk, blst_ir_temp_measure_switch_t sw);
blst_ack_code_t blst_sdk_ir_camera_set_temperature_measure_wait_ack(blst_sdk_t *sdk, blst_ir_temp_measure_switch_t sw, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_sharpness(blst_sdk_t *sdk, uint8_t sharpness);
blst_ack_code_t blst_sdk_ir_camera_set_sharpness_wait_ack(blst_sdk_t *sdk, uint8_t sharpness, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_brightness(blst_sdk_t *sdk, uint8_t brightness);
blst_ack_code_t blst_sdk_ir_camera_set_brightness_wait_ack(blst_sdk_t *sdk, uint8_t brightness, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_contrast(blst_sdk_t *sdk, uint8_t contrast);
blst_ack_code_t blst_sdk_ir_camera_set_contrast_wait_ack(blst_sdk_t *sdk, uint8_t contrast, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_denoise(blst_sdk_t *sdk, uint8_t enabled, uint8_t level);
blst_ack_code_t blst_sdk_ir_camera_set_denoise_wait_ack(blst_sdk_t *sdk, uint8_t enabled, uint8_t level, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_enhancement(blst_sdk_t *sdk, uint8_t level);
blst_ack_code_t blst_sdk_ir_camera_set_enhancement_wait_ack(blst_sdk_t *sdk, uint8_t level, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_point_temperature(blst_sdk_t *sdk, uint16_t x, uint16_t y);
blst_ack_code_t blst_sdk_ir_camera_set_point_temperature_wait_ack(blst_sdk_t *sdk, uint16_t x, uint16_t y, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_area_temperature(blst_sdk_t *sdk, uint16_t width, uint16_t height, uint16_t center_x, uint16_t center_y);
blst_ack_code_t blst_sdk_ir_camera_set_area_temperature_wait_ack(blst_sdk_t *sdk, uint16_t width, uint16_t height, uint16_t center_x, uint16_t center_y, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_gain_mode(blst_sdk_t *sdk, blst_ir_gain_mode_t mode);
blst_ack_code_t blst_sdk_ir_camera_set_gain_mode_wait_ack(blst_sdk_t *sdk, blst_ir_gain_mode_t mode, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_temperature_overlay(blst_sdk_t *sdk, blst_ir_temp_overlay_switch_t sw);
blst_ack_code_t blst_sdk_ir_camera_set_temperature_overlay_wait_ack(blst_sdk_t *sdk, blst_ir_temp_overlay_switch_t sw, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_digital_zoom_with_speed(blst_sdk_t *sdk, uint8_t zoom, blst_camera_zoom_speed_t speed);
blst_ack_code_t blst_sdk_ir_camera_set_digital_zoom_with_speed_wait_ack(blst_sdk_t *sdk, uint8_t zoom, blst_camera_zoom_speed_t speed, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_fusion_visible(blst_sdk_t *sdk, blst_ir_fusion_visible_switch_t sw, uint8_t visible_weight, uint8_t ir_weight);
blst_ack_code_t blst_sdk_ir_camera_set_fusion_visible_wait_ack(blst_sdk_t *sdk, blst_ir_fusion_visible_switch_t sw, uint8_t visible_weight, uint8_t ir_weight, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_fusion_visible_debug(blst_sdk_t *sdk, blst_ir_fusion_visible_debug_action_t action, int16_t visible_offset_x, int16_t visible_offset_y, int16_t rotation_angle);
blst_ack_code_t blst_sdk_ir_camera_set_fusion_visible_debug_wait_ack(blst_sdk_t *sdk, blst_ir_fusion_visible_debug_action_t action, int16_t visible_offset_x, int16_t visible_offset_y, int16_t rotation_angle, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_fusion_visible_grayscale(blst_sdk_t *sdk, blst_ir_fusion_visible_grayscale_t grayscale);
blst_ack_code_t blst_sdk_ir_camera_set_fusion_visible_grayscale_wait_ack(blst_sdk_t *sdk, blst_ir_fusion_visible_grayscale_t grayscale, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_isotherm(blst_sdk_t *sdk, blst_ir_isotherm_switch_t sw, int16_t high_temp_decic, int16_t low_temp_decic);
blst_ack_code_t blst_sdk_ir_camera_set_isotherm_wait_ack(blst_sdk_t *sdk, blst_ir_isotherm_switch_t sw, int16_t high_temp_decic, int16_t low_temp_decic, uint32_t timeout_ms);
int blst_sdk_ir_camera_set_ai_super_resolution(blst_sdk_t *sdk, blst_ir_ai_super_resolution_switch_t sw);
blst_ack_code_t blst_sdk_ir_camera_set_ai_super_resolution_wait_ack(blst_sdk_t *sdk, blst_ir_ai_super_resolution_switch_t sw, uint32_t timeout_ms);

/* Section 3.2.3: visible camera setting messages */
int blst_sdk_visible_camera_get_all_params(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_visible_camera_get_all_params_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_record_resolution(blst_sdk_t *sdk, blst_visible_record_resolution_t resolution);
blst_ack_code_t blst_sdk_visible_camera_set_record_resolution_wait_ack(blst_sdk_t *sdk, blst_visible_record_resolution_t resolution, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_photo_resolution(blst_sdk_t *sdk, blst_visible_photo_resolution_t resolution);
blst_ack_code_t blst_sdk_visible_camera_set_photo_resolution_wait_ack(blst_sdk_t *sdk, blst_visible_photo_resolution_t resolution, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_iso(blst_sdk_t *sdk, blst_visible_iso_t iso);
blst_ack_code_t blst_sdk_visible_camera_set_iso_wait_ack(blst_sdk_t *sdk, blst_visible_iso_t iso, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_shutter(blst_sdk_t *sdk, blst_visible_shutter_t shutter);
blst_ack_code_t blst_sdk_visible_camera_set_shutter_wait_ack(blst_sdk_t *sdk, blst_visible_shutter_t shutter, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_ev(blst_sdk_t *sdk, blst_visible_ev_t ev);
blst_ack_code_t blst_sdk_visible_camera_set_ev_wait_ack(blst_sdk_t *sdk, blst_visible_ev_t ev, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_white_balance(blst_sdk_t *sdk, blst_visible_white_balance_t white_balance);
blst_ack_code_t blst_sdk_visible_camera_set_white_balance_wait_ack(blst_sdk_t *sdk, blst_visible_white_balance_t white_balance, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_antiflicker(blst_sdk_t *sdk, blst_visible_antiflicker_t antiflicker);
blst_ack_code_t blst_sdk_visible_camera_set_antiflicker_wait_ack(blst_sdk_t *sdk, blst_visible_antiflicker_t antiflicker, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_highlight_suppression(blst_sdk_t *sdk, blst_visible_level_switch_t sw, uint8_t value);
blst_ack_code_t blst_sdk_visible_camera_set_highlight_suppression_wait_ack(blst_sdk_t *sdk, blst_visible_level_switch_t sw, uint8_t value, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_backlight_comp(blst_sdk_t *sdk, blst_visible_level_switch_t sw, uint8_t value);
blst_ack_code_t blst_sdk_visible_camera_set_backlight_comp_wait_ack(blst_sdk_t *sdk, blst_visible_level_switch_t sw, uint8_t value, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_ae_lock(blst_sdk_t *sdk, blst_visible_ae_lock_t ae_lock);
blst_ack_code_t blst_sdk_visible_camera_set_ae_lock_wait_ack(blst_sdk_t *sdk, blst_visible_ae_lock_t ae_lock, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_metering_mode(blst_sdk_t *sdk, blst_visible_metering_mode_t mode);
blst_ack_code_t blst_sdk_visible_camera_set_metering_mode_wait_ack(blst_sdk_t *sdk, blst_visible_metering_mode_t mode, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_night_mode(blst_sdk_t *sdk, blst_visible_night_mode_t mode);
blst_ack_code_t blst_sdk_visible_camera_set_night_mode_wait_ack(blst_sdk_t *sdk, blst_visible_night_mode_t mode, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_night_fps(blst_sdk_t *sdk, blst_visible_night_fps_t fps);
blst_ack_code_t blst_sdk_visible_camera_set_night_fps_wait_ack(blst_sdk_t *sdk, blst_visible_night_fps_t fps, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_fusion_ir(blst_sdk_t *sdk, blst_visible_fusion_ir_switch_t sw, uint8_t ir_weight, uint8_t visible_weight);
blst_ack_code_t blst_sdk_visible_camera_set_fusion_ir_wait_ack(blst_sdk_t *sdk, blst_visible_fusion_ir_switch_t sw, uint8_t ir_weight, uint8_t visible_weight, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_fusion_ir_debug(blst_sdk_t *sdk, blst_visible_fusion_ir_debug_action_t action, int16_t ir_offset_x, int16_t ir_offset_y, int16_t rotation_angle);
blst_ack_code_t blst_sdk_visible_camera_set_fusion_ir_debug_wait_ack(blst_sdk_t *sdk, blst_visible_fusion_ir_debug_action_t action, int16_t ir_offset_x, int16_t ir_offset_y, int16_t rotation_angle, uint32_t timeout_ms);
int blst_sdk_visible_camera_set_fusion_ir_grayscale(blst_sdk_t *sdk, blst_visible_fusion_ir_grayscale_t grayscale);
blst_ack_code_t blst_sdk_visible_camera_set_fusion_ir_grayscale_wait_ack(blst_sdk_t *sdk, blst_visible_fusion_ir_grayscale_t grayscale, uint32_t timeout_ms);

/* Section 3.2.4.1-3.2.4.13: camera common control messages */
int blst_sdk_camera_set_photo_record_mode(blst_sdk_t *sdk, blst_camera_photo_record_mode_t mode);
blst_ack_code_t blst_sdk_camera_set_photo_record_mode_wait_ack(blst_sdk_t *sdk, blst_camera_photo_record_mode_t mode, uint32_t timeout_ms);
int blst_sdk_camera_set_photo_params(blst_sdk_t *sdk, blst_camera_photo_mode_t mode, uint8_t delay_seconds, uint8_t burst_count);
blst_ack_code_t blst_sdk_camera_set_photo_params_wait_ack(blst_sdk_t *sdk, blst_camera_photo_mode_t mode, uint8_t delay_seconds, uint8_t burst_count, uint32_t timeout_ms);
int blst_sdk_camera_take_photo(blst_sdk_t *sdk, blst_camera_capture_mode_t mode, const char *folder_name, const char *photo_name);
blst_ack_code_t blst_sdk_camera_take_photo_wait_ack(blst_sdk_t *sdk, blst_camera_capture_mode_t mode, const char *folder_name, const char *photo_name, uint32_t timeout_ms);
int blst_sdk_camera_stop_photo(blst_sdk_t *sdk, blst_camera_capture_mode_t mode);
blst_ack_code_t blst_sdk_camera_stop_photo_wait_ack(blst_sdk_t *sdk, blst_camera_capture_mode_t mode, uint32_t timeout_ms);
int blst_sdk_camera_record(blst_sdk_t *sdk, blst_camera_record_mode_t mode, blst_camera_record_command_t command, const char *folder_name, const char *video_name);
blst_ack_code_t blst_sdk_camera_record_wait_ack(blst_sdk_t *sdk, blst_camera_record_mode_t mode, blst_camera_record_command_t command, const char *folder_name, const char *video_name, uint32_t timeout_ms);
/* Protocol Byte1 is zoom speed in newer public protocol; the legacy mode parameter is interpreted as speed. */
int blst_sdk_camera_set_hybrid_zoom(blst_sdk_t *sdk, blst_camera_zoom_mode_t mode, uint16_t zoom_x10);
blst_ack_code_t blst_sdk_camera_set_hybrid_zoom_wait_ack(blst_sdk_t *sdk, blst_camera_zoom_mode_t mode, uint16_t zoom_x10, uint32_t timeout_ms);
int blst_sdk_camera_set_hybrid_zoom_with_speed(blst_sdk_t *sdk, blst_camera_zoom_speed_t speed, uint16_t zoom_x10);
blst_ack_code_t blst_sdk_camera_set_hybrid_zoom_with_speed_wait_ack(blst_sdk_t *sdk, blst_camera_zoom_speed_t speed, uint16_t zoom_x10, uint32_t timeout_ms);
/* The mode parameter is ignored by newer public protocol; Byte2 is reserved and set to 0. */
int blst_sdk_camera_control_zoom(blst_sdk_t *sdk, blst_camera_zoom_control_t control, blst_camera_zoom_mode_t mode);
blst_ack_code_t blst_sdk_camera_control_zoom_wait_ack(blst_sdk_t *sdk, blst_camera_zoom_control_t control, blst_camera_zoom_mode_t mode, uint32_t timeout_ms);
int blst_sdk_camera_control_zoom_simple(blst_sdk_t *sdk, blst_camera_zoom_control_t control);
blst_ack_code_t blst_sdk_camera_control_zoom_simple_wait_ack(blst_sdk_t *sdk, blst_camera_zoom_control_t control, uint32_t timeout_ms);
int blst_sdk_camera_precise_retake(blst_sdk_t *sdk, blst_camera_capture_mode_t mode, blst_visible_photo_resolution_t photo_resolution, uint16_t visible_zoom_x10, uint16_t precise_focal_length, const char *folder_name, const char *photo_name);
blst_ack_code_t blst_sdk_camera_precise_retake_wait_ack(blst_sdk_t *sdk, blst_camera_capture_mode_t mode, blst_visible_photo_resolution_t photo_resolution, uint16_t visible_zoom_x10, uint16_t precise_focal_length, const char *folder_name, const char *photo_name, uint32_t timeout_ms);
int blst_sdk_camera_set_video_bitrate(blst_sdk_t *sdk, blst_camera_video_bitrate_t bitrate);
blst_ack_code_t blst_sdk_camera_set_video_bitrate_wait_ack(blst_sdk_t *sdk, blst_camera_video_bitrate_t bitrate, uint32_t timeout_ms);
int blst_sdk_camera_set_video_output_resolution(blst_sdk_t *sdk, blst_camera_video_output_resolution_t resolution);
blst_ack_code_t blst_sdk_camera_set_video_output_resolution_wait_ack(blst_sdk_t *sdk, blst_camera_video_output_resolution_t resolution, uint32_t timeout_ms);
int blst_sdk_camera_set_video_codec(blst_sdk_t *sdk, blst_camera_codec_t codec);
blst_ack_code_t blst_sdk_camera_set_video_codec_wait_ack(blst_sdk_t *sdk, blst_camera_codec_t codec, uint32_t timeout_ms);
int blst_sdk_camera_format_tf_card(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_format_tf_card_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_sync_time(blst_sdk_t *sdk, uint64_t timestamp_ms, int32_t timezone_minutes, int32_t daylight_saving);
blst_ack_code_t blst_sdk_camera_sync_time_wait_ack(blst_sdk_t *sdk, uint64_t timestamp_ms, int32_t timezone_minutes, int32_t daylight_saving, uint32_t timeout_ms);
int blst_sdk_camera_factory_reset(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_factory_reset_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);

/* Section 3.2.4.14-3.2.4.23: camera common advanced control messages */
int blst_sdk_camera_request_gps_info(blst_sdk_t *sdk, uint8_t lens_enabled, uint8_t hour, uint8_t minute, uint8_t second, uint16_t millisecond);
blst_ack_code_t blst_sdk_camera_request_gps_info_wait_ack(blst_sdk_t *sdk, uint8_t lens_enabled, uint8_t hour, uint8_t minute, uint8_t second, uint16_t millisecond, uint32_t timeout_ms);
int blst_sdk_camera_set_ip(blst_sdk_t *sdk, blst_camera_ip_type_t ip_type, const blst_camera_ip_address_t *ip, const blst_camera_ip_address_t *netmask, const blst_camera_ip_address_t *gateway);
blst_ack_code_t blst_sdk_camera_set_ip_wait_ack(blst_sdk_t *sdk, blst_camera_ip_type_t ip_type, const blst_camera_ip_address_t *ip, const blst_camera_ip_address_t *netmask, const blst_camera_ip_address_t *gateway, uint32_t timeout_ms);
int blst_sdk_camera_get_ip(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_get_ip_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_focus(blst_sdk_t *sdk, blst_camera_focus_mode_t mode, uint16_t left_top_x, uint16_t left_top_y, uint16_t right_bottom_x, uint16_t right_bottom_y, blst_camera_focus_type_t focus_type);
blst_ack_code_t blst_sdk_camera_focus_wait_ack(blst_sdk_t *sdk, blst_camera_focus_mode_t mode, uint16_t left_top_x, uint16_t left_top_y, uint16_t right_bottom_x, uint16_t right_bottom_y, blst_camera_focus_type_t focus_type, uint32_t timeout_ms);
int blst_sdk_camera_focus_manual_value(blst_sdk_t *sdk, uint16_t focus_value);
blst_ack_code_t blst_sdk_camera_focus_manual_value_wait_ack(blst_sdk_t *sdk, uint16_t focus_value, uint32_t timeout_ms);
int blst_sdk_camera_set_osd_master_switch(blst_sdk_t *sdk, blst_camera_osd_master_switch_t sw);
blst_ack_code_t blst_sdk_camera_set_osd_master_switch_wait_ack(blst_sdk_t *sdk, blst_camera_osd_master_switch_t sw, uint32_t timeout_ms);
int blst_sdk_camera_power_control(blst_sdk_t *sdk, blst_camera_power_action_t action);
blst_ack_code_t blst_sdk_camera_power_control_wait_ack(blst_sdk_t *sdk, blst_camera_power_action_t action, uint32_t timeout_ms);
int blst_sdk_camera_get_version(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_get_version_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_set_image_mode(blst_sdk_t *sdk, blst_camera_image_mode_t mode);
blst_ack_code_t blst_sdk_camera_set_image_mode_wait_ack(blst_sdk_t *sdk, blst_camera_image_mode_t mode, uint32_t timeout_ms);
int blst_sdk_camera_set_ai_recognition(blst_sdk_t *sdk, blst_camera_ai_switch_t sw, uint8_t model_type, const uint8_t category_mask[16]);
blst_ack_code_t blst_sdk_camera_set_ai_recognition_wait_ack(blst_sdk_t *sdk, blst_camera_ai_switch_t sw, uint8_t model_type, const uint8_t category_mask[16], uint32_t timeout_ms);


/* Section 3.2.4.24-3.2.4.30: target GPS, tracking, core temperature and OSD configuration */
int blst_sdk_camera_request_target_gps(blst_sdk_t *sdk, const blst_camera_target_gps_request_t *request);
blst_ack_code_t blst_sdk_camera_request_target_gps_wait_ack(blst_sdk_t *sdk, const blst_camera_target_gps_request_t *request, uint32_t timeout_ms);
int blst_sdk_camera_track_target(blst_sdk_t *sdk, blst_camera_track_switch_t sw, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
blst_ack_code_t blst_sdk_camera_track_target_wait_ack(blst_sdk_t *sdk, blst_camera_track_switch_t sw, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t timeout_ms);
int blst_sdk_camera_get_core_temperature(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_get_core_temperature_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_set_osd_config(blst_sdk_t *sdk, uint8_t item_id, uint8_t enabled, blst_camera_osd_position_t position, blst_camera_osd_color_t color);
blst_ack_code_t blst_sdk_camera_set_osd_config_wait_ack(blst_sdk_t *sdk, uint8_t item_id, uint8_t enabled, blst_camera_osd_position_t position, blst_camera_osd_color_t color, uint32_t timeout_ms);
int blst_sdk_camera_set_osd_custom_text(blst_sdk_t *sdk, uint8_t text_index, const char *text);
blst_ack_code_t blst_sdk_camera_set_osd_custom_text_wait_ack(blst_sdk_t *sdk, uint8_t text_index, const char *text, uint32_t timeout_ms);

/* Section 3.2.4.31-3.2.4.39: media playback/download control messages */
int blst_sdk_camera_set_playback_download_mode(blst_sdk_t *sdk, blst_camera_playback_download_mode_t mode);
blst_ack_code_t blst_sdk_camera_set_playback_download_mode_wait_ack(blst_sdk_t *sdk, blst_camera_playback_download_mode_t mode, uint32_t timeout_ms);
int blst_sdk_camera_refresh_media_library(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_refresh_media_library_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_delete_media(blst_sdk_t *sdk, const char *absolute_path);
blst_ack_code_t blst_sdk_camera_delete_media_wait_ack(blst_sdk_t *sdk, const char *absolute_path, uint32_t timeout_ms);
int blst_sdk_camera_download_media(blst_sdk_t *sdk, blst_camera_media_download_type_t media_type, uint16_t packet_size, uint32_t start_packet_index, uint8_t packet_count, const char *absolute_path);
blst_ack_code_t blst_sdk_camera_download_media_wait_ack(blst_sdk_t *sdk, blst_camera_media_download_type_t media_type, uint16_t packet_size, uint32_t start_packet_index, uint8_t packet_count, const char *absolute_path, uint32_t timeout_ms);
int blst_sdk_camera_start_playback(blst_sdk_t *sdk, const char *absolute_path);
blst_ack_code_t blst_sdk_camera_start_playback_wait_ack(blst_sdk_t *sdk, const char *absolute_path, uint32_t timeout_ms);
int blst_sdk_camera_stop_playback(blst_sdk_t *sdk, const char *absolute_path);
blst_ack_code_t blst_sdk_camera_stop_playback_wait_ack(blst_sdk_t *sdk, const char *absolute_path, uint32_t timeout_ms);
int blst_sdk_camera_pause_playback(blst_sdk_t *sdk, const char *absolute_path);
blst_ack_code_t blst_sdk_camera_pause_playback_wait_ack(blst_sdk_t *sdk, const char *absolute_path, uint32_t timeout_ms);
int blst_sdk_camera_seek_playback(blst_sdk_t *sdk, uint32_t seek_seconds, const char *absolute_path);
blst_ack_code_t blst_sdk_camera_seek_playback_wait_ack(blst_sdk_t *sdk, uint32_t seek_seconds, const char *absolute_path, uint32_t timeout_ms);

/* Section 3.2.4.40-3.2.4.47: client endpoint, query and report switch messages */
int blst_sdk_camera_set_media_client(blst_sdk_t *sdk, blst_camera_media_client_set_type_t type, const blst_camera_ip_address_t *ip, uint16_t port);
blst_ack_code_t blst_sdk_camera_set_media_client_wait_ack(blst_sdk_t *sdk, blst_camera_media_client_set_type_t type, const blst_camera_ip_address_t *ip, uint16_t port, uint32_t timeout_ms);
int blst_sdk_camera_get_osd_config(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_get_osd_config_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_get_ai_category(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_get_ai_category_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_get_core_sn(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_get_core_sn_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_get_media_client(blst_sdk_t *sdk, blst_camera_media_client_get_type_t type);
blst_ack_code_t blst_sdk_camera_get_media_client_wait_ack(blst_sdk_t *sdk, blst_camera_media_client_get_type_t type, uint32_t timeout_ms);
int blst_sdk_camera_get_kernel_version(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_get_kernel_version_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_set_target_gps_report_switch(blst_sdk_t *sdk, blst_camera_target_gps_report_switch_t sw);
blst_ack_code_t blst_sdk_camera_set_target_gps_report_switch_wait_ack(blst_sdk_t *sdk, blst_camera_target_gps_report_switch_t sw, uint32_t timeout_ms);
int blst_sdk_camera_refresh_idr(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_camera_refresh_idr_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_camera_set_dynamic_bitrate(blst_sdk_t *sdk, uint16_t bitrate_kbps);
blst_ack_code_t blst_sdk_camera_set_dynamic_bitrate_wait_ack(blst_sdk_t *sdk, uint16_t bitrate_kbps, uint32_t timeout_ms);
int blst_sdk_camera_set_split_link_zoom(blst_sdk_t *sdk, blst_camera_split_link_zoom_switch_t sw);
blst_ack_code_t blst_sdk_camera_set_split_link_zoom_wait_ack(blst_sdk_t *sdk, blst_camera_split_link_zoom_switch_t sw, uint32_t timeout_ms);
int blst_sdk_camera_imx989_focus(blst_sdk_t *sdk, blst_camera_imx989_focus_mode_t mode, uint16_t focus_value);
blst_ack_code_t blst_sdk_camera_imx989_focus_wait_ack(blst_sdk_t *sdk, blst_camera_imx989_focus_mode_t mode, uint16_t focus_value, uint32_t timeout_ms);

/* Section 3.3.1: laser payload protocol */
int blst_sdk_laser_set_range(blst_sdk_t *sdk, blst_laser_range_mode_t mode);
blst_ack_code_t blst_sdk_laser_set_range_wait_ack(blst_sdk_t *sdk, blst_laser_range_mode_t mode, uint32_t timeout_ms);
int blst_sdk_laser_set_periodic_range(blst_sdk_t *sdk, blst_laser_periodic_range_mode_t mode);
blst_ack_code_t blst_sdk_laser_set_periodic_range_wait_ack(blst_sdk_t *sdk, blst_laser_periodic_range_mode_t mode, uint32_t timeout_ms);

/* Section 3.3.2: SBUS channel protocol */
int blst_sdk_sbus_set_range(blst_sdk_t *sdk, uint16_t max_value, uint16_t min_value);
blst_ack_code_t blst_sdk_sbus_set_range_wait_ack(blst_sdk_t *sdk, uint16_t max_value, uint16_t min_value, uint32_t timeout_ms);
int blst_sdk_sbus_config_channels(blst_sdk_t *sdk, const blst_sbus_channel_config_t *config);
blst_ack_code_t blst_sdk_sbus_config_channels_wait_ack(blst_sdk_t *sdk, const blst_sbus_channel_config_t *config, uint32_t timeout_ms);
int blst_sdk_sbus_get_config(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_sbus_get_config_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);
int blst_sdk_sbus_set_switch(blst_sdk_t *sdk, blst_sbus_switch_t sw);
blst_ack_code_t blst_sdk_sbus_set_switch_wait_ack(blst_sdk_t *sdk, blst_sbus_switch_t sw, uint32_t timeout_ms);
int blst_sdk_sbus_set_zoom_channel(blst_sdk_t *sdk, uint8_t channel);
blst_ack_code_t blst_sdk_sbus_set_zoom_channel_wait_ack(blst_sdk_t *sdk, uint8_t channel, uint32_t timeout_ms);
int blst_sdk_sbus_get_zoom_channel(blst_sdk_t *sdk);
blst_ack_code_t blst_sdk_sbus_get_zoom_channel_wait_ack(blst_sdk_t *sdk, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
