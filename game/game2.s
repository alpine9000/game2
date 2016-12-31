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

	xdef _audio_beat1
	xdef _audio_beat2
	xdef _audio_beat3
	xdef _audio_beat4
	xdef _audio_shoot
	xdef _audio_invaderkilled
	xdef _audio_explosion
	
	
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
	jsr	_init_amiga
	jsr	_screen_setup
	jsr	_si_init
	
GameLoop:	
	jsr	_hw_waitVerticalBlank
	jsr	_hw_readJoystick
	jsr	_si_loop
	if TRACKLOADER=0
	btst	#6,$bfe001  	; test LEFT mouse click
	bne 	GameLoop
	else
	bra	GameLoop
	endif

	if TRACKLOADER=0
QuitGame:
	jmp	LongJump
	endif


	include "os.i"

	align 4
_copper:
copper:
;;;  bitplane pointers must be first else poking addresses will be incorrect
	dc.w	BPL1PTL,0
	dc.w	BPL1PTH,0
	dc.w    $d407,$fffe
	dc.w    COLOR01,$0f0
	dc.w    $ffdf,$fffe
	dc.w    $0d07,$fffe	
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

	align 4
bitplanes:
_bitplanes:
	dcb.b	SCREEN_WIDTH_BYTES*SCREEN_BIT_DEPTH*SCREEN_HEIGHT,0
_spriteBitplanes:
	incbin	"out/sprite.bin"

_audio_beat1:
	incbin  "out/beat1.raw"
_audio_beat2:
	incbin  "out/beat2.raw"
_audio_beat3:
	incbin  "out/beat3.raw"
_audio_beat4:
	incbin  "out/beat4.raw"	
_audio_shoot:	
	incbin  "out/shoot.raw"
_audio_explosion:
	incbin  "out/explosion.raw"
_audio_invaderkilled:
	incbin  "out/invaderkilled.raw"
	
	align 4
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