### This project is under development and will see changes

# PICO GB
This project was developed to allow the usage of a Raspberry Pi Pico within a gameboy cartridge. For now, the project has a basic menu UI that allows you to boot into various roms baked into the Pico ELF code.

# How it works
The gameboy has 16 address pins and 8 data pins that need to be mapped to the Pico's IO pins. As well as this, we need to know if we are reading and writing data between the Pico and GB. The GameBoy provides a read and write pin, but using boolean logic, we can get away with using just WR as if its not set we must be reading data.
GameBoy | Pico
--- | ---
Address Pins 0-15 | 0-15
Data Pins 0-6 | 16-22
Data Pin 7 | 26
WR Pin | 27

# How the repo works
## Make Gameboy Menu
Within the GBCode folder, there are several folders that contain C projects that when compiled, produce GB code. The menu GB code is defined under "Bootloader" and pulls in the other GB code snippets. The complation uses the gbdk thats located under the "third_party" folder.

## Making the Pico code
To get started with making the Pico code, you should follow the official getting started guide on how to use VSCode with the pico-sdk and GCC for Arm-non-eabi. After you have VSCode configured, to build the project, run "MakeSLN.bat" located in the root directory and then you can navigate to the build folder for the SLN.

## Packing GB games into C
To make a C copy of a game that can be linked to the Pico menu, simply add a .gb file into Roms/ and run the "GB2C.bat". After this you can add the GameBoy games to the CMake list and add them in the Pico code.

# Updates
Any updates for this project can be found over on my [Reddit][https://www.reddit.com/user/LyneByLyne] 

# Resources
First a big thanks to HDR who has been working with me on making custom PCB's for this project. 
[HDRs Github](https://github.com/HDR/Game-Boy-Pico-Cart)

Also thanks to Gekkio for debug breakout cart, [GB-BRK-CART](https://github.com/Gekkio/gb-hardware)

# Upcoming features
* Support for multibank games
* Support for cart ram
* RTC
* Passing game names dynamicly over to menu
* External memory support (e.g. SD Card)


![Raspberry Pi Pico running custom menu](https://raw.githubusercontent.com/0xen/PICO-GB/main/etc/running.jpg)
