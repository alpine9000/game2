	include "includes.i"
	xdef _MusicInit
	xdef _MusicStop
	
_MusicInit:
	movem.l	a0-a1/a6/d0,-(sp)
	lea	CUSTOM,a6

	move.l  vectorBase,a0
	; mt_install_cia(a6=CUSTOM, a0=AutoVecBase, d0=PALflag.b)
	move.b	#1,d0
	jsr	mt_install_cia	
	
	lea 	module,a0
	move.l	#0,a1
	move.l	#0,d0
	jsr	mt_init
	; mt_init(a6=CUSTOM,a0=TrackerModule, a1=Samples|NULL, d0=InitialSongPos)

	move.b	#1,mt_Enable
	movem.l	(sp)+,a0-a1/a6/d0
	rts


_MusicStop:
	move.l	a6,-(sp)
	lea	CUSTOM,a6
	jsr	mt_end
	move.l	(sp)+,a6
	rts
	
module:
	incbin "breath_of_life.mod"	
	