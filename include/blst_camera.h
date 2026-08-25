#ifndef BLST_CAMERA_H
#define BLST_CAMERA_H

#include "blst_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 3.2.1 相机载荷协议：周期性状态上报消息 */
#define BLST_MSG_ID_CAMERA_SYSTEM_STATUS   0x000003u
#define BLST_MSG_ID_IR_CAMERA_STATUS       0x000004u
#define BLST_MSG_ID_VISIBLE_CAMERA_STATUS  0x000005u
#define BLST_MSG_ID_CAMERA_UPGRADE_STATUS  0x000008u

/* 3.2.2 红外相机设置消息 */
#define BLST_MSG_ID_IR_PARAMS_GET             0x000100u
#define BLST_MSG_ID_IR_DIGITAL_ZOOM_SET       0x000105u
#define BLST_MSG_ID_IR_PSEUDO_COLOR_SET       0x000106u
#define BLST_MSG_ID_IR_TEMP_MEASURE_SWITCH    0x000108u
#define BLST_MSG_ID_IR_SHARPNESS_SET          0x00010Au
#define BLST_MSG_ID_IR_BRIGHTNESS_SET         0x00010Bu
#define BLST_MSG_ID_IR_CONTRAST_SET           0x00010Cu
#define BLST_MSG_ID_IR_DENOISE_SET            0x00010Du
#define BLST_MSG_ID_IR_ENHANCEMENT_SET        0x00010Eu
#define BLST_MSG_ID_IR_POINT_TEMP_SET         0x00010Fu
#define BLST_MSG_ID_IR_AREA_TEMP_SET          0x000110u
#define BLST_MSG_ID_IR_GAIN_MODE_SET          0x000123u
#define BLST_MSG_ID_IR_TEMP_OVERLAY_SET       0x000125u
#define BLST_MSG_ID_IR_FUSION_VISIBLE_SET     0x000127u
#define BLST_MSG_ID_IR_FUSION_VISIBLE_DEBUG_SET 0x000128u
#define BLST_MSG_ID_IR_FUSION_VISIBLE_GRAYSCALE_SET 0x000129u
#define BLST_MSG_ID_IR_ISOTHERM_SET           0x00012Au
#define BLST_MSG_ID_IR_AI_SUPER_RESOLUTION_SET 0x00012Bu

/* 3.2.3 可见光相机设置消息 */
#define BLST_MSG_ID_VISIBLE_PARAMS_GET              0x000200u
#define BLST_MSG_ID_VISIBLE_RECORD_RESOLUTION_SET   0x000201u
#define BLST_MSG_ID_VISIBLE_PHOTO_RESOLUTION_SET    0x000202u
#define BLST_MSG_ID_VISIBLE_ISO_SET                 0x000203u
#define BLST_MSG_ID_VISIBLE_SHUTTER_SET             0x000204u
#define BLST_MSG_ID_VISIBLE_EV_SET                  0x000205u
#define BLST_MSG_ID_VISIBLE_WHITE_BALANCE_SET       0x000206u
#define BLST_MSG_ID_VISIBLE_ANTIFLICKER_SET         0x000207u
#define BLST_MSG_ID_VISIBLE_HIGHLIGHT_SUPPRESS_SET  0x000208u
#define BLST_MSG_ID_VISIBLE_BACKLIGHT_COMP_SET      0x000209u
#define BLST_MSG_ID_VISIBLE_AE_LOCK_SET             0x00020Au
#define BLST_MSG_ID_VISIBLE_METERING_MODE_SET       0x00020Bu
#define BLST_MSG_ID_VISIBLE_NIGHT_MODE_SET          0x00020Eu
#define BLST_MSG_ID_VISIBLE_NIGHT_FPS_SET           0x00020Fu
#define BLST_MSG_ID_VISIBLE_FUSION_IR_SET           0x000212u
#define BLST_MSG_ID_VISIBLE_FUSION_IR_DEBUG_SET     0x000213u
#define BLST_MSG_ID_VISIBLE_FUSION_IR_GRAYSCALE_SET 0x000214u

/* 3.2.4 通用部分 */
#define BLST_MSG_ID_CAMERA_PHOTO_RECORD_MODE_SET    0x000300u
#define BLST_MSG_ID_CAMERA_PHOTO_PARAMS_SET         0x000301u
#define BLST_MSG_ID_CAMERA_PHOTO_CAPTURE            0x000302u
#define BLST_MSG_ID_CAMERA_RECORD                   0x000303u
#define BLST_MSG_ID_CAMERA_HYBRID_ZOOM_SET          0x000304u
#define BLST_MSG_ID_CAMERA_CONTINUOUS_ZOOM          0x000306u
#define BLST_MSG_ID_CAMERA_PRECISE_RETAKE           0x000307u
#define BLST_MSG_ID_CAMERA_VIDEO_BITRATE_SET        0x000308u
#define BLST_MSG_ID_CAMERA_VIDEO_OUTPUT_RES_SET     0x00030Au
#define BLST_MSG_ID_CAMERA_VIDEO_CODEC_SET          0x00030Bu
#define BLST_MSG_ID_CAMERA_TF_CARD_FORMAT           0x00030Du
#define BLST_MSG_ID_CAMERA_TIME_SYNC                0x00030Eu
#define BLST_MSG_ID_CAMERA_FACTORY_RESET            0x00030Fu
#define BLST_MSG_ID_CAMERA_GPS_INFO_REQUEST        0x000310u
#define BLST_MSG_ID_CAMERA_IP_SET                  0x000311u
#define BLST_MSG_ID_CAMERA_IP_GET                  0x000312u
#define BLST_MSG_ID_CAMERA_FOCUS                   0x000313u
#define BLST_MSG_ID_CAMERA_OSD_MASTER_SWITCH       0x000314u
#define BLST_MSG_ID_CAMERA_POWER_CONTROL           0x000316u
#define BLST_MSG_ID_CAMERA_VERSION_GET             0x000317u
#define BLST_MSG_ID_CAMERA_IMAGE_MODE_SET          0x000318u
#define BLST_MSG_ID_CAMERA_AI_RECOGNITION          0x000319u
#define BLST_MSG_ID_CAMERA_TARGET_GPS_REQUEST      0x000320u
#define BLST_MSG_ID_CAMERA_TRACK_TARGET            0x000324u
#define BLST_MSG_ID_CAMERA_CORE_TEMPERATURE_GET    0x000325u
#define BLST_MSG_ID_CAMERA_OSD_CONFIG_SET          0x000329u
#define BLST_MSG_ID_CAMERA_OSD_CUSTOM_TEXT_SET     0x00032Au
#define BLST_MSG_ID_CAMERA_PLAYBACK_DOWNLOAD_MODE  0x00032Bu
#define BLST_MSG_ID_CAMERA_MEDIA_LIBRARY_REFRESH   0x00032Cu
#define BLST_MSG_ID_CAMERA_MEDIA_DELETE            0x00032Du
#define BLST_MSG_ID_CAMERA_MEDIA_DOWNLOAD          0x00032Eu
#define BLST_MSG_ID_CAMERA_PLAYBACK_START          0x00032Fu
#define BLST_MSG_ID_CAMERA_PLAYBACK_STOP           0x000330u
#define BLST_MSG_ID_CAMERA_PLAYBACK_PAUSE          0x000331u
#define BLST_MSG_ID_CAMERA_PLAYBACK_SEEK           0x000332u
#define BLST_MSG_ID_CAMERA_MEDIA_PLAYBACK_STATUS   0x000333u
#define BLST_MSG_ID_CAMERA_MEDIA_CLIENT_SET         0x000334u
#define BLST_MSG_ID_CAMERA_OSD_CONFIG_GET          0x000335u
#define BLST_MSG_ID_CAMERA_AI_CATEGORY_GET         0x000336u
#define BLST_MSG_ID_CAMERA_CORE_SN_GET             0x000337u
#define BLST_MSG_ID_CAMERA_MEDIA_CLIENT_GET        0x000339u
#define BLST_MSG_ID_CAMERA_KERNEL_VERSION_GET      0x00033Au
#define BLST_MSG_ID_CAMERA_TARGET_GPS_REPORT_SWITCH 0x00033Bu
#define BLST_MSG_ID_CAMERA_IDR_REFRESH             0x00033Du
#define BLST_MSG_ID_CAMERA_DYNAMIC_BITRATE_SET     0x00033Eu
#define BLST_MSG_ID_CAMERA_SPLIT_LINK_ZOOM_SET     0x00033Fu
#define BLST_MSG_ID_CAMERA_IMX989_FOCUS            0x000340u

