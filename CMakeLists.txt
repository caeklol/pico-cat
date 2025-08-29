cmake_minimum_required(VERSION 3.13)

include(pico-sdk/pico_sdk_init.cmake)
include(pico-extras/external/pico_extras_import.cmake)

project(placeholder-fc C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

pico_sdk_init()

file(GLOB_RECURSE code_SRC
     "src/*.h"
     "src/*.c"
     "src/*.cpp"
     "src/*.hpp"
)

add_executable(code ${code_SRC})

target_link_libraries(code pico_stdlib hardware_pwm hardware_sleep)

# Enable USB output, disable UART output
pico_enable_stdio_usb(code 1)
pico_enable_stdio_uart(code 0)

# Create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(code)

