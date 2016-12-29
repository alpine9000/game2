	include "includes.i"
	xdef _SetupPalette
	
_SetupPalette:
	move.l	a6,-(sp)
	lea CUSTOM,a6
	move.w #$000,COLOR00(a6)
	move.w #$fff,COLOR01(a6)
	move.l	(sp)+,a6
	rts