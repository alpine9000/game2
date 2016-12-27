	include "includes.i"

	xdef _InitInterrupts
	
_InitInterrupts:
	movem.l	a0/a3,-(sp)
	lea	Level3InterruptHandler,a3
	move.l	vectorBase,a0
 	move.l	a3,LVL3_INT_VECTOR(a0)
	movem.l	(sp)+,a0/a3
	rts

Level3InterruptHandler:
	movem.l	d0-a6,-(sp)
	lea	CUSTOM,a6
.checkVerticalBlank:
	move.w	INTREQR(a6),d0
	and.w	#INTF_VERTB,d0	
	beq	.checkCopper

.verticalBlank:
	move.w	#INTF_VERTB,INTREQ(a6)	; clear interrupt bit	
	add.l	#1,_verticalBlankCount
	;; MusicPlay
.checkCopper:
	move.w	INTREQR(a6),d0
	and.w	#INTF_COPER,d0	
	beq.s	.interruptComplete
.copperInterrupt:
	move.w	#INTF_COPER,INTREQ(a6)	; clear interrupt bit	
	
.interruptComplete:
	movem.l	(sp)+,d0-a6
	rte
