	.model tiny
	option oldstructs
	option proc:private

CWCStackSize	equ	1024	; cannot exceed EXECopyStubLen size (1135 bytes)

decode_c_struc struc
DecC_ID	db "CWC"
DecC_Bits	db ?
DecC_Len	dw ?,?
DecC_Size	dw ?,?
decode_c_struc	ends

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

b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr
f	equ	fword ptr


RepMinSize	equ	2

var_struc	struc
;	db 100h-10h dup (0)
	db CWCStackSize-10h dup (0)

var_SourceSeg	dw ?
var_CopyLen	dw ?
var_EntryIP	dw 0
var_EntryCS	dw ?
var_ImageLen	dw ?,?
var_EntryES	dw ?
var_struc	ends

	.code

start	proc	near
;
;Decompress the EXE and it's header/relocation entries.
;
	cld
	pushm	di,es
	call	Decode
	popm	di,es
	;
	;Get a pointer to the EXE header.
	;
	mov	ax,ss:w[var_ImageLen]
	mov	bx,ss:w[var_ImageLen+2]
	shr	bx,1
	rcr	ax,1
	shr	bx,1
	rcr	ax,1
	shr	bx,1
	rcr	ax,1
	shr	bx,1
	rcr	ax,1
	mov	dx,es
	add	dx,ax
	mov	ds,dx
	mov	si,ss:w[var_ImageLen]
	and	si,15
;
;Process the relocation entries.
;
	pushm	ds,si
	mov	cx,[si+6]		;get number of relocation entries.
	add	si,1bh
	;
@@0:	or	cx,cx
	jz	@@1
	pushm	si,ds,es
	mov	ax,[si+2]		;/
	mov	si,[si]		;Get the offset read.
	mov	bx,es
	add	ax,bx		;/
	mov	es,ax		;/
	add	es:[si],bx		;"Relocate" the value...
	popm	si,ds,es
	dec	cx
	add	si,4
	jnz	@@0
	mov	ax,ds
	add	ax,1000h
	mov	ds,ax
	jmp	@@0
	;
@@1:	popm	ds,si
;
;Get entry register values setup.
;
	mov	bx,es
	mov	ax,[si+0eh]
	add	ax,bx
	mov	dx,ax
	mov	bp,[si+10h]
	mov	ax,[si+16h]
	add	ax,bx
	mov	cs:EntryCS,ax
	mov	ax,[si+14h]
	mov	cs:EntryIP,ax
	mov	es,ss:w[var_EntryES]
	mov	ds,ss:w[var_EntryES]
	mov	ss,dx
	mov	sp,bp
;
;Pass control to the real program.
;
	jmp	cs:dword ptr [EntryIP]
Start	endp


Decode	proc	near
;
;Get next input bit.
;
_DCD_ReadBit	macro
	local __0
	adc	bp,bp
;	adc	bx,bx
	dec	dl
	jnz	__0
	mov	bp,[si]
;	mov	bx,[si+2]
	inc	si
	inc	si
;	lea	si,4[si]
	mov	dl,dh
__0:	;
	endm
	;
	;Check for main ID string.
	;
	xor	ch,ch
	mov	cl,b[si+DecC_Bits]
	mov	ax,1
	shl	ax,cl
	dec	ax
	mov	cs:w[@@Masker+2],ax
	sub	cl,8
	mov	cs:b[@@Shifter+1],cl
	add	si,size decode_c_struc
;
;Get on with decodeing the data.
;
	mov	bp,[si]
;	mov	bx,[si+2]
	add	si,2
	mov	dl,16
	mov	dh,dl
;
;The main decompresion loop.
;
@@0:	_DCD_ReadBit
	jnc	@@1
	;
	;Read a raw byte.
	;
	mov	al,[si]
	mov	es:[di],al
	inc	si
	inc	di
	jmp	@@0
	;
@@1:	_DCD_ReadBit
	jnc	@@2
	;
	;Do a rep with 8 bit position, 2 bit length.
	;
	xor	cx,cx
	_DCD_ReadBit
	adc	cl,cl
	_DCD_ReadBit
	adc	cl,cl
	add	cl,2
	xor	ax,ax
	mov	al,[si]
	inc	si
	dec	ax
	js	@@1_0
	;
@@1_1:	;do a rep.
	;
	add	ax,cx
	pushm	si,ds,es
	pop	ds
	mov	si,di
	sub	si,ax
	rep	movsb
	popm	si,ds
	jmp	@@0
	;
	;Do a run.
	;
@@1_0:	mov	al,[si]
	inc	si
	inc	cx
	rep	stosb
	jmp	@@0
	;
@@2:	_DCD_ReadBit
	jnc	@@3
	;
	;Do a rep with 12 bit position, 4 bit length.
	;
	xor	ax,ax
	mov	ax,[si]
	add	si,2
	mov	ch,al
	mov	cl,4
	shr	ax,cl
	mov	cl,ch
	and	cx,15
	add	cl,2
	dec	ax
	jns	@@1_1
	jmp	@@1_0
	;
	;Do a rep with 12-bit position, 12-bit length.
	;
@@3:	mov	ax,[si+1]
	mov	ch,al
@@Shifter:	mov	cl,12-8
	shr	ax,cl
	mov	cl,[si]
	add	si,3
@@Masker:	and	cx,4095
	add	cx,2
	dec	ax
	jns	@@1_1
	;
	;Check for special codes of 0-15 (Would use 2 or 4 bit if really needed)
	;
	cmp	cx,RepMinSize+15+1
	jnc	@@1_0
	cmp	cl,RepMinSize+2	;Rationalise destination?
	jz	@@3_3
	cmp	cl,RepMinSize+1	;Rationalise source?
	jz	@@3_2
	cmp	cl,RepMinSize+3
	jz	@@3_1
	cmp	cl,RepMinSize	;Terminator?
	jz	@@4
	jmp	@@4
	;
	;Copy literal string.
	;
@@3_1:	xor	ch,ch
	mov	cl,[si]
	inc	si
	rep	movsb
	jmp	@@0
	;
	;rationalise DS:SI
	;
@@3_2:	cmp	si,49152
	jc	@@0
	mov	ax,si
	shr	ax,1
	shr	ax,1
	shr	ax,1
	shr	ax,1
	push	bx
	mov	bx,ds
	add	ax,bx
	mov	ds,ax
	and	si,15
	pop	bx
	jmp	@@0
	;
	;rationalise ES:DI
	;
@@3_3:	cmp	di,49152
	jc	@@0
	mov	ax,di
	sub	ax,49152
	pushm	ax,bx
	shr	ax,1
	shr	ax,1
	shr	ax,1
	shr	ax,1
	mov	bx,es
	add	ax,bx
	mov	es,ax
	popm	ax,bx
	sub	di,ax
	and	ax,15
	add	di,ax
	jmp	@@0
	;
	;We're all done so exit.
	;
@@4:	ret
Decode	endp

EntryIP	dw ?
EntryCS	dw ?

	end	start


