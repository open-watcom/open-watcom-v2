; INTASM.ASM
; Demonstrates calling interrupt not natively supported by CauseWay
;  that uses segment:offset pointers.
; Assembly language version, C version is INTC.C

.MODEL MEDIUM
.STACK 400H

.CONST
FileName	DB	'INTASM.EXE',0

.DATA

ASCIIZSegment	DW	0
ASCIIZSelector	DW	0
CanonicalSegment	DW	0
CanonicalSelector	DW	0

RealRegsStruc STRUC
	Real_EDI	DD	?	;EDI
	Real_ESI	DD	?	;ESI
	Real_EBP	DD	?	;EBP
				DD	?	;Reserved.
	Real_EBX	DD	?	;EBX
	Real_EDX	DD	?	;EDX
	Real_ECX	DD	?	;ECX
	Real_EAX	DD	?	;EAX
	Real_Flags	DW	?	;FLAGS
	Real_ES		DW	?	;ES
	Real_DS		DW	?	;DS
	Real_FS		DW	?	;FS
	Real_GS		DW	?	;GS
	Real_IP		DW	?	;IP
	Real_CS		DW	?	;CS
	Real_SP		DW	?	;SP
	Real_SS		DW	?	;SS
RealRegsStruc ENDS

RealRegs	RealRegsStruc	<>

.CODE
.386

start:
	mov	ax,DGROUP
	mov	ds,ax

; allocate low dos memory for the two buffers
	mov	bx,8		; # paras to allocate
	mov	ax,0ff21h	; GetMemDOS
	int	31h
	jc	errout
	mov	ASCIIZSelector,dx
	mov	ASCIIZSegment,ax

	mov	bx,8		; # paras to allocate
	mov	ax,0ff21h	; GetMemDOS
	int	31h
	jc	errout
	mov	CanonicalSelector,dx
	mov	CanonicalSegment,ax

; copy ASCIIZ file name into buffer
;  use INTASM.EXE for example
	mov	es,ASCIIZSelector
	mov	si,OFFSET DGROUP:FileName
	xor	di,di

looper:
	movsb
	cmp	BYTE PTR ds:[si-1],0
	jne	looper

; setup register values to pass to real mode
	mov	RealRegs.Real_ESI,0	; zero offset on DOS memory allocations
	mov	RealRegs.Real_EDI,0
	mov	ax,ASCIIZSegment	; setup segments with real mode values
	mov	RealRegs.Real_DS,ax
	mov	ax,CanonicalSegment
	mov	RealRegs.Real_ES,ax
	mov	ah,60h				; truename function
	mov	WORD PTR RealRegs.Real_EAX,ax

; perform real mode interrupt call
	push	ds
	pop	es
	mov	bl,21h
	mov	di,OFFSET DGROUP:RealRegs
	mov	ax,0ff01h		; IntXX
	int	31h

	test	RealRegs.Real_Flags,1	; check carry flag error status
	jne	errout

; display truename
	push	ds
	mov	ds,CanonicalSelector
	xor	edx,edx

; mindless one char at a time display loop of ASCIIZ string
display:
	cmp	BYTE PTR [edx],0
	je	done
	mov	cx,1
	mov	bx,1			; stdout
	mov	ah,40h
	int	21h
	inc	edx
	jmp	display

done:
	pop	ds

	mov	ax,4c00h
	int	21h

; error allocating memory or in real mode interrupt
errout:
	mov	ax,4c01h
	int	21h

END	start
