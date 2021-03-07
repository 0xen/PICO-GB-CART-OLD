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
	ld	hl, #0x0000
	ld	(hl), a
;./C/romreset.c:10: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:11: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:12: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:13: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:14: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:15: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:16: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:17: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:18: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:19: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:20: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:21: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:22: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:23: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:24: (*(UBYTE *)(0x0000)) = data + 1;
	ld	l, h
	ld	(hl), a
;./C/romreset.c:44: }
	ret
;./C/romreset.c:46: void reset_gameboy() __naked {
;	---------------------------------
; Function reset_gameboy
; ---------------------------------
_reset_gameboy::
;./C/romreset.c:50: __endasm;
	ld	a,(_my_cpu)
	jp	0x100
;./C/romreset.c:51: }
	.area _RAMCODE
	.area _CABS (ABS)