#define BLST_CAMERA_SYSTEM_STATUS_PAYLOAD_LEN   16u
#define BLST_IR_CAMERA_STATUS_PAYLOAD_LEN       34u
#define BLST_VISIBLE_CAMERA_STATUS_PAYLOAD_LEN  15u
#define BLST_CAMERA_UPGRADE_STATUS_PAYLOAD_LEN  3u

#define BLST_IR_SIMPLE_PAYLOAD_LEN             2u
#define BLST_IR_PARAMS_GET_PAYLOAD_LEN         2u
#define BLST_IR_DENOISE_PAYLOAD_LEN            3u
#define BLST_IR_POINT_TEMP_PAYLOAD_LEN         5u
#define BLST_IR_AREA_TEMP_PAYLOAD_LEN          9u
#define BLST_IR_PARAMS_ACK_PAYLOAD_LEN         23u
#define BLST_IR_FUSION_VISIBLE_PAYLOAD_LEN     4u
#define BLST_IR_FUSION_VISIBLE_DEBUG_PAYLOAD_LEN 7u
#define BLST_IR_FUSION_VISIBLE_GRAYSCALE_PAYLOAD_LEN 2u
#define BLST_IR_ISOTHERM_PAYLOAD_LEN           6u
#define BLST_IR_AI_SUPER_RESOLUTION_PAYLOAD_LEN 2u

#define BLST_VISIBLE_SIMPLE_PAYLOAD_LEN        2u
#define BLST_VISIBLE_WHITE_BALANCE_PAYLOAD_LEN 4u
#define BLST_VISIBLE_LEVEL_SWITCH_PAYLOAD_LEN  3u
#define BLST_VISIBLE_PARAMS_ACK_PAYLOAD_LEN    21u
#define BLST_VISIBLE_NIGHT_MODE_STATUS_PAYLOAD_LEN 3u
#define BLST_VISIBLE_FUSION_IR_PAYLOAD_LEN     4u
#define BLST_VISIBLE_FUSION_IR_DEBUG_PAYLOAD_LEN 7u
#define BLST_VISIBLE_FUSION_IR_GRAYSCALE_PAYLOAD_LEN 2u

