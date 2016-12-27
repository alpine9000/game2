	if P61_MUSIC=1
	include "includes.i"
	
	xdef 	_MusicStart
	xdef	_MusicPlay
	if TRACKLOADER=0
	xdef	_MusicStop
	endif
	
MusicProcessFade:
	move.l	d0,-(sp)
	cmp.w   #0,musicFade
        beq     .dontFadeOutMusic
        move.w  musicFade,d0
        add.w   d0,P61_Master
        cmp.w   #0,P61_Master
        beq     .fadeComplete
        cmp.w   #64,P61_Master
        beq     .fadeComplete
        bra     .dontFadeOutMusic
.fadeComplete:
        move.w  #0,musicFade
.dontFadeOutMusic:
	move.l	(sp)+,d0
	rts

_MusicStart:
	;; d0.w	- index of module to play
.wait: 				; In case there is currently  fade in progress
	jsr	WaitVerticalBlank
	bsr	MusicProcessFade
	cmp.w	#0,P61_Master
	beq	.skip
	cmp.w	#64,P61_Master
	blt	.wait

	cmp.w	currentModule,d0
	beq	.skip
	cmp.w	#-1,currentModule
	beq	.fadeComplete

.fadeOutMusic:
	cmp.w	#0,P61_Master
	beq	.fadeComplete
	sub.w	#1,P61_Master
	jsr	WaitVerticalBlank
	bra	.fadeOutMusic
.fadeComplete:

	move.w	d0,currentModule
	movem.l	d0-a6,-(sp)
	cmp.w	#0,p61Inited
	beq	.skipEnd
	movem.l	d0-a6,-(sp)
	IntsOff
	jsr	P61_End
	IntsOn	
	movem.l	(sp)+,d0-a6	
	move.w	#0,p61Inited	
.skipEnd:
	lea	module,a0
	lea	modules,a1
	lsl.w	#3,d0
	adda.w	d0,a1
	move.l	(a1)+,d0
	move.l	(a1),a1
	jsr	DiskLoadData
	lea     module,a0
        sub.l   a1,a1
        sub.l   a2,a2
        moveq   #0,d0
	move.w	#64,P61_Master
	jsr     P61_Init
	move.w	#1,p61Inited
	movem.l	(sp)+,d0-a6
.skip:
	rts

_MusicStop:
	movem.l	d0-a6,-(sp)	
	jsr	WaitVerticalBlank	
	IntsOff
	jsr	WaitVerticalBlank
	jsr	P61_End
	movem.l	(sp)+,d0-a6
	rts

_MusicPlay:
	cmp.w	#0,P61_Master
	bne	.playMusic
	move.w  #0,AUD0VOL(a6)
	move.w  #0,AUD1VOL(a6)
	move.w  #0,AUD2VOL(a6)
	bra	.continue
.playMusic:
	jsr	P61_Music
.continue:
	rts
	
currentModule:
	dc.w	-1

modules:
	dc.l	enddiskmoduleA-diskmoduleA
	dc.l	diskmoduleA

musicFade:	; 0 - no fade, 1 - fade out 2, fade in
	dc.w    0
p61Inited:
	dc.w	0

	section	.bss
module:	
	ds.b	MAX_P61_SIZE
	ds.b	512

	section	.noload

moduleDiskData:
	P61Module A,"assets/P61.jmd-songC"
	endif