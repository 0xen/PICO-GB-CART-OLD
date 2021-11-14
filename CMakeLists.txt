cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 11)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

set(PICO_PLATFORM rp2040)
set(PICO_BOARD pico)

set(PROJECT gb-cart)

project(${PROJECT} C CXX)

# Initialize the SDK
pico_sdk_init()


#add_compile_options(-Wall
#        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
#        -Wno-unused-function # we have some for the docs that aren't called
#        -Wno-maybe-uninitialized
#        )

include(example_auto_set_url.cmake)

##########################################################################
############################### Bootloader ###############################
##########################################################################
set(BOOTLOADER bootloader)

add_executable(${BOOTLOADER}
        ${CMAKE_CURRENT_SOURCE_DIR}/bootloader.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hw_config.c
)



target_link_libraries(${BOOTLOADER} PRIVATE
        hardware_structs
        hardware_sync
        hardware_flash
        hardware_watchdog
        hardware_resets
        hardware_xosc
        hardware_clocks
        hardware_pll
        hardware_dma
        pico_platform
        pico_standard_link
        FatFs_SPI
        pico_stdlib
        )


# enable usb output, disable uart output
#pico_enable_stdio_usb(${BOOTLOADER} 1)
#pico_enable_stdio_uart(${BOOTLOADER} 0)


# Add SPI SD card support
add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

#pico_set_binary_type(${BOOTLOADER} copy_to_ram)

pico_add_uf2_output(${BOOTLOADER})
pico_set_program_name(${BOOTLOADER} ${BOOTLOADER})

set(BOOTLOADER_UF2 ${CMAKE_CURRENT_BINARY_DIR}/${BOOTLOADER}.uf2)

###########################################################################
################################# FIRMWARE ################################
###########################################################################

set(FIRMWARE firmware)

add_executable(${FIRMWARE}
firmware.c
${CMAKE_CURRENT_SOURCE_DIR}/hw_config.c
        )
target_compile_options(${FIRMWARE} PRIVATE -Os)
target_link_libraries(${FIRMWARE}
pico_stdlib
hardware_watchdog
hardware_flash
FatFs_SPI
)

# enable usb output, disable uart output
pico_enable_stdio_usb(${FIRMWARE} 0)
pico_enable_stdio_uart(${FIRMWARE} 0)

# create map/bin/hex file etc.
pico_add_uf2_output(${FIRMWARE})
pico_add_hex_output(${FIRMWARE})

# Add SPI SD card support
#add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

# Use a separate linker script for the application to make sure it is built
# to run at the right location (after the flashloader).
# Tell the linker to use pages no bigger than 4k or the UF2 generation from
# the ELF image has issues
# (see https://github.com/raspberrypi/pico-sdk/issues/573)


target_link_options(${FIRMWARE} PRIVATE -Wl,-z,max-page-size=4096)
pico_set_linker_script(${FIRMWARE} ${CMAKE_CURRENT_SOURCE_DIR}/memmap_default.ld)

#pico_set_binary_type(${FIRMWARE} copy_to_ram)

set(FIRMWARE_UF2 ${CMAKE_CURRENT_BINARY_DIR}/${FIRMWARE}.uf2)



###########################################################################
############################ STANDALONE FIRMWARE ##########################
###########################################################################

set(STANDALONE_FIRMWARE standalone_firmware)

add_executable(${STANDALONE_FIRMWARE}
firmware.c
${CMAKE_CURRENT_SOURCE_DIR}/hw_config.c
        )
target_compile_options(${STANDALONE_FIRMWARE} PRIVATE -Os)
target_link_libraries(${STANDALONE_FIRMWARE}
pico_stdlib
hardware_watchdog
hardware_flash
FatFs_SPI
)

# enable usb output, disable uart output
pico_enable_stdio_usb(${STANDALONE_FIRMWARE} 0)
pico_enable_stdio_uart(${STANDALONE_FIRMWARE} 0)

# create map/bin/hex file etc.
pico_add_uf2_output(${STANDALONE_FIRMWARE})
pico_add_hex_output(${STANDALONE_FIRMWARE})

# Add SPI SD card support
#add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

pico_set_binary_type(${STANDALONE_FIRMWARE} copy_to_ram)


##########################################################################
############################### APPLICATION ##############################
##########################################################################

# combine the bootloader and cart exe

set(COMPLETE_UF2 ${CMAKE_CURRENT_BINARY_DIR}/gb-cart.uf2)

find_package (Python3 REQUIRED COMPONENTS Interpreter)
add_custom_command(OUTPUT ${COMPLETE_UF2} DEPENDS ${BOOTLOADER} ${FIRMWARE}
        COMMENT "Building full UF2 image"
        COMMAND ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/uf2tool.py
                -o ${COMPLETE_UF2} ${BOOTLOADER_UF2} ${FIRMWARE_UF2}
        )

add_custom_target(${PROJECT} ALL DEPENDS ${COMPLETE_UF2})

install(FILES ${COMPLETE_UF2} DESTINATION ${CMAKE_INSTALL_PREFIX} )