; Example of using NEAR model
; This example will also work using FLAT model (WL32 /f option)
;
	.386p

	include cw.inc

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
;CS=DS=ES=FS=GS=_NEAR from this point on unless we're in interrupt code in which
;case we need to reload DS,ES,FS,GS. SS will be using a system stack for
;hardware interrupts or CallBack's but will otherwise already = _NEAR.
;
	mov	edx,offset message1	;need to do something to show
	mov	ah,9		;there is life after WL32.
	int	21h
;
;Patch an interrupt vector for the sake of it.
;
	mov	bl,60h
	sys	GetVect		;Get current vector contents.
	push	edx
	push	ecx
	mov	cx,cs
	mov	edx,offset IntHandler
	sys	SetVect		;Set new handler address.
;
;Allocate some memory for a text screen buffer.
;
	mov	ebx,80*2*25
	sys	GetMemNear
	jc	@@Error
	mov	edi,esi
	mov	edx,esi
;
;Get a near address for color text screen memory.
;
	mov	esi,0b8000h		;normal linear address.
	sys	Linear2Near
	mov	ebx,esi		;near address.
;
;Copy current screen contents.
;
	mov	ecx,(80*2*25)/4
	rep	movsd
;
;Fill the screen with rubbish.
;
	mov	edi,ebx
	mov	eax,03400340h
	mov	ecx,(80*2*25)/4
	rep	stosd
;
;Pass control to the interrupt handler.
;
	int	60h
;
;Restore the screen.
;
	mov	edi,ebx
	mov	esi,edx
	mov	ecx,(80*2*25)/4
	rep	movsd
;
;Release screen buffer memory.
;
	mov	esi,edx
	sys	RelMemNear
;
;Restore original interrupt vector.
;
	pop	ecx
	pop	edx
	mov	bl,60h
	sys	SetVect
;
;Demonstration over.
;
@@Error:	mov	ax,4c00h
	int	21h
start	endp


;-----------------------------------------------------------------------------
IntHandler	proc	far
;
;Save all the registers first.
;
	push	ds
	push	es
	push	fs
	push	gs
	pushad
;
;Now make all our data addressable.
;
	mov	ax,_NEAR
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
;
;Print a message again.
;
	mov	edx,offset message2
	mov	ah,9
	int	21h
;
;Wait for the key press.
;
	mov	ah,0
	int	16h
;
;Restore the registers.
;
	popad
	pop	gs
	pop	fs
	pop	es
	pop	ds
	iretd
IntHandler	endp


;-----------------------------------------------------------------------------
message1	db 'hello world',13,10,'$'
message2	db 13,10,'press any key to continue',13,10,13,10,'$'

_NEAR	ends

	end	start
