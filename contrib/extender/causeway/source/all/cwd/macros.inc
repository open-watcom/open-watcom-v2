b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr
f	equ	fword ptr

pushm	MACRO	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
	IRP	X,<R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16> 	;REPEAT FOR EACH PARM
	IFNB	<X>			;IF THIS PARM NOT BLANK
	PUSH	X			;SAVE THE REGISTER
	ENDIF				;END IFNB
	ENDM				;END IRP
	ENDM

popm	macro	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
	IRP	X,<R16,R15,R14,R13,R12,R11,R10,R9,R8,R7,R6,R5,R4,R3,R2,R1> ;REPEAT FOR EACH PARM
	IFNB	<X>				;IF THIS PARM NOT BLANK
	POP	X				;POP THE REGISTER
	ENDIF					;END IFNB
	ENDM					;END IRP
	ENDM

rep_movsb	macro
	push	ecx
	shr	ecx,2
	rep	movsd
	pop	ecx
	and	ecx,3
	rep	movsb
	endm

rep_movsw	macro
	push	ecx
	shr	ecx,1
	rep	movsd
	pop	ecx
	and	ecx,1
	rep	movsw
	endm

rep_stosb	macro
	push	eax
	mov	ah,al
	push	ax
	shl	eax,16
	pop	ax
	push	ecx
	shr	ecx,2
	rep	stosd
	pop	ecx
	and	ecx,3
	rep	stosb
	pop	eax
	endm

rep_stosw	macro
	push	eax
	push	ax
	shl	eax,16
	pop	ax
	push	ecx
	shr	ecx,1
	rep	stosd
	pop	ecx
	and	ecx,1
	rep	stosw
	pop	eax
	endm

