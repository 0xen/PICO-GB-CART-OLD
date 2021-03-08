


cmake_minimum_required(VERSION 3.12)

set(PICO_EXTRAS_PATH F:/Downloads/pico-extras-master/pico-extras-master)
# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)
 
project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

include(example_auto_set_url.cmake)



set(project_name PICO_GB_Bootloader)
project(${project_name})


set(src
boot.c
GBCode/Bootloader/GB/bootloadercode.c

#ROMS
Roms/DRMARIO.c
Roms/TETRIS.c
Roms/ZELDA.c
Roms/ALLEYWAY.c
Roms/SUPERMARIOLAND.c
Roms/MARIOANDYOSHI.c
Roms/ALSTARCHALLENGE2.c
Roms/pocket.c

)
 
set(headers
)

add_executable(${project_name} ${src} ${headers})



# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(${project_name} pico_stdlib)
#target_link_libraries(${project_name} pico_multicore)
target_link_libraries(${project_name} hardware_pio)
#target_link_libraries(${project_name} hardware_spi)
#target_link_libraries(${project_name} pico_sd_card)

# enable usb output, disable uart output
pico_enable_stdio_usb(${project_name} 0)
pico_enable_stdio_uart(${project_name} 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${project_name})

# add url via pico_set_program_url
example_auto_set_url(${project_name})


pico_set_binary_type(${project_name} copy_to_ram)
