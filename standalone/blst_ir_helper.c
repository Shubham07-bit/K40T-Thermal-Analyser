/*
 * blst_ir_helper.c
 *
 * Tiny C helper that exposes the BLST SDK IR-file functions to Python via a
 * shared library.  The static BLST SDK archive bundled in this repository is
 * linked into this helper.
 *
 * Linux build (run from this directory):
 *     gcc -std=c99 -O2 -shared -fPIC \
 *         -I../include \
 *         blst_ir_helper.c \
 *         ../lib/linux-x86_64/libblstsdk.a \
 *         -o libblst_ir_helper.so -lm
 *
 * Windows MSVC build (from a Visual Studio Developer Command Prompt):
 *     cl /O2 /W3 /D_USRDLL /D_WINDLL /I..\include \
 *        blst_ir_helper.c \
 *        ..\lib\windows-x86_64\blstsdk64.lib \
 *        ws2_32.lib winmm.lib setupapi.lib \
 *        /link /DLL /OUT:libblst_ir_helper.dll
 *
 * The resulting libblst_ir_helper.so / libblst_ir_helper.dll must sit next to
 * thermal_stats_blst.py, or be on the system library search path.
 */

#include <blst_ir_file.h>
#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

EXPORT int blst_ir_helper_get_info(const char *path,
                                   uint16_t *width,
                                   uint16_t *height,
                                   size_t *count)
{
    return blst_ir_file_get_info(path, width, height, count);
}

EXPORT int blst_ir_helper_parse_matrix(const char *path,
                                       float *out,
                                       size_t count)
{
    return blst_ir_file_parse_temperature_matrix(path, out, count);
}
