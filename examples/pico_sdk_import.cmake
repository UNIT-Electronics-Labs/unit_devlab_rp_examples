# Importador compartido del Pico SDK para todos los ejemplos.
# picodev proporciona PICO_SDK_PATH mediante CMake; una compilación manual
# también puede proporcionarlo como variable de entorno.
if(NOT PICO_SDK_PATH AND DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}")
endif()

if(NOT PICO_SDK_PATH)
    message(FATAL_ERROR
        "PICO_SDK_PATH no está definido. Usa 'picodev build' o configura "
        "la variable de entorno PICO_SDK_PATH."
    )
endif()

include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")
