; Example of installing new interrupt handler in protected mode
; This example will work with both the older NEAR and the new FLAT models
;
	.386p

	include cw.inc

b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr

;
;The segment name _NEAR isn't important here, it's the class 'near' that is.
;
_NEAR	segment para public 'near' use32
	assume cs:_NEAR, ds:_NEAR

;-----------------------------------------------------------------------------
;
;On entry DS=ES=PSP as normal, SS=_NEAR
;
start	proc	far
	mov	ax,_NEAR		;should yield a data selector
	mov	ds,ax		;with a limit of 4G. SS already
	mov	es,ax		;has this value so that could be
	mov	fs,ax		;used but this method shows that
	mov	gs,ax		;segments can still be used.
;
;CS=DS=ES=FS=GS=_NEAR  from this point on unless we're in interrupt code in which
;case we need to reload DS,ES,FS,GS. SS will be using a system stack for
;hardware interrupts or CallBack's but will otherwise already = _NEAR.
;

;
;Want to play with IRQ 0 so we need to lock the new handler's memory.
;
	mov	esi,offset Int08Handler
	mov	ebx,offset Int08StuffEnd-Int08Handler
	sys	LockMemNear
	mov	edx,offset message1
	jc	@@9
;
;Get old vector address.
;
	mov	bl,8
	sys	GetVect
	mov	d[oldint08],edx
	mov	w[oldint08+4],cx
;
;Set new handler address.
;
	mov	edx,offset Int08Handler
	mov	cx,cs
	sys	SetVect
;
;shell to DOS so you can see hardware interrupts being resignaled from real
;mode to protected mode.
;
	mov	edx,offset aname
	mov	ebx,offset ptab
	mov	ax,4b00h
	int	21h
;
;Restore the old handler. Not strictly necesary but it's good practice.
;
	mov	edx,d[oldint08]
	mov	cx,w[oldint08+4]
	mov	bl,8
	sys	SetVect
;
;Print something to say we're back.
;
	mov	edx,offset message2
@@9:	mov	ah,9
	int	21h
;
;Back to good old DOS.
;
	mov	ax,4c00h
	int	21h
start	endp


;-----------------------------------------------------------------------------
Int08Handler	proc	far
	pushad
	push	ds
	mov	ax,_NEAR
	mov	ds,ax
;
;Set new border color.
;
	mov	ah,color
	inc	color
	and	color,63
	mov	dx,3dah
	in	al,dx
	mov	dl,0c0h
	mov	al,11h
	out	dx,al
	mov	al,ah
	out	dx,al
	mov	al,20h
	out	dx,al
;
;chain to old handler.
;
	pop	ds
	popad
	assume ds:nothing
	jmp	oldint08
	assume ds:_NEAR
;
color	db ?
oldint08	df ?
Int08Handler	endp
Int08stuffEnd	label byte


;-----------------------------------------------------------------------------
message1	db 'Not enough physical memory to lock IRQ 0 handler.',13,10,'$'
message2	db 'Back to the real world.',13,10,'$'
;
aname	db 'c:\command.com',0	; change 'c' to drive containing COMMAND.COM
ptab	dw 0
	dd ctab
	dw _NEAR
	dd 0
	dw 0
	dd 0
	dw 0
ctab	db 0,13
;
_NEAR	ends
	end	start