#define BLST_CAMERA_COMMON_SIMPLE_PAYLOAD_LEN       2u
#define BLST_CAMERA_PHOTO_PARAMS_PAYLOAD_LEN        4u
#define BLST_CAMERA_MEDIA_NAME_FOLDER_LEN           20u
#define BLST_CAMERA_MEDIA_NAME_FILE_LEN             32u
#define BLST_CAMERA_PHOTO_COMMAND_PAYLOAD_LEN       54u
#define BLST_CAMERA_RECORD_COMMAND_PAYLOAD_LEN      54u
#define BLST_CAMERA_HYBRID_ZOOM_PAYLOAD_LEN         3u
#define BLST_CAMERA_CONTINUOUS_ZOOM_PAYLOAD_LEN     2u
#define BLST_CAMERA_PRECISE_RETAKE_PAYLOAD_LEN      58u
#define BLST_CAMERA_TIME_SYNC_PAYLOAD_LEN           16u
#define BLST_CAMERA_PHOTO_STATUS_PAYLOAD_LEN        7u
#define BLST_CAMERA_RECORD_STATUS_PAYLOAD_LEN       7u
#define BLST_CAMERA_HYBRID_ZOOM_STATUS_PAYLOAD_LEN  2u
#define BLST_CAMERA_TF_FORMAT_STATUS_PAYLOAD_LEN    3u
#define BLST_CAMERA_GPS_INFO_REQUEST_PAYLOAD_LEN   6u
#define BLST_CAMERA_GPS_INFO_ACK_PAYLOAD_LEN       31u
#define BLST_CAMERA_IP_SET_PAYLOAD_LEN             13u
#define BLST_CAMERA_IP_GET_PAYLOAD_LEN             2u
#define BLST_CAMERA_IP_GET_ACK_PAYLOAD_LEN         14u
#define BLST_CAMERA_FOCUS_PAYLOAD_LEN              10u
#define BLST_CAMERA_OSD_MASTER_SWITCH_PAYLOAD_LEN  3u
#define BLST_CAMERA_VERSION_ACK_PAYLOAD_LEN        5u
#define BLST_CAMERA_AI_RECOGNITION_PAYLOAD_LEN     18u
#define BLST_CAMERA_AI_MAX_TARGETS                 24u
#define BLST_CAMERA_AI_TARGET_PAYLOAD_LEN          10u
#define BLST_CAMERA_AI_STATUS_MIN_PAYLOAD_LEN      2u
#define BLST_CAMERA_TARGET_GPS_REQUEST_PAYLOAD_LEN 42u
#define BLST_CAMERA_TARGET_GPS_ACK_PAYLOAD_LEN     27u
#define BLST_CAMERA_TRACK_TARGET_PAYLOAD_LEN       10u
#define BLST_CAMERA_TRACK_STATUS_MIN_PAYLOAD_LEN   2u
#define BLST_CAMERA_CORE_TEMP_GET_PAYLOAD_LEN      2u
#define BLST_CAMERA_CORE_TEMP_ACK_PAYLOAD_LEN      4u
#define BLST_CAMERA_OSD_CONFIG_PAYLOAD_LEN         5u
#define BLST_CAMERA_OSD_CUSTOM_TEXT_PAYLOAD_LEN    66u
#define BLST_CAMERA_OSD_CUSTOM_TEXT_MAX_LEN        64u
#define BLST_CAMERA_MEDIA_PATH_MAX_LEN             246u
#define BLST_CAMERA_MEDIA_DELETE_MIN_PAYLOAD_LEN   2u
#define BLST_CAMERA_MEDIA_DOWNLOAD_MIN_PAYLOAD_LEN 10u
#define BLST_CAMERA_PLAYBACK_SEEK_MIN_PAYLOAD_LEN  6u
#define BLST_CAMERA_MEDIA_PLAYBACK_STATUS_PAYLOAD_LEN 10u
#define BLST_CAMERA_MEDIA_CLIENT_SET_PAYLOAD_LEN      8u
#define BLST_CAMERA_MEDIA_CLIENT_GET_PAYLOAD_LEN      2u
#define BLST_CAMERA_MEDIA_CLIENT_GET_ACK_PAYLOAD_LEN  7u
#define BLST_CAMERA_OSD_CONFIG_GET_PAYLOAD_LEN        2u
#define BLST_CAMERA_OSD_CONFIG_COUNT                  15u
#define BLST_CAMERA_OSD_CONFIG_GET_ACK_PAYLOAD_LEN    31u
#define BLST_CAMERA_AI_CATEGORY_GET_PAYLOAD_LEN       2u
#define BLST_CAMERA_AI_CATEGORY_ACK_PAYLOAD_LEN       17u
#define BLST_CAMERA_CORE_SN_GET_PAYLOAD_LEN           2u
#define BLST_CAMERA_CORE_SN_ACK_PAYLOAD_LEN           20u
#define BLST_CAMERA_CORE_SN_STRING_LEN                20u
#define BLST_CAMERA_KERNEL_VERSION_GET_PAYLOAD_LEN    2u
#define BLST_CAMERA_TARGET_GPS_REPORT_SWITCH_PAYLOAD_LEN 2u
#define BLST_CAMERA_IDR_REFRESH_PAYLOAD_LEN           2u
#define BLST_CAMERA_DYNAMIC_BITRATE_PAYLOAD_LEN       3u
#define BLST_CAMERA_SPLIT_LINK_ZOOM_PAYLOAD_LEN       2u
#define BLST_CAMERA_IMX989_FOCUS_PAYLOAD_LEN          3u

typedef enum {
    BLST_CAMERA_MODE_PHOTO = 0x00,
    BLST_CAMERA_MODE_RECORD = 0x01
} blst_camera_photo_record_mode_t;

typedef enum {
    BLST_CAMERA_NET_RES_1080P30 = 0x00,
    BLST_CAMERA_NET_RES_720P30 = 0x01
} blst_camera_network_resolution_t;

typedef enum {
    BLST_CAMERA_CODEC_H264 = 0x00,
    BLST_CAMERA_CODEC_H265 = 0x01
} blst_camera_codec_t;

typedef enum {
    BLST_CAMERA_STREAM_IR = 0x00,
    BLST_CAMERA_STREAM_VISIBLE_0X05 = 0x05,
    BLST_CAMERA_STREAM_VISIBLE_0X06 = 0x06,
    BLST_CAMERA_STREAM_SPLIT = 0x07
} blst_camera_stream_mode_t;

typedef enum {
    BLST_CAMERA_BITRATE_1M = 0x01,
    BLST_CAMERA_BITRATE_1_5M = 0x02,
    BLST_CAMERA_BITRATE_2M = 0x03,
    BLST_CAMERA_BITRATE_4M = 0x04,
    BLST_CAMERA_BITRATE_8M = 0x05,
    BLST_CAMERA_BITRATE_12M = 0x06
} blst_camera_video_bitrate_t;

typedef enum {
    BLST_CAMERA_PHOTO_SINGLE = 0x00,
    BLST_CAMERA_PHOTO_BURST = 0x01,
    BLST_CAMERA_PHOTO_DELAY = 0x02
} blst_camera_photo_mode_t;

typedef enum {
    BLST_CAMERA_SD_OK = 0x00,
    BLST_CAMERA_SD_ABNORMAL = 0x01,
    BLST_CAMERA_SD_SLOW = 0x02,
    BLST_CAMERA_SD_NOT_INSERTED = 0x03,
    BLST_CAMERA_SD_FULL = 0x04,
    BLST_CAMERA_SD_FORMAT_ERROR = 0x05
} blst_camera_sd_status_t;

typedef enum {
    BLST_CAMERA_VIDEO_STREAM_PLAYBACK_DOWNLOAD = 0x01,
    BLST_CAMERA_VIDEO_STREAM_NORMAL = 0x02
} blst_camera_video_stream_status_t;


typedef enum {
    BLST_CAMERA_UPGRADE_REPAIR_SUCCESS = 0x00,
    BLST_CAMERA_UPGRADE_REPAIR_FAILED = 0x01,
    BLST_CAMERA_UPGRADING = 0x02,
    BLST_CAMERA_REPAIRING = 0x03
} blst_camera_upgrade_repair_status_t;

typedef enum {
    BLST_IR_TEMP_MEASURE_ON = 0x00,
    BLST_IR_TEMP_MEASURE_OFF = 0x01
} blst_ir_temp_measure_switch_t;

typedef enum {
    BLST_IR_GAIN_HIGH = 0x00,
    BLST_IR_GAIN_LOW = 0x01,
    BLST_IR_GAIN_AUTO = 0x02
} blst_ir_gain_mode_t;

typedef enum {
    BLST_IR_TEMP_OVERLAY_OFF = 0x00,
    BLST_IR_TEMP_OVERLAY_ON = 0x01
} blst_ir_temp_overlay_switch_t;

typedef enum {
    BLST_IR_FUSION_VISIBLE_OFF = 0x00,
    BLST_IR_FUSION_VISIBLE_ON = 0x01
} blst_ir_fusion_visible_switch_t;

