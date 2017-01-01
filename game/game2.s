	include "includes.i"

	xdef _custom
	xdef _spriteBitplanes

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
	jsr	_si_init
	jsr	_si_loop

	if TRACKLOADER=0
QuitGame:
	jmp	LongJump
	endif

	include "os.i"

	align 4
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
	if TRACKLOADER=1
startUserstack:
	ds.b	1000
userstack:
	endif
	end