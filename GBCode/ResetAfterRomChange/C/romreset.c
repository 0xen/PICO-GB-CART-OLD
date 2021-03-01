#pragma codeseg RAMCODE

#include <gb/gb.h>

UBYTE my_cpu;
void reset_gameboy();
void my_ram_function(UINT8 data, UBYTE cpu) {
    
    (*(UBYTE *)(0x0000)) = data + 1;

    // Reset Screen
    UBYTE * address = (UBYTE *)(0x9800u); 
    for(UWORD i = 0 ; i < 32*32; ++i)
    {
        while(STAT_REG & 0b10);
        *address++ = 0;
    }

        while((STAT_REG & 0b11) == 1);
        while((STAT_REG & 0b11) != 1);
        while((STAT_REG & 0b11) == 1);
        while((STAT_REG & 0b11) != 1);
        while((STAT_REG & 0b11) == 1);
        while((STAT_REG & 0b11) != 1);

    my_cpu = cpu;
    reset_gameboy();

}

void reset_gameboy() __naked {
__asm
    ld a,(_my_cpu)
    jp 0x100
__endasm;
}