typedef enum {
    BLST_IR_FUSION_VISIBLE_DEBUG_ADJUST = 0x01,
    BLST_IR_FUSION_VISIBLE_DEBUG_SAVE = 0x02
} blst_ir_fusion_visible_debug_action_t;

typedef enum {
    BLST_IR_FUSION_VISIBLE_GRAYSCALE_OFF = 0x00,
    BLST_IR_FUSION_VISIBLE_GRAYSCALE_ON = 0x01
} blst_ir_fusion_visible_grayscale_t;

typedef enum {
    BLST_IR_ISOTHERM_OFF = 0x00,
    BLST_IR_ISOTHERM_ON = 0x01
} blst_ir_isotherm_switch_t;

typedef enum {
    BLST_IR_AI_SUPER_RESOLUTION_OFF = 0x00,
    BLST_IR_AI_SUPER_RESOLUTION_ON = 0x01
} blst_ir_ai_super_resolution_switch_t;

typedef enum {
    BLST_VISIBLE_ZOOM_DONE = 0x00,
    BLST_VISIBLE_ZOOMING = 0x01
} blst_visible_zoom_status_t;

typedef enum {
    BLST_VISIBLE_EV_AUTO = 0x00,
    BLST_VISIBLE_EV_PLUS_2 = 0x0A,
    BLST_VISIBLE_EV_PLUS_1 = 0x10,
    BLST_VISIBLE_EV_0 = 0x16,
    BLST_VISIBLE_EV_MINUS_1 = 0x1C,
    BLST_VISIBLE_EV_MINUS_2 = 0x23
} blst_visible_ev_t;

typedef enum {
    BLST_VISIBLE_ISO_AUTO = 0x00,
    BLST_VISIBLE_ISO_100 = 0x01,
    BLST_VISIBLE_ISO_200 = 0x02,
    BLST_VISIBLE_ISO_400 = 0x03,
    BLST_VISIBLE_ISO_800 = 0x04,
    BLST_VISIBLE_ISO_1600 = 0x05,
    BLST_VISIBLE_ISO_3200 = 0x06,
    BLST_VISIBLE_ISO_6400 = 0x07
} blst_visible_iso_t;

typedef enum {
    BLST_VISIBLE_PHOTO_RES_8000_6000 = 0x14,
    BLST_VISIBLE_PHOTO_RES_4000_3000 = 0x15
} blst_visible_photo_resolution_t;

typedef enum {
    BLST_VISIBLE_RECORD_RES_1080P = 0x08,
    BLST_VISIBLE_RECORD_RES_4K = 0x26,
    BLST_VISIBLE_RECORD_RES_4000_3000 = 0x36
} blst_visible_record_resolution_t;

typedef enum {
    BLST_VISIBLE_SHUTTER_AUTO = 0x00,
    BLST_VISIBLE_SHUTTER_1_4 = 0x01,
    BLST_VISIBLE_SHUTTER_1_8 = 0x02,
    BLST_VISIBLE_SHUTTER_1_15 = 0x03,
    BLST_VISIBLE_SHUTTER_1_30 = 0x04,
    BLST_VISIBLE_SHUTTER_1_60 = 0x05,
    BLST_VISIBLE_SHUTTER_1_125 = 0x06,
    BLST_VISIBLE_SHUTTER_1_250 = 0x07,
    BLST_VISIBLE_SHUTTER_1_500 = 0x08,
    BLST_VISIBLE_SHUTTER_1_1000 = 0x09,
    BLST_VISIBLE_SHUTTER_1_2000 = 0x0A,
    BLST_VISIBLE_SHUTTER_1_4000 = 0x0B,
    BLST_VISIBLE_SHUTTER_1_5000 = 0x0C,
    BLST_VISIBLE_SHUTTER_1_6000 = 0x0D,
    BLST_VISIBLE_SHUTTER_1_8000 = 0x0E
} blst_visible_shutter_t;

typedef enum {
    BLST_VISIBLE_WB_AUTO = 0x01,
    BLST_VISIBLE_WB_INCANDESCENT = 0x02,
    BLST_VISIBLE_WB_FLUORESCENT = 0x03,
    BLST_VISIBLE_WB_WARM_FLUORESCENT = 0x04,
    BLST_VISIBLE_WB_DAYLIGHT = 0x05,
    BLST_VISIBLE_WB_CLOUDY = 0x06,
    BLST_VISIBLE_WB_TWILIGHT = 0x07,
    BLST_VISIBLE_WB_SHADE = 0x08
} blst_visible_white_balance_t;

typedef enum {
    BLST_VISIBLE_ANTIFLICKER_OFF = 0x01,
    BLST_VISIBLE_ANTIFLICKER_50HZ = 0x02,
    BLST_VISIBLE_ANTIFLICKER_60HZ = 0x03,
    BLST_VISIBLE_ANTIFLICKER_AUTO = 0x04
} blst_visible_antiflicker_t;

typedef enum {
    BLST_VISIBLE_LEVEL_SWITCH_ON = 0x01,
    BLST_VISIBLE_LEVEL_SWITCH_OFF = 0x02
} blst_visible_level_switch_t;

typedef enum {
    BLST_VISIBLE_AE_LOCK_ON = 0x01,
    BLST_VISIBLE_AE_LOCK_OFF = 0x02
} blst_visible_ae_lock_t;

typedef enum {
    BLST_VISIBLE_FOCUS_DONE = 0x00,
    BLST_VISIBLE_FOCUSING = 0x01
} blst_visible_focus_status_t;

typedef enum {
    BLST_VISIBLE_METERING_CENTER_WEIGHTED = 0x01,
    BLST_VISIBLE_METERING_AREA = 0x02,
    BLST_VISIBLE_METERING_AVERAGE = 0x03
} blst_visible_metering_mode_t;

typedef enum {
    BLST_VISIBLE_NIGHT_MODE_ON = 0x01,
    BLST_VISIBLE_NIGHT_MODE_OFF = 0x03
} blst_visible_night_mode_t;

typedef enum {
    BLST_VISIBLE_NIGHT_FPS_30 = 0x01,
    BLST_VISIBLE_NIGHT_FPS_15 = 0x02,
    BLST_VISIBLE_NIGHT_FPS_7 = 0x03
} blst_visible_night_fps_t;

typedef enum {
    BLST_VISIBLE_SET_DONE = 0x00,
    BLST_VISIBLE_SETTING = 0x01,
    BLST_VISIBLE_SET_FAILED = 0x02
} blst_visible_set_status_t;

typedef enum {
    BLST_VISIBLE_FUSION_IR_OFF = 0x00,
    BLST_VISIBLE_FUSION_IR_ON = 0x01
} blst_visible_fusion_ir_switch_t;

