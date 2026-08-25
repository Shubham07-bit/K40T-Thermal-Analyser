#ifndef BLST_UTILS_H
#define BLST_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void blst_put_u16_le(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
}

static inline void blst_put_i16_le(uint8_t *p, int16_t v)
{
    blst_put_u16_le(p, (uint16_t)v);
}

static inline void blst_put_u32_le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
    p[2] = (uint8_t)((v >> 16) & 0xFFu);
    p[3] = (uint8_t)((v >> 24) & 0xFFu);
}

static inline void blst_put_i32_le(uint8_t *p, int32_t v)
{
    blst_put_u32_le(p, (uint32_t)v);
}

static inline void blst_put_u64_le(uint8_t *p, uint64_t v)
{
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
    p[2] = (uint8_t)((v >> 16) & 0xFFu);
    p[3] = (uint8_t)((v >> 24) & 0xFFu);
    p[4] = (uint8_t)((v >> 32) & 0xFFu);
    p[5] = (uint8_t)((v >> 40) & 0xFFu);
    p[6] = (uint8_t)((v >> 48) & 0xFFu);
    p[7] = (uint8_t)((v >> 56) & 0xFFu);
}

static inline int blst_copy_fixed_ascii(uint8_t *dst, size_t dst_len, const char *src)
{
    size_t src_len;

    if (dst == NULL || dst_len == 0u) {
        return 0;
    }

    memset(dst, 0, dst_len);
    if (src == NULL) {
        return 0;
    }

    src_len = strlen(src);
    if (src_len > dst_len) {
        src_len = dst_len;
    }

    memcpy(dst, src, src_len);
    return (int)src_len;
}

static inline int blst_copy_var_ascii(uint8_t *dst, size_t dst_capacity, const char *src, uint8_t *out_len)
{
    size_t src_len;

    if (dst == NULL || src == NULL || out_len == NULL) {
        return -1;
    }

    src_len = strlen(src);
    if (src_len > 255u || src_len > dst_capacity) {
        return -1;
    }

    memcpy(dst, src, src_len);
    *out_len = (uint8_t)src_len;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif
