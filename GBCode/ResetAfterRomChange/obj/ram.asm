;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.0.7 #12016 (MINGW64)
;--------------------------------------------------------
	.module romreset
	.optsdcc -mgbz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _my_ram_function
	.globl _my_cpu
	.globl _reset_gameboy
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
_my_cpu::
	.ds 1
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _RAMCODE
;./C/romreset.c:7: void my_ram_function(UINT8 data, UBYTE cpu) {
;	---------------------------------
; Function my_ram_function
; ---------------------------------
_my_ram_function::
;./C/romreset.c:9: (*(UBYTE *)(0x0000)) = data + 1;
	ldhl	sp,	#2
	ld	a, (hl)
	inc	a
	ld	(#0x0000),a
;./C/romreset.c:13: for(UWORD i = 0 ; i < 32*32; ++i)
	ld	bc, #0x9800
	ld	de, #0x0000
00124$:
	ld	a, d
	sub	a, #0x04
	jr	NC, 00105$
;./C/romreset.c:15: while(STAT_REG & 0b10);
00101$:
	ldh	a, (_STAT_REG+0)
	bit	1, a
	jr	NZ, 00101$
;./C/romreset.c:16: *address++ = 0;
	xor	a, a
	ld	(bc), a
	inc	bc
;./C/romreset.c:13: for(UWORD i = 0 ; i < 32*32; ++i)
	inc	de
	jr	00124$
;./C/romreset.c:19: while((STAT_REG & 0b11) == 1);
00105$:
	ldh	a, (_STAT_REG+0)
	and	a, #0x03
	ld	b, a
	ld	c, #0x00
	ld	a, b
	dec	a
	or	a, c
	jr	Z, 00105$
;./C/romreset.c:20: while((STAT_REG & 0b11) != 1);
00108$:
	ldh	a, (_STAT_REG+0)
	and	a, #0x03
	ld	b, a
	ld	c, #0x00
	ld	a, b
	dec	a
	or	a, c
	jr	NZ, 00108$
;./C/romreset.c:21: while((STAT_REG & 0b11) == 1);
00111$:
	ldh	a, (_STAT_REG+0)
	and	a, #0x03
	ld	b, a
	ld	c, #0x00
	ld	a, b
	dec	a
	or	a, c
	jr	Z, 00111$
;./C/romreset.c:22: while((STAT_REG & 0b11) != 1);
00114$:
	ldh	a, (_STAT_REG+0)
	and	a, #0x03
	ld	b, a
	ld	c, #0x00
	ld	a, b
	dec	a
	or	a, c
	jr	NZ, 00114$
;./C/romreset.c:23: while((STAT_REG & 0b11) == 1);
00117$:
	ldh	a, (_STAT_REG+0)
	and	a, #0x03
	ld	b, a
	ld	c, #0x00
	ld	a, b
	dec	a
	or	a, c
	jr	Z, 00117$
;./C/romreset.c:24: while((STAT_REG & 0b11) != 1);
00120$:
	ldh	a, (_STAT_REG+0)
	and	a, #0x03
	ld	b, a
	ld	c, #0x00
	ld	a, b
	dec	a
	or	a, c
	jr	NZ, 00120$
;./C/romreset.c:26: my_cpu = cpu;
	ldhl	sp,	#3
	ld	a, (hl)
	ld	(#_my_cpu),a
;./C/romreset.c:27: reset_gameboy();
;./C/romreset.c:29: }
	jp  _reset_gameboy
;./C/romreset.c:31: void reset_gameboy() __naked {
;	---------------------------------
; Function reset_gameboy
; ---------------------------------
_reset_gameboy::
;./C/romreset.c:35: __endasm;
	ld	a,(_my_cpu)
	jp	0x100
;./C/romreset.c:36: }
	.area _RAMCODE
	.area _CABS (ABS)
