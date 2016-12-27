	include "includes.i"
	xdef _SetupPalette
	
_SetupPalette:
	move.l	a6,-(sp)
	lea CUSTOM,a6
	include "out/background-palette.s"
	move.l	(sp)+,a6
	rts