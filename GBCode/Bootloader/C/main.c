
#include <stdio.h>

#include <gb/gb.h>
#include <gb/font.h>
#include <gb/console.h>
#include <gb/drawing.h>

#include "../../ResetAfterRomChange/GB/romreset.h"

#define BACKGROUND_ADDR 0x9800u
#define WINDOW_ADDR 0x9C00u

unsigned char fontRemapping[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,96,97,
0,0,0,0,0,0,0,0,0,0,0,0,98,99,100,101,
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95
};


extern const unsigned char RomResetCode[];
extern unsigned int RomResetCodeSize;

UINT8 gameCount = 3;
UINT8 gameCursor = 0;

UBYTE joypadData;
UBYTE joypadLast;

UBYTE strlen(const UBYTE *str) {
    UBYTE len = 0;
    while (*str++) len++;
    return len; 
}

void memcpy(UBYTE *dest, const UBYTE *src, UWORD size) {
    UBYTE * d = dest; 
    const UBYTE *s = src; 
    for (UWORD i = size; i != 0; i--) 
        *d++ = *s++;
}

void toDisplay(UBYTE* address, const UBYTE* data, UWORD size)
{
    const UBYTE *d = data; 
    for(UBYTE i = 0 ; i < size; ++i)
    {
        while(STAT_REG & 0b10);
        *address++ = fontRemapping[*d++];
    }
}

void BackgroundPrintf(UBYTE x, UBYTE y, const unsigned char * text) {
    UBYTE * addr = (UBYTE *)(BACKGROUND_ADDR + ((UWORD)y << 5) + x); 
    toDisplay(addr, text, strlen(text));
}

void WindowPrintf(UBYTE x, UBYTE y, const unsigned char * text) {
    UBYTE * addr = (UBYTE *)(WINDOW_ADDR + ((UWORD)y << 5) + x); 
    toDisplay(addr, text, strlen(text));
}

void ClearCursor()
{
    BackgroundPrintf( 0, 1 + gameCursor, " " );
}

void RenderCursor()
{
    BackgroundPrintf( 0, 1 + gameCursor, ">" );
}

BOOLEAN ButtonInteractive(UINT8 key)
{
    if(joypadData & key)
    {
        if(!(joypadLast & key))
        {
            return TRUE;
        }
    }
    return FALSE;
}

typedef void (*ResetRom_t)(UINT8,UBYTE);
const ResetRom_t ResetRom = (ResetRom_t)(RAM_CODE_ADDR);

void PrepareRomChange()
{
    memcpy((void *) RAM_CODE_ADDR, RomResetCode, RomResetCodeSize);
}


void main()
{

    PrepareRomChange();

    font_t font;
    font_init();

    color(WHITE, BLACK, SOLID);

    font = font_load(font_ibm);
    font_set(font);


    mode(get_mode() | M_NO_SCROLL);

    

    
    BackgroundPrintf( 8, 0, "Games" );
    BackgroundPrintf( 1, 1, "TETRIS" );
    BackgroundPrintf( 1, 2, "DR MARIO" );
    BackgroundPrintf( 1, 3, "Supper Mario" );
    RenderCursor();


    while(TRUE)
    {
        wait_vbl_done();

        joypadData = joypad();

        
        if(ButtonInteractive(J_UP))
        {
            if(gameCursor>0)
            {
                ClearCursor();
                gameCursor--;
                RenderCursor();
            }
        }
        
        if(ButtonInteractive(J_DOWN))
        {
            if(gameCursor<gameCount - 1)
            {
                ClearCursor();
                gameCursor++;
                RenderCursor();
            }
        }


        if(joypadData & J_A)
        {
            PrepareRomChange();
            ResetRom(gameCursor,_cpu);
        }

        joypadLast = joypadData;
    }
}