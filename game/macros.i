MusicStart:	macro
	if MUSIC=1
	jsr	_MusicStart
	endif
	endm

MusicPlay:	macro
	if MUSIC=1
	jsr	_MusicPlay
	endif
	endm

MusicStop:	macro
	if MUSIC=1
	jsr	_MusicStop
	endif
	endm

IntsOff:	macro
	move	#$7fff,INTENA(a6) 	; disable all interrupts
	endm
IntsOn:		macro
	move.w	#(INTF_SETCLR|INTF_VERTB|INTF_INTEN),INTENA(a6)			
	endm
	
WaitScanLines: macro
	if \1 != 0
	lea 	$dff006,a0
	move.w	#\1-1,d2
.\@nTimes:
	move.w	(a0),d0
	lsr.w	#8,d0
.\@loop:
	move.w	(a0),d1
	lsr.w	#8,d1
	cmp.w	d0,d1
	beq	.\@loop
	dbra	d2,.\@nTimes
.\@done:
	endif
	endm

WaitBlitter:	macro
	tst	DMACONR(a6)		;for compatibility
.\@:
	btst	#6,DMACONR(a6)
	bne.s 	.\@
	endm


P61Module: macro
	cnop	0,512	
diskmodule\1:
	incbin	\2
	cnop	0,512
enddiskmodule\1:
	endm


RenderSkippedFramesCounter: macro
	if 	SKIPPED_FRAMES_DISPLAY=1
	move.l	verticalBlankCount,d0
	move.l	frameCount,d1	
	cmp.l	d1,d0
	beq	.noSkippedFrames
	addq	#1,d0
	cmp.l	d1,d0
	beq	.noSkippedFrames
	move.l	frameCount,verticalBlankCount
	lea	skippedFramesCounterText,a0
	jsr	IncrementCounter
	lea	skippedFramesCounterText,a1	
	move.w	#SCREEN_WIDTH-(4*8),d0
	move.w	#0,d1
	jsr	RenderCounterAtY
.noSkippedFrames:
	endif
endm