/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"


extern const unsigned char bootloaderCode[];
extern unsigned int bootloaderCodeSize;

// ROMS
extern const unsigned char TETRISCode[];
extern unsigned int TETRISCodeSize;
extern const unsigned char DRMARIOCode[];
extern unsigned int DRMARIOCodeSize;
extern const unsigned char SUPERMARIOLANDCode[];
extern unsigned int SUPERMARIOLANDCodeSize;
extern const unsigned char MARIOANDYOSHICode[];
extern unsigned int MARIOANDYOSHICodeSize;

const uint16_t romCount = 3;
const unsigned char* romInstances[] = {
    TETRISCode,
    DRMARIOCode,
    SUPERMARIOLANDCode
};

void MainMenu();

// Banking Methods
void GameRunRomOnly();
void GameRunMBC1();
void GameRunMBC1AndRam();

#define BANK_SIZE 0x4000

#define RESET_PIN 28
#define WR_PIN 27

uint8_t pMemoryBanks[4 * 0x8000];

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

inline static void dataRead()
{
    gpio_set_dir_in_masked(dataPinMask);
}

inline static void dataWrite()
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

void LoadGame(const unsigned char* data)
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
	//if (mRamBankSize > 0)
	//{
    //    switch (mRamBankSize)
	//    {
	//    case 1:
	//    	mRamSizeBytes = 1024 * 2;
	//    	break;
	//    case 2:
	//    	mRamSizeBytes = 1024 * 8;
	//    	break;
	//    case 3:
	//    	mRamSizeBytes = 1024 * 32;
	//    	break;
	//    case 4:
	//    	mRamSizeBytes = 1024 * 128;
	//    	break;
	//    }
	//	mRam = (uint8_t*) malloc(mRamSizeBytes);
	//}

    //uint32_t offset = 0;
    //// Prepare the banks
    //for(uint8_t i = 0 ; i < (mRomBankSize - 1); ++i)
    //{
    //    // Bank 0
    //    for(uint32_t j = 0 ; j < BANK_SIZE; ++j)
    //    {
    //        pMemoryBanks[offset + j] = data[j];
    //    }
    //    offset+=BANK_SIZE;
    //    // Bank 'n'
    //    for(uint32_t k = 0 ; k < BANK_SIZE; ++k)
    //    {
    //        pMemoryBanks[offset + k] = data[(i + 1) * BANK_SIZE + k];
    //    }
    //    offset+=BANK_SIZE;
    //}

    for(uint32_t i = 0 ; i < BANK_SIZE * mRomBankSize; ++i)
    {
        pMemoryBanks[i] = data[i];
    }

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

inline void SelectGame(uint8_t data)
{
    if(data >0)
    {
        LoadGame(romInstances[data-1]);
        return;
    }
}

void LoadBootrom()
{
    for(uint32_t j = 0 ; j < bootloaderCodeSize; ++j)
    {
        pMemoryBanks[j] = bootloaderCode[j];
    }

}

void MainMenu()
{

    static uint16_t lastAddress = 0;
    uint16_t address = 0;
    uint32_t outputData = 0;
    uint8_t* bankPtr = pMemoryBanks;
    ResetConsole();
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

                outputData |= ((uint32_t)(pMemoryBanks[address]>>0)&0b1111111)<<16;
                outputData |= ((uint32_t)(pMemoryBanks[address]>>7)&0b1)<<26;

                gpio_put_masked(dataPinMask, outputData);
            }
            else
            {
                dataRead();
                SelectGame(ReadInputData());
            }
        }
    }
}
 
void GameRunRomOnly()
{
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
    //uint8_t m_mode = 0;
    uint8_t m_rom_bank_high = 0;
    uint8_t m_memory_bank = 0;

    uint32_t bank_offset = 0;
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

                if(address >=0x4000)
                {
                    
                    address += bank_offset;
                }
                    outputData |= ((uint32_t)(currentBank[address]>>0)&0b1111111)<<16;
                    outputData |= ((uint32_t)(currentBank[address]>>7)&0b1)<<26;


                gpio_put_masked(dataPinMask, outputData);
            }
            else
            {
                dataRead();
                data = ReadInputData();

                switch (address & 0xE000)
                {
                case 0x2000:
                    {
                        //if (m_mode == 0)
                        //{
                        //    m_memory_bank = (data & 0x1F) | (m_rom_bank_high << 5);
                        //}
                        //else
                        //{
                            m_memory_bank = data & 0x1f;
                        //}
                        // If rom bank is set to 0x00,0x20,0x40,0x60 we incroment it

                        //if (m_memory_bank == 0x00 || m_memory_bank == 0x20 ||
                        //    m_memory_bank == 0x40 || m_memory_bank == 0x60)
                        //    m_memory_bank++;


                        m_memory_bank &= (mRomBankSize - 1);
                        
                        bank_offset = (m_memory_bank - 1) * 0x4000;
                        //m_memory_bank--;

                        //currentBank = &pMemoryBanks[0x8000 * m_memory_bank];
                        //if(m_memory_bank!=1)
                        //{
                        //    gpio_put(25,1);
                        //while(true){}
                        //}
                        break;
                    }
                case 0x4000:
                {
                    //m_rom_bank_high = data & 0x03;
                    //m_memory_bank = (m_memory_bank & 0x1F) | (m_rom_bank_high << 5);
                    //// If rom bank is set to 0x00,0x20,0x40,0x60 we incroment it
                    ////if (m_memory_bank == 0x00 || m_memory_bank == 0x20 ||
                    ////    m_memory_bank == 0x40 || m_memory_bank == 0x60)
                    ////    m_memory_bank++;
    //
                    //m_memory_bank &= (mRomBankSize - 1);
    //
                    //// Load into memory bank 1
                    //currentBank = &pMemoryBanks[0x8000 * m_memory_bank];
                    break;
                }
                }


            }
        }
    }
}
void GameRunMBC1AndRam(){}


int main() {
    stdio_init_all();

    set_sys_clock_khz(270000, true);

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