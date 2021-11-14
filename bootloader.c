/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include <stdint.h>
#include <string.h>
#include "hardware/regs/addressmap.h"
#include "hardware/regs/m0plus.h"
#include "hardware/structs/watchdog.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/resets.h"
#include "hardware/dma.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/binary_info.h"

#include "sd_card.h"
#include "ff.h"


static const uint32_t BOOTLOADER_SIZE = 0x20000;

static const uint32_t FLASH_MAGIC1 = 0x8ecd5efb; // Randomly picked numbers
static const uint32_t FLASH_MAGIC2 = 0xc5ae52a9;

#define bl2crc(x)      (*((uint32_t*)(((uint32_t)(x) + 0xfc))))

static const uint32_t  sStart = XIP_BASE + BOOTLOADER_SIZE;

#define flashoffset(x) (((uint32_t)x) - XIP_BASE)


#define UF2_HEADER_32_MAGIC0 0
#define UF2_HEADER_32_MAGIC1 1
#define UF2_HEADER_32_FLAGS 2
#define UF2_HEADER_32_ADDRESS 3
#define UF2_HEADER_32_SIZE 4
#define UF2_HEADER_32_BLOCK_NUM 5
#define UF2_HEADER_32_BLOCK_COUNT 6
#define UF2_HEADER_32_FILE_SIZE 7

#define UF2_HEADER_8_DATA 32

#define UF2_HEADER_32_MAGIC_END 127


bool Test(uint32_t address, uint8_t* sd_data, size_t len)
{
    char* romData = (char*)address;
    printf("%02x\n",address);

    for (size_t i = 0; i < len; ++i)
    {
        printf("%02x", romData[i]);
        if (i % 16 == 15)
            printf("\n");
        else
            printf(" ");
    }
    printf("\n");
    printf("\n");

    for (size_t i = 0; i < len; ++i)
    {
        printf("%02x", sd_data[i]);
        if (i % 16 == 15)
            printf("\n");
        else
            printf(" ");
    }

    for (size_t i = 0; i < len; ++i)
    {
        if(romData[i] != sd_data[i])
        {
            return true;
        }
    }
    return false;
}


uint32_t crc32(const void *data, size_t len, uint32_t crc)
{
    // Nothing else is running on the system, so it doesn't matter which
    // DMA channel we use
    static const uint8_t channel = 0;

    uint8_t dummy;

    dma_channel_config c = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_sniff_enable(&c, true);

    // Turn on CRC32 (non-bit-reversed data)
    dma_sniffer_enable(channel, 0x00, true);
    dma_hw->sniff_data = crc;

    dma_channel_configure(
        channel,
        &c,
        &dummy,
        data,
        len,
        true    // Start immediately
    );

    dma_channel_wait_for_finish_blocking(channel);

    return(dma_hw->sniff_data);
}

