	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Flip a bitmap from left to right, eg, around Y axis, but affecting it in the X
;
;On Entry:
;
;ESI	- bitmap
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
BitmapXFlip	proc	near
	public BitmapXFlip
	pushad
	mov	eax,BM_Wide[esi]
	dec	eax
	mul	BM_PWide[esi]
	mov	ebp,eax
	add	ebp,BM_PWide[esi]	;line width.
	add	eax,size BM
	add	eax,esi
	mov	edi,eax		;point to end of first line.
	mov	ecx,BM_Wide[esi]
	shr	ecx,1		;width to process.
	mov	edx,BM_Depth[esi]
	mov	ebx,BM_PWide[esi]
	add	esi,size BM		;point to start of first line.
@@0:	pushm	ecx,esi,edi
@@1:	cmp	ebx,3
	jz	@@24bit
	cmp	ebx,2
	jz	@@16bit
@@8bit:	mov	al,[esi]
	xchg	al,[edi]
	mov	[esi],al
	jmp	@@2
@@16bit:	mov	ax,[esi]
	xchg	ax,[edi]
	mov	[esi],ax
	jmp	@@2
@@24bit:	mov	ax,[esi+0]
	xchg	ax,[edi+0]
	mov	[esi+0],ax
	mov	al,[esi+2]
	xchg	al,[edi+2]
	mov	[esi+2],al
@@2:	add	esi,ebx
	sub	edi,ebx
	dec	ecx
	jnz	@@1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,ebp
	dec	edx
	jnz	@@0
	popad
	ret
BitmapXFlip	endp


	efile
	end



