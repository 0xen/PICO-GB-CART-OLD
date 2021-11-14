/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
//#include <tusb.h>
#include "hardware/watchdog.h"
#include "hardware/structs/watchdog.h"

#include "sd_card.h"
#include "ff.h"

uint32_t address_pins = 0b1111111111111111; // Address pins
uint32_t data_pins = 0b110001111101 << 16; // Data pins
          
void ErrorFlash(int error)
{
    while(true)
    {
        for(int i = 0 ; i < error; i++)
        {
            gpio_put(25, 1);
            sleep_ms(200);
            gpio_put(25, 0);
            sleep_ms(200);
        }
        sleep_ms(1500);
    }
}

enum vreg_voltage {
    VREG_VOLTAGE_0_85 = 0b0110,    ///< 0.85v
    VREG_VOLTAGE_0_90 = 0b0111,    ///< 0.90v
    VREG_VOLTAGE_0_95 = 0b1000,    ///< 0.95v
    VREG_VOLTAGE_1_00 = 0b1001,    ///< 1.00v
    VREG_VOLTAGE_1_05 = 0b1010,    ///< 1.05v
    VREG_VOLTAGE_1_10 = 0b1011,    ///< 1.10v
    VREG_VOLTAGE_1_15 = 0b1100,    ///< 1.15v
    VREG_VOLTAGE_1_20 = 0b1101,    ///< 1.20v
    VREG_VOLTAGE_1_25 = 0b1110,    ///< 1.25v
    VREG_VOLTAGE_1_30 = 0b1111,    ///< 1.30v

    VREG_VOLTAGE_MIN = VREG_VOLTAGE_0_85,      ///< Always the minimum possible voltage
    VREG_VOLTAGE_DEFAULT = VREG_VOLTAGE_1_10,  ///< Default voltage on power up.
    VREG_VOLTAGE_MAX = VREG_VOLTAGE_1_30,      ///< Always the maximum possible voltage
};

uint32_t readAddress()
{
    return gpio_get_all() & address_pins;
}

void InitSD()
{
    // init sd card
    if(!sd_init_driver())
    {
        printf("Failed to start SD Card Driver\n");
        return 0;
    }
}

void OpenGame(const char* filename, uint8_t* data)
{
    
    InitSD();



    FRESULT fr;
    FATFS fs;
    int ret;

    fr = f_mount(&fs,"0:",1);
    if(fr != FR_OK)
    {
        printf("Failed to Mount\n");
        ErrorFlash(1);
        return;
    }

    
    FIL fil;
    fr = f_open(&fil, filename, FA_READ);
    if(fr != FR_OK)
    {
        printf("Failed to open\n");
        ErrorFlash(2);
        return;
    }


    int fileSize = f_size(&fil);


    UINT br = 0;

    fr = f_read(&fil, data, 0x8000, br);
    if (FR_OK != fr)
    {
        printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
        ErrorFlash(3);
        //return;
    }

    
    //for (size_t i = 0; i < 0x8000; ++i)
    //{
    //    printf("%02x", data[i]);
    //    if (i % 16 == 15)
    //        printf("\n");
    //    else
    //        printf(" ");
    //}





    fr = f_close(&fil);
    if(fr != FR_OK)
    {
        printf("Failed to close\n");
        ErrorFlash(4);
        return;
    }


    printf("Read Done\n");
}

int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    uint8_t* data = malloc(0x4000 * 2); // 64
    OpenGame("Tetris.gb",data);
    //OpenGame("Pokemon.gb",data);

    //tud_cdc_set_wanted_char('\0');
    //while (!tud_cdc_connected()) { sleep_ms(100);  }
    //printf("tud_cdc_connected()\n");
//
    //printf("Running from flash OG\n");

    //vreg_set_voltage(VREG_VOLTAGE_1_30);
    //sleep_ms(1000);
    //set_sys_clock_khz(402000, true);
    
    //set_sys_clock_khz(200000, true); // 214us
    //set_sys_clock_khz(250000, true); // 171us
    //set_sys_clock_khz(266000, true); // 161us  (1596/6)
    //set_sys_clock_khz(270000, true); // 158us



    gpio_put(25, 1);

    // Reset address pins
    gpio_init_mask(address_pins);
    gpio_set_dir_masked(address_pins,0);


    // Reset data pins
    gpio_init_mask(data_pins);
    gpio_set_dir_masked(data_pins,data_pins);
    gpio_put_masked(data_pins,0);



    while(true)
    {
        uint8_t inst = data[readAddress()];

        uint32_t mask = 0;

        mask |= (inst & 0b11000000) << 20; // 26, 27
        mask |= (inst & 0b00111110) << 17; // 18, 19, 20, 21, 22
        mask |= (inst & 0b00000001) << 16; // 16
        gpio_put_masked(data_pins,
        mask
        );
    }



    free(data);



    return 0;
}