typedef enum {
    BLST_VISIBLE_FUSION_IR_DEBUG_ADJUST = 0x01,
    BLST_VISIBLE_FUSION_IR_DEBUG_SAVE = 0x02
} blst_visible_fusion_ir_debug_action_t;

typedef enum {
    BLST_VISIBLE_FUSION_IR_GRAYSCALE_OFF = 0x00,
    BLST_VISIBLE_FUSION_IR_GRAYSCALE_ON = 0x01
} blst_visible_fusion_ir_grayscale_t;

typedef enum {
    BLST_CAMERA_CAPTURE_MODE_DEFAULT = 0x00,
    BLST_CAMERA_CAPTURE_MODE_IR = 0x01,
    BLST_CAMERA_CAPTURE_MODE_VISIBLE = 0x02,
    BLST_CAMERA_CAPTURE_MODE_IR_VISIBLE = 0x03
} blst_camera_capture_mode_t;

typedef enum {
    BLST_CAMERA_PHOTO_CMD_START = 0x00,
    BLST_CAMERA_PHOTO_CMD_STOP = 0x01
} blst_camera_photo_command_t;

typedef enum {
    BLST_CAMERA_RECORD_MODE_DEFAULT = 0x00,
    BLST_CAMERA_RECORD_MODE_IR = 0x01,
    BLST_CAMERA_RECORD_MODE_VISIBLE = 0x02,
    BLST_CAMERA_RECORD_MODE_IR_VISIBLE = 0x03,
    BLST_CAMERA_RECORD_MODE_STREAM_CAPTURE = 0x04
} blst_camera_record_mode_t;

typedef enum {
    BLST_CAMERA_RECORD_CMD_START = 0x01,
    BLST_CAMERA_RECORD_CMD_STOP = 0x02
} blst_camera_record_command_t;

typedef enum {
    BLST_CAMERA_ZOOM_MODE_NORMAL = 0x00,
    BLST_CAMERA_ZOOM_MODE_3M = 0x01,
    BLST_CAMERA_ZOOM_MODE_4M = 0x02,
    BLST_CAMERA_ZOOM_MODE_5M = 0x03
} blst_camera_zoom_mode_t;

typedef enum {
    BLST_CAMERA_ZOOM_SPEED_NORMAL = 0x00,
    BLST_CAMERA_ZOOM_SPEED_FAST = 0x01
} blst_camera_zoom_speed_t;

typedef enum {
    BLST_CAMERA_ZOOM_CONTROL_CONTINUOUS_IN = 0x00,
    BLST_CAMERA_ZOOM_CONTROL_CONTINUOUS_OUT = 0x01,
    BLST_CAMERA_ZOOM_CONTROL_STOP = 0x02,
    BLST_CAMERA_ZOOM_CONTROL_STEP_IN = 0x03,
    BLST_CAMERA_ZOOM_CONTROL_STEP_OUT = 0x04
} blst_camera_zoom_control_t;

typedef enum {
    BLST_CAMERA_VIDEO_OUTPUT_RES_1080P30 = 0x01,
    BLST_CAMERA_VIDEO_OUTPUT_RES_720P30 = 0x02
} blst_camera_video_output_resolution_t;

typedef enum {
    BLST_CAMERA_FORMAT_DONE = 0x00,
    BLST_CAMERA_FORMATTING = 0x01,
    BLST_CAMERA_FORMAT_FAILED = 0x02,
    BLST_CAMERA_FORMAT_SD_UNAVAILABLE = 0x03
} blst_camera_tf_format_status_code_t;


typedef enum {
    BLST_CAMERA_IP_STATIC = 0x00,
    BLST_CAMERA_IP_DYNAMIC = 0x01
} blst_camera_ip_type_t;

typedef enum {
    BLST_CAMERA_FOCUS_AUTO = 0x00,
    BLST_CAMERA_FOCUS_MANUAL_PLUS = 0x01,
    BLST_CAMERA_FOCUS_MANUAL_MINUS = 0x02,
    BLST_CAMERA_FOCUS_MANUAL_STOP = 0x03,
    BLST_CAMERA_FOCUS_AREA_AUTO = 0x04,
    BLST_CAMERA_FOCUS_ONE_SHOT = 0x05,
    BLST_CAMERA_FOCUS_ZOOM_AUTO_ON = 0x06,
    BLST_CAMERA_FOCUS_ZOOM_AUTO_OFF = 0x07,
    BLST_CAMERA_FOCUS_MANUAL_VALUE = 0x08
} blst_camera_focus_mode_t;

typedef enum {
    BLST_CAMERA_FOCUS_TYPE_NORMAL = 0x00,
    BLST_CAMERA_FOCUS_TYPE_NEAR_SUPPORT = 0x01,
    BLST_CAMERA_FOCUS_TYPE_3M = 0x02,
    BLST_CAMERA_FOCUS_TYPE_4M = 0x03,
    BLST_CAMERA_FOCUS_TYPE_5M = 0x04
} blst_camera_focus_type_t;

typedef enum {
    BLST_CAMERA_OSD_MASTER_OFF = 0x00,
    BLST_CAMERA_OSD_MASTER_ON = 0x01
} blst_camera_osd_master_switch_t;

typedef enum {
    BLST_CAMERA_POWER_SHUTDOWN = 0x01,
    BLST_CAMERA_POWER_SOFT_REBOOT = 0x02
} blst_camera_power_action_t;

typedef enum {
    BLST_CAMERA_IMAGE_MODE_IR = 0x00,
    BLST_CAMERA_IMAGE_MODE_VISIBLE_0X05 = 0x05,
    BLST_CAMERA_IMAGE_MODE_VISIBLE_0X06 = 0x06,
    BLST_CAMERA_IMAGE_MODE_SPLIT = 0x07
} blst_camera_image_mode_t;

typedef enum {
    BLST_CAMERA_AI_SWITCH_ON = 0x01,
    BLST_CAMERA_AI_SWITCH_OFF = 0x02
} blst_camera_ai_switch_t;

typedef enum {
    BLST_CAMERA_TRACK_TARGET_ON = 0x01,
    BLST_CAMERA_TRACK_TARGET_OFF = 0x02
} blst_camera_track_switch_t;

typedef enum {
    BLST_CAMERA_OSD_POSITION_LEFT_TOP = 0x01,
    BLST_CAMERA_OSD_POSITION_LEFT_BOTTOM = 0x02,
    BLST_CAMERA_OSD_POSITION_RIGHT_TOP = 0x03,
    BLST_CAMERA_OSD_POSITION_RIGHT_BOTTOM = 0x04
} blst_camera_osd_position_t;

