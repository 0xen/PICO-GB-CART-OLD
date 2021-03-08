/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"


#define MAX_GAMES 10

#define BANK_SIZE 0x4000

#define RESET_PIN 28
#define WR_PIN 27

extern unsigned char bootloaderCode[];
extern unsigned int bootloaderCodeSize;

// ROMS
extern unsigned char TETRISCode[];
extern unsigned int TETRISCodeSize;
extern unsigned char DRMARIOCode[];
extern unsigned int DRMARIOCodeSize;
extern unsigned char SUPERMARIOLANDCode[];
extern unsigned int SUPERMARIOLANDCodeSize;
extern unsigned char MARIOANDYOSHICode[];
extern unsigned int MARIOANDYOSHICodeSize;
extern unsigned char ALSTARCHALLENGE2Code[];
extern unsigned int ALSTARCHALLENGE2CodeSize;
extern unsigned char pocketCode[];
extern unsigned int pocketCodeSize;
extern unsigned char ZELDACode[];
extern unsigned int ZELDACodeSize;

const uint16_t romCount = 3;
unsigned char* romInstances[] = {
    TETRISCode,
    DRMARIOCode,
    pocketCode
    
    //MARIOANDYOSHICode
};


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

void MainMenu();

// Banking Methods
void GameRunRomOnly();
void GameRunMBC1();
void GameRunMBC1AndRam();

uint8_t* pMemoryBanks = NULL;

void InitOutput(int pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
}

