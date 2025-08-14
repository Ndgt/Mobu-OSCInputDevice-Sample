cmake_minimum_required(VERSION 3.17)

project(OSCInputDeviceSample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === Plugin Configuration ===
set(PLUGIN_VERSION 1.0)

# === Environment-specific user configuration ===
set(PRODUCT_VERSION 2025)
set(OSC_PACK_ROOT "C:/oscpack_1_1_0/install")
set(MOBU_ROOT "C:/Program Files/Autodesk/MotionBuilder ${PRODUCT_VERSION}")

# === Environment Setup ===
if(NOT WIN32)
    message(FATAL_ERROR "This plugin now only supports Windows.")
endif()

if(PRODUCT_VERSION LESS 2020)
    message(FATAL_ERROR "Only MotionBuilder 2020 or above is supported.")
endif()

# === Sources ===
add_library(${PROJECT_NAME} SHARED
    OSCInputDeviceSample_register.cpp
    OSCInputDeviceSample_main.h
    OSCInputDeviceSample_main.cpp
    OSCInputDeviceSample_UI.h
    OSCInputDeviceSample_UI.cpp
    OSCInputDeviceSample_socket.h
    OSCInputDeviceSample_socket.cpp
)

# Set the plugin file name
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}-${PLUGIN_VERSION}-MB${PRODUCT_VERSION}"
)

# === Compile Options & Definitions ===
target_compile_options(${PROJECT_NAME} PRIVATE /source-charset:utf-8)
target_compile_definitions(${PROJECT_NAME} PRIVATE PRODUCT_VERSION=${PRODUCT_VERSION}) # for Mobu SDK

# === Include Paths ===
target_include_directories(${PROJECT_NAME} PRIVATE
    "${MOBU_ROOT}/OpenRealitySDK/include"
    "${OSC_PACK_ROOT}/include"
)

# === Link Libraries ===
target_link_libraries(${PROJECT_NAME} PRIVATE
    "${MOBU_ROOT}/OpenRealitySDK/lib/x64/fbsdk.lib"
    Ws2_32
    winmm
    "$<$<CONFIG:Debug>:${OSC_PACK_ROOT}/lib/Debug/oscpackd.lib>"
    "$<$<CONFIG:Release>:${OSC_PACK_ROOT}/lib/Release/oscpack.lib>"
)

# === Post-build: Copy to Plugin Folder ===
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:${PROJECT_NAME}>"
        "${MOBU_ROOT}/bin/x64/plugins/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
)