	include "includes.i"


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
	;; 	jsr	_Render		
	jsr	_Init
	jsr	_SetupScreen

	if 0
	move.l	#0,d0
	;; MusicStart
	endif


	lea 	module,a0
	move.l	#0,a1
	move.l	#0,d0
	jsr	mt_init
	; mt_init(a6=CUSTOM,a0=TrackerModule, a1=Samples|NULL, d0=InitialSongPos)
	move.l  vectorBase,a0
	; mt_install_cia(a6=CUSTOM, a0=AutoVecBase, d0=PALflag.b)
	move.b	#1,d0
	jsr	mt_install_cia

	move.b	#1,mt_Enable
	endif
	
GameLoop:	
	jsr	WaitVerticalBlank
	bra	GameLoop

	if TRACKLOADER=0
QuitGame:
	;; MusicStop
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
	;; 	dcb.b	SCREEN_WIDTH_BYTES*SCREEN_BIT_DEPTH*SCREEN_HEIGHT,0
	incbin "out/background.bin"

module:
	incbin "breath_of_life.mod"

	if 0
tileset:
_tileset:
	incbin "out/outzonea.bin"
	endif
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