void InitInput(int pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

void ResetConsole()
{
    gpio_put(RESET_PIN, 0);
    sleep_ms(100);
    gpio_put(RESET_PIN, 1);
}

uint32_t dataPinMask = 0b100011111110000000000000000;
uint32_t addressPinMask = 0b1111111111111111;

void dataRead()
{
    gpio_set_dir_in_masked(dataPinMask);
}

void dataWrite()
{
    gpio_set_dir_out_masked(dataPinMask);
}

uint8_t ReadInputData()
{
    uint32_t allPins = gpio_get_all();
    uint8_t responce = (allPins >> 16) & 0b1111111;
    // 8th bit
    responce |= ((allPins >> 26) & 0b1)<<7;
    return responce;
}

static const uint16_t TITLE_MEMORY_LOCATION = 0x0134;
static const uint16_t DISTINATION_CODE = 0x014A;
static const uint16_t PLATFORM = 0x0143;
static const uint16_t CARTRIDGE_TYPE = 0x0147;
static const uint16_t ROM_SIZE = 0x0148;
static const uint16_t RAM_SIZE = 0x0149;

uint8_t mRomBankSize = 0;
uint8_t mRamBankSize = 0;
uint8_t mRomBank = 0;
uint8_t mMBCType = 0;
uint8_t mRomBankHigh = 0;
uint8_t mRamBank = 0;
uint16_t mRamOffset = 0;
uint32_t mRamSizeBytes = 0;
uint16_t mCartMode = 0;
bool mCartRamEnabled = false;

uint8_t* mRam = NULL;

void LoadGame(uint8_t* data)
{
    // Load Header information
    mRomBankSize = data[ROM_SIZE];
    switch(mRomBankSize)
    {
        case 0: { mRomBankSize = 2; break; }
        case 1: { mRomBankSize = 4; break; }
        case 2: { mRomBankSize = 8; break; }
        case 3: { mRomBankSize = 16; break; }
        case 4: { mRomBankSize = 32; break; }
        case 5: { mRomBankSize = 64; break; }
        case 6: { mRomBankSize = 128; break; }
    }
    mRamBankSize = data[RAM_SIZE];
    mMBCType = data[CARTRIDGE_TYPE];

    pMemoryBanks = data;

    switch(mMBCType)
    {
        case 0x0: // ROM Only
        {
            GameRunRomOnly();
            break;
        }
        case 0x1: // MBC1
        {
            GameRunMBC1();
            break;
        }
    }
}

void SelectGame(uint8_t data)
{  
    if(data >0 && data <= romCount)
    {
        LoadGame((uint8_t*)romInstances[data-1]);
        return;
    }
}

void LoadBootrom()
{
    pMemoryBanks = (uint8_t*)bootloaderCode;
}

void MainMenu()
{

    static uint16_t lastAddress = 0;
    uint16_t address = 0;
    uint32_t outputData = 0;
    uint8_t* bankPtr = pMemoryBanks;
    uint8_t data = 0;

    static const uint8_t titleSize = 1 + (16 * MAX_GAMES);
    uint8_t titles[titleSize];

    {
        titles[0] = romCount;
        uint16_t offset = 1;
        for(uint16_t i = 0 ; i < romCount; i++)
        {
            for(uint16_t j = 0 ; j < 16; j++)
            {
                titles[offset++] = romInstances[i][0x134 + j];
            }
        }
    }

    ResetConsole();
    while(true)
    {   
        address = gpio_get_all() & 0b1111111111111111;

        
        if(!gpio_get(WR_PIN))
        {
            if(address!=lastAddress)
            {
                lastAddress = address;
                dataWrite();

                outputData = 0;

                if(address < 0x8000)
                {
                    data = pMemoryBanks[address];
                }
                else if(address >= 0xA000 && address <= 0xA000 + titleSize)
                {
                    data = titles[address-0xA000];
                }

                outputData |= ((uint32_t)(data>>0)&0b1111111)<<16;
                outputData |= ((uint32_t)(data>>7)&0b1)<<26;

                gpio_put_masked(dataPinMask, outputData);
            }
        }
        else
        {
            dataRead();
            SelectGame(ReadInputData());
        }
    }
}
 
void GameRunRomOnly()
{
    set_sys_clock_khz(270000, true);
    ResetConsole();
    
    static uint16_t lastAddress = 0;
    uint16_t address = 0;
    uint32_t outputData = 0;

    uint8_t* currentBank = pMemoryBanks;
    while(true)
    {   
        address = gpio_get_all() & 0b1111111111111111;
        if(address!=lastAddress)
        {
            lastAddress = address;
            if(!gpio_get(WR_PIN))
            {
                dataWrite();

                outputData = 0;

                outputData |= ((uint32_t)(currentBank[address]>>0)&0b1111111)<<16;
                outputData |= ((uint32_t)(currentBank[address]>>7)&0b1)<<26;

                gpio_put_masked(dataPinMask, outputData);
            }
        }
    }
}

void GameRunMBC1()
{
    

    ResetConsole();
    
    static uint16_t lastAddress = 0;
    uint32_t address = 0;
    uint32_t outputData = 0;

    uint8_t* currentBank = pMemoryBanks; 

    uint8_t data = 0;
    uint8_t mode = 0;
    uint8_t rom_bank_high = 0;
    uint8_t memory_bank = 0;

    uint32_t bank_offset = 0;
    while(true)
    {   
        address = gpio_get_all() & 0b1111111111111111;
        
        if(!gpio_get(WR_PIN))
        {
            if(address!=lastAddress)
            {
                lastAddress = address;
                dataWrite();

                outputData = 0;

                if(address >=0x4000)
                {
                    address += bank_offset;
                }

                outputData |= ((uint32_t)(currentBank[address]>>0)&0b1111111)<<16;
                outputData |= ((uint32_t)(currentBank[address]>>7)&0b1)<<26;


                gpio_put_masked(dataPinMask, outputData);
            }
        }
        else
        {
            dataRead();
            data = ReadInputData();

            switch (address & 0xE000)
            {
                case 0x2000:
                {
                    if (mode == 0)
                    {
                        memory_bank = (data & 0x1F) | (rom_bank_high << 5);
                    }
                    else
                    {
                        memory_bank = data & 0x1f;
                    }
                    // If rom bank is set to 0x00,0x20,0x40,0x60 we incroment it
                    if (memory_bank == 0x00 || memory_bank == 0x20 ||
                        memory_bank == 0x40 || memory_bank == 0x60)
                        memory_bank++;


                    memory_bank &= (mRomBankSize - 1);
                    
                    bank_offset = (memory_bank - 1) * 0x4000;
                    break;
                }
                case 0x4000:
                {
                    rom_bank_high = data & 0x03;
                    memory_bank = (memory_bank & 0x1F) | (rom_bank_high << 5);
                    // If rom bank is set to 0x00,0x20,0x40,0x60 we incroment it
                    if (memory_bank == 0x00 || memory_bank == 0x20 ||
                        memory_bank == 0x40 || memory_bank == 0x60)
                        memory_bank++;

                    memory_bank &= (mRomBankSize - 1);

                    bank_offset = (memory_bank - 1) * 0x4000;
                    break;
                }
                case 0x6000: // Ram-Rom Mode
                {
		            mode = data & 0x01;
                    break;
                }
            }

        }
    }
}
void GameRunMBC1AndRam(){}


int main() {
    stdio_init_all();

    vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_ms(1000);
    set_sys_clock_khz(360000, true);

    InitOutput(25);

    InitOutput(RESET_PIN);
    gpio_put(RESET_PIN, 1);

    InitInput(WR_PIN);


    gpio_init_mask(addressPinMask);
    gpio_set_dir_in_masked(addressPinMask);

    gpio_init_mask(dataPinMask);
    dataWrite();

    LoadBootrom();

    MainMenu();
}