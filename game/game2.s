	include "includes.i"

SCREEN_WIDTH_BYTES=320/8
SCREEN_HEIGHT=278
SCREEN_BIT_DEPTH=1
	
	xdef _custom
	xdef _bitplanes
	xdef _spriteBitplanes
	xdef _verticalBlankCount
	xdef _bitplanes
	xdef _copper
	xdef _WaitBlitter
	
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
	;; jsr	_MusicInit
	jsr	_SpaceInvadersInit
	
GameLoop:	
	jsr	WaitVerticalBlank
	jsr	ReadJoystick
	jsr	_SpaceInvadersLoop
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


_WaitBlitter:
	move.l	a6,-(sp)
	lea 	CUSTOM,a6
	tst 	DMACONR(a6)		;for compatibility
.waitblit:
	btst 	#6,DMACONR(a6)
	bne.s 	.waitblit
	move.l	(sp)+,a6
	rts
	
	include "os.i"

	align 4
_copper:
copper:
;;;  bitplane pointers must be first else poking addresses will be incorrect
	dc.w	BPL1PTL,0
	dc.w	BPL1PTH,0
	dc.w    $d007,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $ffdf,$fffe
	dc.w    $0c07,$fffe	
	dc.w    COLOR01,$fff

	dc.w    $0d47,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $0d67,$fffe
	dc.w    COLOR01,$fff
	dc.w    $0e47,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $0e67,$fffe
	dc.w    COLOR01,$fff
	dc.w    $0f47,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $0f67,$fffe
	dc.w    COLOR01,$fff
	dc.w    $1047,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $1067,$fffe
	dc.w    COLOR01,$fff
	dc.w    $1147,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $1167,$fffe
	dc.w    COLOR01,$fff
	dc.w    $1247,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $1267,$fffe
	dc.w    COLOR01,$fff
	dc.w    $1347,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $1367,$fffe
	dc.w    COLOR01,$fff
	dc.w    $1447,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $1467,$fffe
	dc.w    COLOR01,$fff
	dc.l	$fffffffe

	if 0
_map:
	include "out/map-map.s"
	endif

	align 4
bitplanes:
_bitplanes:
	dcb.b	SCREEN_WIDTH_BYTES*SCREEN_BIT_DEPTH*SCREEN_HEIGHT,0
_spriteBitplanes:
	incbin	"out/sprite.bin"
	;; dcb.b	SCREEN_WIDTH_BYTES*SCREEN_BIT_DEPTH*SCREEN_HEIGHT,0

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