int startMainApplication()
{
    printf("Attempting to run application\n");
    //if(crc32((const void*)sStart, 252, 0xffffffff) == bl2crc(sStart))
    {
        if((watchdog_hw->scratch[0] == FLASH_MAGIC1) ||
           (watchdog_hw->scratch[0] == ~FLASH_MAGIC1))
        {
            watchdog_hw->scratch[0] = 0;
        }

        uint32_t address = sStart + 0x100;
        printf("Running memory address %02x\n",address);

        asm volatile (
        "ldr r0, =%[start]\n"
        "ldr r1, =%[vtable]\n"
        "str r0, [r1]\n"
        "ldmia r0, {r0, r1}\n"
        "msr msp, r0\n"
        "bx r1\n"
        :
        : [start] "X" (address), [vtable] "X" (PPB_BASE + M0PLUS_VTOR_OFFSET)
        :
        );
    }
    printf("Failed to start\n");

    // We will only return if the main application couldn't be started
    return 0;
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
typedef struct __packed __aligned(4)
{
    uint32_t magic1;
    uint32_t magic2;
    uint32_t length;
    uint32_t crc32;
    uint8_t  data[];
}tFlashHeader;


const uint32_t UF2_FLASH_SECTOR_SIZE = 512;


bool FirmwareDiffrence(const char* filename)
{
    FRESULT fr;
    FIL fil;
    fr = f_open(&fil, filename, FA_READ);
    if(fr != FR_OK)
    {
        printf("Failed to open\n");
        return;
    }
    
    UINT br = 0;
    uint8_t  buffer[UF2_FLASH_SECTOR_SIZE];
    uint32_t* header = (uint32_t*)buffer;
    while (!f_eof(&fil))
    {
        fr = f_read(&fil, buffer, UF2_FLASH_SECTOR_SIZE, br);
        if (FR_OK != fr)
        {
            printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
        }
        if( header[UF2_HEADER_32_MAGIC0] == 0x0A324655 &&
            header[UF2_HEADER_32_MAGIC1] == 0x9E5D5157 &&
            header[UF2_HEADER_32_MAGIC_END] == 0xAB16F30)
        {
            if(Test(header[UF2_HEADER_32_ADDRESS],&buffer[UF2_HEADER_8_DATA],256))
            {

                fr = f_close(&fil);
                if(fr != FR_OK)
                {
                    printf("Failed to close\n");
                    return false;
                }
                return true;
            }
        }
    }

    fr = f_close(&fil);
    if(fr != FR_OK)
    {
        printf("Failed to close\n");
        return false;
    }
    
    return false;
}

void UpdateFirmware(const char* filename)
{

    FRESULT fr;
    
    FIL fil;
    fr = f_open(&fil, filename, FA_READ);
    if(fr != FR_OK)
    {
        printf("Failed to open\n");
        return;
    }


    int firmwareSize = f_size(&fil);
    int numSectors = (firmwareSize / UF2_FLASH_SECTOR_SIZE);

    int eraseSize = numSectors * 256;


    eraseSize = (eraseSize + 4095) & 0xfffff000;

    // If we dont erase in time, reboot
    watchdog_reboot(0, 0, 500);

    flash_range_erase(flashoffset(sStart), eraseSize);

    UINT br = 0;

    uint8_t  buffer[UF2_FLASH_SECTOR_SIZE];
    uint32_t* header = (uint32_t*)buffer;

    while (!f_eof(&fil))
    {
        fr = f_read(&fil, buffer, UF2_FLASH_SECTOR_SIZE, br);
        if (FR_OK != fr)
        {
            printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
        }
        if( header[UF2_HEADER_32_MAGIC0] == 0x0A324655 &&
            header[UF2_HEADER_32_MAGIC1] == 0x9E5D5157 &&
            header[UF2_HEADER_32_MAGIC_END] == 0xAB16F30)
        {
            // Reset the watchdog counter
            watchdog_update();

            uint32_t addressAfterBase = header[UF2_HEADER_32_ADDRESS] - XIP_BASE;


            
            //uint32_t ints = save_and_disable_interrupts();
            flash_range_program(addressAfterBase, &buffer[UF2_HEADER_8_DATA], 256);
            //restore_interrupts(ints);

        }
    }


    fr = f_close(&fil);
    if(fr != FR_OK)
    {
        printf("Failed to close\n");
        return;
    }
}

void ProcessFirmware()
{
    
    InitSD();



    FRESULT fr;
    FATFS fs;
    int ret;
    char filename[] = "firmware.uf2";

    fr = f_mount(&fs,"0:",1);
    if(fr != FR_OK)
    {
        printf("Failed to Mount\n");
        return;
    }


    if(FirmwareDiffrence(filename))
    {
        UpdateFirmware(filename);
    }










    // Disable the watchdog
    hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);


}


int main()
{
    stdio_init_all();

    //tud_cdc_set_wanted_char('\0');
    //while (!tud_cdc_connected()) { sleep_ms(100);  }
    //printf("tud_cdc_connected()\n");


    //flash_range_erase(0x40000, FLASH_SECTOR_SIZE);













    //ProcessFirmware();














    watchdog_hw->scratch[0] = 10;

    /*
        while(true)
            tight_loop_contents();
            */


    startMainApplication();
    
    // Otherwise go to the bootrom bootloader as a last resort
    //reset_usb_boot(0, 0);
    return 0;
}