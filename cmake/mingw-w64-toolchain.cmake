set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

set(CMAKE_C_COMPILER   "/tmp/opencode/mingw-local/usr/bin/x86_64-w64-mingw32-gcc")
set(CMAKE_CXX_COMPILER "/tmp/opencode/mingw-local/usr/bin/x86_64-w64-mingw32-g++")

set(CMAKE_SYSROOT "/tmp/opencode/mingw-local/usr/x86_64-w64-mingw32")

set(CMAKE_FIND_ROOT_PATH
    "${CMAKE_SYSROOT}"
    "/home/shubham/Desktop/BLST_SDK_release_v1.1_linux_x86_64/thermal_analyzer/windows-qt/6.8.3/mingw_64"
)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_PREFIX_PATH "/home/shubham/Desktop/BLST_SDK_release_v1.1_linux_x86_64/thermal_analyzer/windows-qt/6.8.3/mingw_64")
