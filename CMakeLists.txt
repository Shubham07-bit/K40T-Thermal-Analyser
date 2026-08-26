cmake_minimum_required(VERSION 3.16)
project(thermal_analyzer VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Optional: point to your Qt 6.8.3 installation if not in PATH
if(EXISTS "/home/shubham/Qt6.8.3-install/lib/cmake/Qt6")
    list(APPEND CMAKE_PREFIX_PATH "/home/shubham/Qt6.8.3-install/lib/cmake/Qt6")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# BLST SDK bundled with the project.
set(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

# Linux SDK
set(SDK_INCLUDE_LINUX "${SDK_ROOT}/include")
set(SDK_LIB_LINUX "${SDK_ROOT}/lib/linux-x86_64/libblstsdk.a")

# Windows SDK. MinGW can only link MinGW-compatible static/import libraries
# (.a / .dll.a). MSVC .lib files are not compatible with MinGW.
set(SDK_INCLUDE_WIN "${SDK_ROOT}/include")
set(SDK_LIB_WIN_A "${SDK_ROOT}/lib/windows-x86_64/libblstsdk64.a")
set(SDK_LIB_WIN_DLLA "${SDK_ROOT}/lib/windows-x86_64/libblstsdk64.dll.a")
set(SDK_LIB_WIN_MSVC "${SDK_ROOT}/lib/windows-x86_64/blstsdk64.lib")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND EXISTS "${SDK_LIB_WIN_A}")
    set(SDK_INCLUDE "${SDK_INCLUDE_WIN}")
    set(SDK_LIB "${SDK_LIB_WIN_A}")
    set(WITH_BLST_SDK ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND EXISTS "${SDK_LIB_WIN_DLLA}")
    set(SDK_INCLUDE "${SDK_INCLUDE_WIN}")
    set(SDK_LIB "${SDK_LIB_WIN_DLLA}")
    set(WITH_BLST_SDK ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND EXISTS "${SDK_LIB_WIN_MSVC}" AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(SDK_INCLUDE "${SDK_INCLUDE_WIN}")
    set(SDK_LIB "${SDK_LIB_WIN_MSVC}")
    set(WITH_BLST_SDK ON)
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows" AND EXISTS "${SDK_LIB_LINUX}")
    set(SDK_INCLUDE "${SDK_INCLUDE_LINUX}")
    set(SDK_LIB "${SDK_LIB_LINUX}")
    set(WITH_BLST_SDK ON)
else()
    set(WITH_BLST_SDK OFF)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND EXISTS "${SDK_LIB_WIN_MSVC}" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(WARNING "Found lib/windows-x86_64/blstsdk64.lib, but it is an MSVC static library and cannot be linked by MinGW. "
                    "K40T IR JPEG support will be disabled on Windows for the MinGW build. "
                    "Provide lib/windows-x86_64/libblstsdk64.a (static) or "
                    "lib/windows-x86_64/libblstsdk64.dll.a + blstsdk64.dll (MinGW import) to enable it. "
                    "For MSVC builds the .lib can be used directly.")
endif()

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.ui
    src/ImageView.cpp
    src/ThermalDataModel.cpp
    src/ColorMap.cpp
    src/PngTiffLoader.cpp
    src/DjiRjpgLoader.cpp
    src/BatchProcessor.cpp
    src/OverlayExporter.cpp
    src/MatrixDialog.cpp
)

set(HEADERS
    src/MainWindow.h
    src/ImageView.h
    src/ThermalDataModel.h
    src/ColorMap.h
    src/ThermalImageLoader.h
    src/PngTiffLoader.h
    src/BlstIrLoader.h
    src/BatchProcessor.h
    src/OverlayExporter.h
    src/MatrixDialog.h
    src/ThermalBox.h
)

if(WITH_BLST_SDK)
    list(APPEND SOURCES src/BlstIrLoader.cpp)
    list(APPEND HEADERS src/BlstIrLoader.h)
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if(WITH_BLST_SDK)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WITH_BLST_SDK=1)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDK_INCLUDE})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDK_LIB})
endif()

if(WIN32 AND WITH_BLST_SDK)
    # The BLST SDK networking/serial modules need Winsock and setup APIs.
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 winmm setupapi)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# libm is part of the MSVC C runtime, so only link it on Unix.
if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE FALSE
    MACOSX_BUNDLE FALSE
)