typedef enum {
    BLST_CAMERA_OSD_COLOR_WHITE = 0x01,
    BLST_CAMERA_OSD_COLOR_RED = 0x02
} blst_camera_osd_color_t;

typedef enum {
    BLST_CAMERA_PLAYBACK_DOWNLOAD_MODE_ON = 0x01,
    BLST_CAMERA_PLAYBACK_DOWNLOAD_MODE_OFF = 0x02
} blst_camera_playback_download_mode_t;

typedef enum {
    BLST_CAMERA_MEDIA_TYPE_LIBRARY_LIST = 0x01,
    BLST_CAMERA_MEDIA_TYPE_PREVIEW = 0x02,
    BLST_CAMERA_MEDIA_TYPE_THUMBNAIL = 0x03,
    BLST_CAMERA_MEDIA_TYPE_ORIGINAL_IMAGE = 0x04,
    BLST_CAMERA_MEDIA_TYPE_ORIGINAL_VIDEO = 0x05,
    BLST_CAMERA_MEDIA_TYPE_STOP_DOWNLOAD = 0x07,
    BLST_CAMERA_MEDIA_TYPE_CUSTOM_FOLDER_LIST = 0x08
} blst_camera_media_download_type_t;

typedef enum {
    BLST_CAMERA_MEDIA_DOWNLOAD_IN_PROGRESS = 0x01,
    BLST_CAMERA_MEDIA_DOWNLOAD_DONE = 0x02
} blst_camera_media_download_status_code_t;

typedef enum {
    BLST_CAMERA_MEDIA_CLIENT_PLAYBACK = 0x00,
    BLST_CAMERA_MEDIA_CLIENT_DOWNLOAD = 0x01
} blst_camera_media_client_set_type_t;

typedef enum {
    BLST_CAMERA_MEDIA_CLIENT_GET_PLAYBACK = 0x01,
    BLST_CAMERA_MEDIA_CLIENT_GET_DOWNLOAD = 0x02
} blst_camera_media_client_get_type_t;

typedef enum {
    BLST_CAMERA_TARGET_GPS_REPORT_ON = 0x01,
    BLST_CAMERA_TARGET_GPS_REPORT_OFF = 0x02
} blst_camera_target_gps_report_switch_t;

typedef enum {
    BLST_CAMERA_SPLIT_LINK_ZOOM_OFF = 0x00,
    BLST_CAMERA_SPLIT_LINK_ZOOM_ON = 0x01
} blst_camera_split_link_zoom_switch_t;

typedef enum {
    BLST_CAMERA_IMX989_FOCUS_ONE_SHOT = 0x00,
    BLST_CAMERA_IMX989_FOCUS_MANUAL = 0x01
} blst_camera_imx989_focus_mode_t;

typedef enum {
    BLST_CAMERA_MEDIA_PLAYING = 0x02,
    BLST_CAMERA_MEDIA_PAUSED = 0x03,
    BLST_CAMERA_MEDIA_STOPPED = 0x07
} blst_camera_media_playback_status_code_t;

typedef struct {
    uint8_t photo_record_mode;
    uint8_t network_resolution;
    uint8_t video_codec;
    uint8_t stream_mode;
    uint8_t video_bitrate;
    uint8_t photo_mode;
    uint8_t delay_photo_time_s;
    uint8_t burst_count;
    uint8_t sd_status;
    uint16_t sd_total_capacity_mb10;
    uint16_t sd_remaining_capacity_mb10;
    uint16_t sd_used_capacity_mb10;
    uint8_t video_stream_status;
} blst_camera_system_status_t;

typedef struct {
    int16_t area_max_temp_decic;
    int16_t area_min_temp_decic;
    int16_t area_center_temp_decic;
    int16_t point_temp_decic;
    int16_t area_avg_temp_decic;
    uint16_t area_max_x;
    uint16_t area_max_y;
    uint16_t area_min_x;
    uint16_t area_min_y;
    uint16_t area_center_x;
    uint16_t area_center_y;
    uint16_t point_x;
    uint16_t point_y;
    uint8_t high_temp_warning;
    uint8_t low_temp_warning;
    uint8_t temp_diff_warning;
    uint8_t threshold_temp_warning;
    uint8_t ir_device_type;
} blst_ir_camera_status_t;


typedef struct {
    uint8_t status;
    uint8_t progress;
    uint8_t reserved;
} blst_camera_upgrade_status_t;

typedef struct {
    uint16_t response_code;
    uint8_t pseudo_color;
    uint8_t temp_zoom_config;
    uint8_t temperature_measure_enabled;
    uint8_t temperature_measure_type;
    uint8_t ir_core_type;
    uint8_t digital_zoom;
    uint8_t sharpness;
    uint8_t gain_mode;
    uint8_t brightness;
    uint8_t contrast;
    uint8_t denoise_config;
    uint8_t denoise_enabled;
    uint8_t denoise_level;
    uint8_t enhancement_config;
    uint8_t enhancement_enabled;
    uint8_t enhancement_level;
    uint8_t reserved[13];
    int16_t high_temp_warning_temp_decic;
    int16_t low_temp_warning_temp_decic;
    int16_t temp_diff_warning_decic;
    uint8_t threshold_temp_warning_enabled;
    int16_t temp_diff_base_decic;
    int16_t temp_float_decic;
    uint8_t fusion_enabled;
    uint8_t fusion_weight;
} blst_ir_camera_all_params_t;

typedef struct {
    uint8_t zoom_status;
    uint16_t focal_length_0_01mm;
    uint16_t hybrid_zoom_x10;
    uint8_t ev;
    uint16_t iso;
    uint16_t electronic_shutter;
    uint8_t ae_lock_status;
    uint8_t focus_status;
    uint16_t precise_retake_focal_length;
} blst_visible_camera_status_t;

typedef struct {
    uint16_t response_code;
    uint8_t photo_resolution;
    uint8_t record_resolution;
    uint8_t record_bitrate;
    uint8_t white_balance;
    uint8_t night_mode;
    uint8_t night_fps;
    uint8_t ev;
    uint8_t iso;
    uint8_t electronic_shutter;
    uint8_t zoom_fine_tune;
    uint8_t backlight_config;
    uint8_t backlight_enabled;
    uint8_t backlight_value;
    uint8_t highlight_config;
    uint8_t highlight_enabled;
    uint8_t highlight_value;
    uint8_t ae_lock;
    uint16_t osd_switch_mask;
    uint8_t antiflicker;
    uint8_t metering_mode;
    uint8_t reserved[2];
    uint8_t fusion_ir_enabled;
    uint8_t fusion_weight;
} blst_visible_camera_all_params_t;

typedef struct {
    uint8_t status;
    uint16_t reserved;
} blst_visible_night_mode_status_t;

