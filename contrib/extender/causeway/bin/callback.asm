;
;A demo of using FarCallReal to pass control from protected mode to real mode
;and then use a Call-Back to pass control from real mode to protected mode.
;
	.386			;This order of .386 and .model
	.model small		;ensures use32 segments.
	.stack 1024		;should be enough for most.

;	option oldstructs		;MASM 6.1 needs this.

	include cw.inc		;define CauseWay API

	.code

;-------------------------------------------------------------------------------
Start	proc	near
	mov	ax,DGROUP		;Make our data addressable.
	mov	ds,ax
;
;Get a Call-Back so that we can pass control from real mode to protected mode.
;
	push	ds
	mov	ax,ds		;Point to the real mode register
	mov	es,ax		;structure.
	mov	edi,offset Real2ProtectedRegs
	mov	ax,cs		;Point to the protected mode
	mov	ds,ax		;recipient of the Call-Back.
	mov	esi,offset Real2ProtectedEntry
	sys	GetCallBack		;allocate a Call-Back.
	pop	ds
	jc	callback_error
;
;Put the real mode address in a useful place.
;
	push	ds
	mov	ax,_REAL
	mov	ds,ax
	assume ds:_REAL
	mov	word ptr [Real2Protected],dx
	mov	word ptr [Real2Protected+2],cx
	assume ds:DGROUP
	pop	ds
;
;We need some DOS memory to copy the real mode code into.
;
	mov	bx,((RealEnd-RealStart)/16)+1
	sys	GetMemDOS
	jc	memory_error
	mov	RealSegment,ax
	mov	RealSelector,dx
;
;Copy our real mode code into the DOS memory allocated.
;
	push	ds
	mov	es,dx
	mov	edi,0
	mov	esi,edi
	mov	ax,_REAL
	mov	ds,ax
	mov	ecx,RealEnd-RealStart
	rep	movsb
	pop	ds
;
;Pass control to the real mode code.
;
	push	ds
	pop	es
	mov	edi,offset Protected2RealRegs
	mov	ax,RealSegment
	mov	Real_CS[edi],ax
	mov	ax,offset RealModeEntry
	mov	Real_IP[edi],ax
	sys	FarCallReal
;
;Now we've proved our point we can exit.
;
	jmp	exit
;
;Wasn't enough DOS memory so print a message and exit.
;
memory_error:	mov	edx,offset MemoryMessage
	jmp	error_print
;
;Wasn't a free Call-Back so print a message and exit.
;
callback_error: mov	edx,offset CallBackMessage
	jmp	error_print
;
;Print a message.
;
error_print:	mov	ah,9
	int	21h
;
;Back to the real world.
;
exit:	mov	ax,4c00h
	int	21h
Start	endp

;-------------------------------------------------------------------------------
;
;This gets control from real mode so do something to let the world know it
;happened.
;
Real2ProtectedEntry proc far
;
;Need to adjust the real mode CS:IP and SP first.
;
	mov	ax,[esi]		;get stacked offset.
	mov	es:Real_IP[edi],ax
	mov	ax,2[esi]
	mov	es:Real_CS[edi],ax
	add	es:Real_SP[edi],4
;
;Now save all the registers ready for exit.
;
	pushad
	push	ds
	push	es
	push	fs
	push	gs
;
;The real mode code could have passed values to this code via the registers. A
;copy of the real mode registers is in the struc pointed to by ES:EDI. When we
;exit the real mode registers will be reloaded from this struc which allows
;values to be passed back as well.
;
	mov	ax,DGROUP		;Make our data addressable.
	mov	ds,ax
;
;Print a message to let us know we got here.
;
	mov	ah,9
	mov	edx,offset ProtModeMessage
	int	21h
;
;Retrieve the registers.
;
	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad
;
;Return to the real mode caller.
;
	iretd
Real2ProtectedEntry endp


	.data
;-------------------------------------------------------------------------------
RealSegment	dw ?
RealSelector	dw ?
Real2ProtectedRegs db size RealRegsStruc dup (?)
Protected2RealRegs db size RealRegsStruc dup (?)
MemoryMessage	db "Not enough DOS memory to run.",13,10,"$"
CallBackMessage db "No free Call-Back.",13,10,"$"
ProtModeMessage db "Hello world from protected mode via a Call-Back.",13,10,"$"

;-------------------------------------------------------------------------------
;
;This segment gets copied into conventional DOS memory.
;
_REAL	segment para public 'data' use16
	assume cs:_REAL, ds:_REAL
RealStart	equ	$
;
;This needs to be far so we can use FarCallReal to address it.
;
RealModeEntry	proc	far
	push	cs		;Make our data addressable. This
	pop	ds		;is valid in real mode.
	mov	dx,offset RealModeMessage
	mov	ah,9
	int	21h
	call	dword ptr [Real2Protected] ;Call protected mode code.
	retf
Real2Protected	dd ?
RealModeMessage db "Hello world from real mode",13,10,"$"
RealModeEntry	endp
;
RealEnd	equ	$
_REAL	ends

	end	Start
