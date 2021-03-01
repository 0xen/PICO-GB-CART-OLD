### This project is under development and will see changes

# PICO GB
This project was developed to allow the usage of a Raspberry PI PICO within a gameboy cartridge. For now, the project has a basic menu UI that allows you to boot into various roms baked into the PICO Elf code.

# How it works
The gameboy has 16 address pins and 8 data pins that need to be mapped to the picos IO pins. As well as this, we need to know if we are reading and wrighting data between the PICO and GB. The gameboy provides a Read and Wright pin, but using boolean logic, we can get away with using just WR as if its not set we must be reading data.
Gameboy | PICO
--- | ---
Address Pins 0-15 | 0-15
Data Pins 0-6 | 16-22
Data Pin 7 | 26
WR Pin | 27

# How the repo works
## Make Gameboy Menu
Within the GBCode folder, there are several folders that contain C projects that when compiled, produce GB code. The menu gb code is defined under "Bootloader" and pulls in the other GB code snippets. The complation uses the gbdk thats located under the "third_party" folder.

## Making the PICO Code
Getting started with making the PICO code, you should follow the offitial getting started guide on how to use VSCode with the pico-sdk and GCC for Arm-non-eabi. After you have VSCode configured, to build the project, run "MakeSLN.bat" located in the root directory and then you can navigate to the build folder for the SLN.

## Packing GB games into C
To make a C copy of a game that can be linked to the PICO menu, simply add a .gb file into Roms and run the "GB2C.bat". After this you can add the gameboy games to the CMake list and add them in the pico code.

# Updates
Any updates for this project can be found over on my (Reddit)[https://www.reddit.com/user/LyneByLyne] 

# Resources
First a big thanks to HDR who has been working with me on making custom PCB's for this project. 
[HDRs Github](https://github.com/HDR/Game-Boy-Pico-Cart)

Also thanks to Gekkio for debug breakout cart, [GB-BRK-CART](https://github.com/Gekkio/gb-hardware)

# Upcoming features
* Support for multibank games
* Support for cart ram
* RTC
* Passing game names dynamicly over to menu
* External memory support (EG, SD Card)


![Raspberry PI Pico running custom menu](https://raw.githubusercontent.com/0xen/PICO-GB/main/etc/running.jpg)
