	include "includes.i"


	xdef _custom
	xdef _bitplanes
	;; xdef _map
	;; xdef _tileset
	xdef _verticalBlankCount
	xdef _bitplanes
	xdef _copper
	
	if TRACKLOADER=1
byteMap:
	dc.l	Entry
	dc.l	endCode-byteMap
	endif

	include "wbstartup.i"		; does nothing if TRACKLOADER=1
	
Entry:
	if TRACKLOADER=0
	jmp 	StartupFromOS
	else
	lea	userstack,a7	
	endif

Main:
	jsr	_Init
	jsr	_SetupScreen
	jsr	_MusicInit
	
GameLoop:	
	jsr	WaitVerticalBlank

	if TRACKLOADER=0
	btst	#6,$bfe001  	; test LEFT mouse click
	bne 	GameLoop
	else
	bra	GameLoop
	endif

	if TRACKLOADER=0
QuitGame:
	jsr	_MusicStop
	jmp	LongJump
	endif

	include "os.i"

	align 4
_copper:
copper:
;;;  bitplane pointers must be first else poking addresses will be incorrect
	dc.w	BPL1PTL,0
	dc.w	BPL1PTH,0
	dc.w	BPL2PTL,0
	dc.w	BPL2PTH,0
	dc.w	BPL3PTL,0
	dc.w	BPL3PTH,0
	dc.w	BPL4PTL,0
	dc.w	BPL4PTH,0
	dc.w	BPL5PTL,0
	dc.w	BPL5PTH,0

	dc.l	$fffffffe

	if 0
_map:
	include "out/map-map.s"
	endif

	align 4
bitplanes:
_bitplanes:
	;; dcb.b	SCREEN_WIDTH_BYTES*SCREEN_BIT_DEPTH*SCREEN_HEIGHT,0
	incbin "out/background.bin"


	if 0
tileset:
_tileset:
	incbin "out/outzonea.bin"
	endif
_custom:
	dc.l	CUSTOM
	
	section	.bss	
	align 4
_verticalBlankCount:
verticalBlankCount:
	dc.l	0	
	if TRACKLOADER=1
startUserstack:
	ds.b	1000
userstack:
	endif
	end