typedef struct {
    uint8_t capture_mode;
    uint8_t feedback;
    uint16_t burst_count;
    uint8_t reserved[3];
} blst_camera_photo_status_t;

typedef struct {
    uint8_t record_mode;
    uint8_t feedback;
    uint16_t record_time_s;
    uint8_t reserved[3];
} blst_camera_record_status_t;

typedef struct {
    uint8_t zoom_status;
    uint8_t reserved;
} blst_camera_hybrid_zoom_status_t;

typedef struct {
    uint8_t status;
    uint16_t reserved;
} blst_camera_tf_format_status_t;


typedef struct {
    uint8_t bytes[4];
} blst_camera_ip_address_t;

typedef struct {
    uint16_t response_code;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
    int32_t longitude_deg1e7;
    int32_t latitude_deg1e7;
    int16_t relative_altitude_m10;
    int16_t altitude_m10;
    int16_t aircraft_yaw_deg100;
    int16_t aircraft_roll_deg100;
    int16_t aircraft_pitch_deg100;
    uint8_t reserved[6];
} blst_camera_gps_info_ack_t;

typedef struct {
    uint16_t response_code;
    blst_camera_ip_address_t ip;
    blst_camera_ip_address_t netmask;
    blst_camera_ip_address_t gateway;
} blst_camera_ip_info_ack_t;

typedef struct {
    uint16_t response_code;
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} blst_camera_version_ack_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint8_t confidence_percent;
    uint8_t category_id;
} blst_camera_ai_target_t;

typedef struct {
    uint8_t target_count;
    uint8_t detection_status;
    blst_camera_ai_target_t targets[BLST_CAMERA_AI_MAX_TARGETS];
} blst_camera_ai_detection_status_t;

typedef struct {
    uint8_t mount_request_mask;
    uint8_t gps_status;
    uint64_t utc_timestamp_ms;
    int32_t longitude_deg1e7;
    int32_t latitude_deg1e7;
    int32_t relative_altitude_mm;
    int32_t altitude_mm;
    int16_t aircraft_yaw_deg100;
    int16_t aircraft_roll_deg100;
    int16_t aircraft_pitch_deg100;
    uint16_t airspeed_ms100;
    uint16_t groundspeed_ms100;
    uint16_t heading_deg100;
    uint16_t throttle_percent100;
    int16_t climb_rate_ms100;
} blst_camera_target_gps_request_t;

typedef struct {
    uint16_t response_code;
    uint8_t mount_enable;
    uint64_t utc_timestamp_ms;
    int32_t longitude_deg1e7;
    int32_t latitude_deg1e7;
    int32_t relative_altitude_mm;
    int32_t altitude_mm;
} blst_camera_target_gps_info_t;

typedef struct {
    uint16_t response_code;
    int16_t temperature_decic;
} blst_camera_core_temperature_ack_t;

typedef struct {
    uint8_t download_status;
    uint8_t playback_status;
    uint32_t total_length_ms;
    uint32_t playback_position_s;
} blst_camera_media_playback_status_t;

typedef struct {
    uint8_t enabled;
    uint8_t position;
    uint8_t color;
    uint8_t raw_attr;
} blst_camera_osd_config_item_t;

typedef struct {
    blst_camera_osd_config_item_t items[BLST_CAMERA_OSD_CONFIG_COUNT];
    uint8_t master_switch;
} blst_camera_osd_config_ack_t;

typedef struct {
    uint8_t enabled;
    uint8_t category_mask[16];
} blst_camera_ai_category_ack_t;

typedef struct {
    char sn[BLST_CAMERA_CORE_SN_STRING_LEN + 1u];
    uint8_t raw[BLST_CAMERA_CORE_SN_STRING_LEN];
} blst_camera_core_sn_ack_t;

typedef struct {
    blst_camera_ip_address_t ip;
    uint16_t port;
    uint8_t type;
} blst_camera_media_client_info_ack_t;

int blst_camera_parse_system_status_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_system_status_t *out_status);
int blst_camera_parse_ir_status_payload(const uint8_t *payload, uint8_t payload_len, blst_ir_camera_status_t *out_status);
int blst_camera_parse_visible_status_payload(const uint8_t *payload, uint8_t payload_len, blst_visible_camera_status_t *out_status);
int blst_camera_parse_upgrade_status_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_upgrade_status_t *out_status);

