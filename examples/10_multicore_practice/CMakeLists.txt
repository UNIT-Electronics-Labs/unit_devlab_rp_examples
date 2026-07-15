cmake_minimum_required(VERSION 3.20)

include("${CMAKE_CURRENT_LIST_DIR}/../pico_sdk_import.cmake")

# ─────────────────────────────────────────────
# CONFIGURACIÓN DEL PROYECTO
# Generado automáticamente por picodev
# ─────────────────────────────────────────────
set(PROJECT_NAME    "10_multicore_practice")
set(PROJECT_SOURCES "main.c" "ssd1306.c")
set(PICO_BOARD      "pico")

set(PROJECT_LIBS
    pico_stdlib
    pico_multicore
    hardware_adc
    hardware_i2c
)
# ─────────────────────────────────────────────
# NO MODIFICAR DE AQUÍ EN ADELANTE
# ─────────────────────────────────────────────

project(${PROJECT_NAME} C CXX ASM)

pico_sdk_init()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})


pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)