DDS	struc		;Structure to get at Low/high words.
lw	dw ?
hw	dw ?
DDS	ends


GetMemory	macro p1,p2,p3
	mov	bx,p1
	mov	ax,4800h
	int	21h
	jc	p3
	push	si
	mov	p2,ax
	mov	si,MemoryListPos
	mov	[si],ax
	add	MemoryListPos,2
	pop	si
	endm

LoseMemory	macro
	local __reloop1
	local __reloop2
	lea	si,MemoryList
__reloop1:	mov	ax,[si]
	cmp	ax,-1
	jz	__reloop2
	mov	es,[si]
	push	si
	mov	ax,4900h
	int	21h
	pop	si
	add	si,2
	jmp	__reloop1
__reloop2:	nop
	endm

SetVector	macro p1,p2,p3
	if	0
	cli
	push	es
	mov	si,VectorListPos
	add	VectorListPos,5
	mov	b[si],p1
	xor	ax,ax
	mov	es,ax
	mov	ax,es:[p1*4]
	mov	[si+1],ax
	mov	p2,ax
	mov	ax,es:[2+(p1*4)]
	mov	[si+3],ax
	mov	p2+2,ax
	lea	ax,p3
	mov	es:[p1*4],ax
	push	cs
	pop	ax
	mov	es:[2+(p1*4)],ax
	pop	es
	sti
	endif

	;p1 - vector number.
	;p2 - storage
	;p3 - replacement

	mov	al,p1
	mov	ah,35h
	int	21h
	mov	p2,bx
	mov	p2+2,es
	mov	si,VectorListPos
	add	VectorListPos,5
	mov	b[si],p1
	mov	1[si],bx
	mov	3[si],es
	mov	al,p1
	mov	ah,25h
	lea	dx,p3
	pushm	ds,cs
	pop	ds
	int	21h
	pop	ds
	endm


LoseVectors	macro
	local __reloop1
	local __reloop2

	if	0
	cli
	push	es
	lea	si,VectorList
__reloop1:	mov	ax,[si+3]
	cmp	ax,-1
	jz	__reloop2
	mov	al,[si]
	xor	ah,ah
	mov	di,ax
	add	di,di
	add	di,di
	xor	ax,ax
	mov	es,ax
	mov	ax,[si+1]
	mov	es:[di],ax
	mov	ax,[si+3]
	mov	es:[di+2],ax
	add	si,5
	jmp	__reloop1
__reloop2:	pop	es
	sti
	endif

	lea	si,VectorList
__reloop1:	mov	ax,[si+3]
	cmp	ax,-1
	jz	__reloop2
	pushm	si,ds
	mov	al,[si]
	mov	ah,25h
	mov	dx,1[si]
	mov	ds,3[si]
	int	21h
	popm	si,ds
	add	si,5
	jmp	__reloop1
__reloop2:
	endm


KeyCheck	macro reg001,reg002
	local	__doneit
	mov	bx,reg001
	cmp	ah,bh
	jnz	__doneit
	cmp	bl,255
	jz	__doneit
	cmp	al,bl
__doneit:	jz	reg002
	endm


Pushm	MACRO	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10
	IRP	X,<R1,R2,R3,R4,R5,R6,R7,R8,R9,R10> 	;REPEAT FOR EACH PARM
	IFNB	<X>			;IF THIS PARM NOT BLANK
	PUSH	X			;SAVE THE REGISTER
	ENDIF				;END IFNB
	ENDM				;END IRP
	ENDM

Popm	macro	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10
	IRP	X,<R10,R9,R8,R7,R6,R5,R4,R3,R2,R1> ;REPEAT FOR EACH PARM
	IFNB	<X>				;IF THIS PARM NOT BLANK
	POP	X				;POP THE REGISTER
	ENDIF					;END IFNB
	ENDM					;END IRP
	ENDM


CLR	MACRO	R1,R2,R3,R4,R5,R6,R7,R8
	IRP	RX,<R1,R2,R3,R4,R5,R6,R7,R8>	;REGISTER LIST
	IFNB	<RX>				;FOR EACH REGISTER IN LIST
	XOR	RX,RX				;CLEAR THE REGISTER
	ENDIF					;END OF IFIDN
	ENDM					;END OF IRP
	ENDM					;MACRO END


SetDS	macro stds001
	mov	ax,stds001
	mov	ds,ax
	endm


