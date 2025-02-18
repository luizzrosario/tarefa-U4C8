cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(tarefa-U4C8 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adiciona o executável principal
add_executable(tarefa-U4C8 
    tarefa-U4C8.c          # Arquivo principal fora das pastas
    src/ssd1306.c 
)

pico_set_program_name(tarefa-U4C8 "tarefa-U4C8")
pico_set_program_version(tarefa-U4C8 "0.1")

# Generate PIO header
pico_generate_pio_header(tarefa-U4C8 ${CMAKE_CURRENT_LIST_DIR}/utils/ws2812.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(tarefa-U4C8 1)
pico_enable_stdio_usb(tarefa-U4C8 1)

# Adiciona as bibliotecas padrão do Pico
target_link_libraries(tarefa-U4C8
        pico_stdlib
        hardware_spi
        hardware_i2c
        hardware_pio
        hardware_pwm
        hardware_adc
)

# Inclui diretórios necessários
target_include_directories(tarefa-U4C8 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include  # Inclui a pasta include
        ${CMAKE_CURRENT_SOURCE_DIR}/src      # Inclui a pasta src (opcional)
)

pico_add_extra_outputs(tarefa-U4C8)  