int blst_camera_build_ir_get_all_params_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
/* Uses normal zoom speed in Byte2 for compatibility. */
int blst_camera_build_ir_digital_zoom_payload(uint8_t zoom, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_pseudo_color_payload(uint8_t pseudo_color, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_temp_measure_switch_payload(blst_ir_temp_measure_switch_t sw, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_sharpness_payload(uint8_t sharpness, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_brightness_payload(uint8_t brightness, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_contrast_payload(uint8_t contrast, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_denoise_payload(uint8_t enabled, uint8_t level, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_enhancement_payload(uint8_t level, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_point_temp_payload(uint16_t x, uint16_t y, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_area_temp_payload(uint16_t width, uint16_t height, uint16_t center_x, uint16_t center_y, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_gain_mode_payload(blst_ir_gain_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_temp_overlay_payload(blst_ir_temp_overlay_switch_t sw, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_digital_zoom_with_speed_payload(uint8_t zoom, blst_camera_zoom_speed_t speed, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_fusion_visible_payload(blst_ir_fusion_visible_switch_t sw, uint8_t visible_weight, uint8_t ir_weight, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_fusion_visible_debug_payload(blst_ir_fusion_visible_debug_action_t action, int16_t visible_offset_x, int16_t visible_offset_y, int16_t rotation_angle, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_fusion_visible_grayscale_payload(blst_ir_fusion_visible_grayscale_t grayscale, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_isotherm_payload(blst_ir_isotherm_switch_t sw, int16_t high_temp_decic, int16_t low_temp_decic, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ir_ai_super_resolution_payload(blst_ir_ai_super_resolution_switch_t sw, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_parse_ir_all_params_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_ir_camera_all_params_t *out_params);

int blst_camera_build_visible_get_all_params_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_record_resolution_payload(blst_visible_record_resolution_t resolution, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_photo_resolution_payload(blst_visible_photo_resolution_t resolution, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_iso_payload(blst_visible_iso_t iso, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_shutter_payload(blst_visible_shutter_t shutter, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_ev_payload(blst_visible_ev_t ev, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_white_balance_payload(blst_visible_white_balance_t white_balance, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_antiflicker_payload(blst_visible_antiflicker_t antiflicker, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_highlight_suppression_payload(blst_visible_level_switch_t sw, uint8_t value, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_backlight_comp_payload(blst_visible_level_switch_t sw, uint8_t value, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_ae_lock_payload(blst_visible_ae_lock_t ae_lock, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_metering_mode_payload(blst_visible_metering_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_night_mode_payload(blst_visible_night_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_night_fps_payload(blst_visible_night_fps_t fps, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_fusion_ir_payload(blst_visible_fusion_ir_switch_t sw, uint8_t ir_weight, uint8_t visible_weight, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_fusion_ir_debug_payload(blst_visible_fusion_ir_debug_action_t action, int16_t ir_offset_x, int16_t ir_offset_y, int16_t rotation_angle, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_visible_fusion_ir_grayscale_payload(blst_visible_fusion_ir_grayscale_t grayscale, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_parse_visible_all_params_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_visible_camera_all_params_t *out_params);
int blst_camera_parse_visible_night_mode_status_payload(const uint8_t *payload, uint8_t payload_len, blst_visible_night_mode_status_t *out_status);

int blst_camera_build_photo_record_mode_payload(blst_camera_photo_record_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_photo_params_payload(blst_camera_photo_mode_t mode, uint8_t delay_seconds, uint8_t burst_count, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_photo_capture_payload(blst_camera_capture_mode_t mode, blst_camera_photo_command_t command, const char *folder_name, const char *photo_name, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_record_payload(blst_camera_record_mode_t mode, blst_camera_record_command_t command, const char *folder_name, const char *video_name, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
/* Protocol Byte1 is zoom speed in newer public protocol; the legacy mode parameter is interpreted as speed. */
int blst_camera_build_hybrid_zoom_payload(blst_camera_zoom_mode_t mode, uint16_t zoom_x10, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_hybrid_zoom_with_speed_payload(blst_camera_zoom_speed_t speed, uint16_t zoom_x10, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
/* The mode parameter is ignored by newer public protocol; Byte2 is reserved and set to 0. */
int blst_camera_build_continuous_zoom_payload(blst_camera_zoom_control_t control, blst_camera_zoom_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_continuous_zoom_simple_payload(blst_camera_zoom_control_t control, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_precise_retake_payload(blst_camera_capture_mode_t mode, blst_visible_photo_resolution_t photo_resolution, uint16_t visible_zoom_x10, uint16_t precise_focal_length, const char *folder_name, const char *photo_name, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_video_bitrate_payload(blst_camera_video_bitrate_t bitrate, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_video_output_resolution_payload(blst_camera_video_output_resolution_t resolution, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_video_codec_payload(blst_camera_codec_t codec, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_tf_card_format_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_time_sync_payload(uint64_t timestamp_ms, int32_t timezone_minutes, int32_t daylight_saving, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_factory_reset_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);

int blst_camera_parse_photo_status_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_photo_status_t *out_status);
int blst_camera_parse_record_status_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_record_status_t *out_status);
int blst_camera_parse_hybrid_zoom_status_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_hybrid_zoom_status_t *out_status);
int blst_camera_parse_tf_card_format_status_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_tf_format_status_t *out_status);

int blst_camera_build_gps_info_request_payload(uint8_t lens_enabled, uint8_t hour, uint8_t minute, uint8_t second, uint16_t millisecond, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ip_set_payload(blst_camera_ip_type_t ip_type, const blst_camera_ip_address_t *ip, const blst_camera_ip_address_t *netmask, const blst_camera_ip_address_t *gateway, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ip_get_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_focus_payload(blst_camera_focus_mode_t mode, uint16_t left_top_x, uint16_t left_top_y, uint16_t right_bottom_x, uint16_t right_bottom_y, blst_camera_focus_type_t focus_type, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_focus_manual_value_payload(uint16_t focus_value, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_osd_master_switch_payload(blst_camera_osd_master_switch_t sw, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_power_control_payload(blst_camera_power_action_t action, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_version_get_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_image_mode_payload(blst_camera_image_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ai_recognition_payload(blst_camera_ai_switch_t sw, uint8_t model_type, const uint8_t category_mask[16], uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_target_gps_request_payload(const blst_camera_target_gps_request_t *request, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_track_target_payload(blst_camera_track_switch_t sw, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_core_temperature_get_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_osd_config_payload(uint8_t item_id, uint8_t enabled, blst_camera_osd_position_t position, blst_camera_osd_color_t color, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_osd_custom_text_payload(uint8_t text_index, const char *text, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_playback_download_mode_payload(blst_camera_playback_download_mode_t mode, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_media_library_refresh_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_media_delete_payload(const char *absolute_path, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_media_download_payload(blst_camera_media_download_type_t media_type, uint16_t packet_size, uint32_t start_packet_index, uint8_t packet_count, const char *absolute_path, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_playback_path_payload(const char *absolute_path, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_playback_seek_payload(uint32_t seek_seconds, const char *absolute_path, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_media_client_set_payload(blst_camera_media_client_set_type_t type, const blst_camera_ip_address_t *ip, uint16_t port, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_osd_config_get_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_ai_category_get_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_core_sn_get_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_media_client_get_payload(blst_camera_media_client_get_type_t type, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_kernel_version_get_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_target_gps_report_switch_payload(blst_camera_target_gps_report_switch_t sw, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_idr_refresh_payload(uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_dynamic_bitrate_payload(uint16_t bitrate_kbps, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_split_link_zoom_payload(blst_camera_split_link_zoom_switch_t sw, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);
int blst_camera_build_imx989_focus_payload(blst_camera_imx989_focus_mode_t mode, uint16_t focus_value, uint8_t *out_payload, size_t out_capacity, uint8_t *out_payload_len);

int blst_camera_parse_gps_info_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_gps_info_ack_t *out_info);
int blst_camera_parse_ip_get_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_ip_info_ack_t *out_info);
int blst_camera_parse_version_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_version_ack_t *out_version);
int blst_camera_parse_ai_detection_status_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_ai_detection_status_t *out_status);
int blst_camera_parse_track_target_status_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_ai_detection_status_t *out_status);
int blst_camera_parse_target_gps_info_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_target_gps_info_t *out_info);
int blst_camera_parse_core_temperature_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_core_temperature_ack_t *out_temperature);
int blst_camera_parse_media_playback_status_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_media_playback_status_t *out_status);
int blst_camera_parse_osd_config_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_osd_config_ack_t *out_config);
int blst_camera_parse_ai_category_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_ai_category_ack_t *out_category);
int blst_camera_parse_core_sn_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_core_sn_ack_t *out_sn);
int blst_camera_parse_media_client_get_ack_payload(const uint8_t *payload, uint8_t payload_len, blst_camera_media_client_info_ack_t *out_info);

#ifdef __cplusplus
}
#endif

#endif
