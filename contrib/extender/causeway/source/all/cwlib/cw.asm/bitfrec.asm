	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Fill a rectangle of a bitmap.
;
;On Entry:
;
;flags	- Flags.
;fillvalue	- Fill value.
;xcoord	- X co-ord.
;ycoord	- Y co-ord.
;wide	- width.
;depth	- depth.
;bitmap	- bitmap.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
BitmapFillRectangle:
	public BitmapFillRectangle
_BitmapFillRectangle proc syscall @@flags:dword, @@fillvalue:dword, @@xcoord:dword, \
	@@ycoord:dword, @@wide:dword, @@depth:dword, @@bitmap
	pushad
;
;Convert fill value to suit mode.
;
	mov	eax,VideoModeFlags
	and	eax,15
	jnz	@@rgbs
	test	@@flags,1		;24-bit value?
	jz	@@GetAddr
;
;Need to convert RGB value to nearest hardware pen number.
;
	mov	eax,@@FillValue
	call	SearchRGB		;Find nearest value.
	mov	@@FillValue,eax
	jmp	@@GetAddr
;
;Check if FillValue is a pen number or RGB.
;
@@rgbs:	test	@@flags,1
	jnz	@@GotRGB
;
;Convert pen number into RGB values.
;
	movzx	eax,b[@@fillvalue]
	mov	eax,d[HardwarePalette+eax+eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	@@fillvalue,eax
;
;Convert RGB to suit mode.
;
@@GotRGB:	mov	eax,VideoModeFlags
	and	eax,15
	dec	eax
	jz	@@16m_2_32k
	dec	eax
	jz	@@16m_2_64k
	jmp	@@GetAddr
;
;Convert RGB to 15 bit.
;
@@16m_2_32k:	mov	ebx,@@fillvalue
	shl	ebx,8
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	mov	@@fillvalue,eax
	jmp	@@GetAddr
;
;Convert RGB to 16 bit.
;
@@16m_2_64k:	mov	ebx,@@fillvalue
	shl	ebx,8
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	mov	@@fillvalue,eax
;
;Calculate start address.
;
@@GetAddr:	mov	esi,@@bitmap
	cmp	@@wide,-1
	jnz	@@8
	mov	eax,BM_Wide[esi]
	mov	@@wide,eax
@@8:	cmp	@@depth,-1
	jnz	@@9
	mov	eax,BM_Depth[esi]
	mov	@@depth,eax
;
;Check for daft width/depth.
;
@@9:	cmp	@@wide,0
	js	@@FillFinished
	jz	@@FillFinished
	cmp	@@depth,0
	js	@@FillFinished
	jz	@@FillFinished
	;
	mov	edi,@@bitmap
	cmp	@@xcoord,0
	jns	@@4
	mov	eax,@@xcoord
	neg	eax
	sub	@@wide,eax
	js	@@FillFinished
	jz	@@FillFinished
	mov	@@xcoord,0
;
@@4:	cmp	@@ycoord,0
	jns	@@5
	mov	eax,@@ycoord
	neg	eax
	sub	@@depth,eax
	js	@@FillFinished
	jz	@@FillFinished
	mov	@@ycoord,0
;
@@5:	mov	eax,@@xcoord
	add	eax,@@wide
	cmp	eax,BM_Wide[edi]
	jl	@@6
	sub	eax,BM_Wide[edi]
	sub	@@wide,eax
	js	@@FillFinished
	jz	@@FillFinished
;
@@6:	mov	eax,@@ycoord
	add	eax,@@depth
	cmp	eax,BM_Depth[edi]
	jl	@@7
	sub	eax,BM_Depth[edi]
	sub	@@depth,eax
	js	@@FillFinished
	jz	@@FillFinished
;
@@7:	mov	eax,BM_PWide[edi]
	mul	BM_Wide[edi]
	mov	esi,eax
	mul	@@ycoord
	mov	ebx,eax
	mov	eax,@@xcoord
	mul	BM_PWide[edi]
	add	ebx,eax
	mov	eax,BM_Flags[edi]
	add	edi,ebx
	add	edi,size BM
	mov	ebx,@@fillvalue
	mov	ecx,@@wide
	mov	edx,@@depth
;
	and	eax,15
	jz	@@bit8
	dec	eax
	jz	@@bit16
	dec	eax
	jz	@@bit16
	dec	eax
	jz	@@bit24
	jmp	@@FillFinished
;
@@bit8:	pushm	ecx,edi
	mov	eax,ebx
	rep_stosb
	popm	ecx,edi
	add	edi,esi
	dec	edx
	jnz	@@bit8
	jmp	@@fillfinished
;
@@bit16:	pushm	ecx,edi
	mov	eax,ebx
	rep_stosw
	popm	ecx,edi
	add	edi,esi
	dec	edx
	jnz	@@bit16
	jmp	@@fillfinished
;
@@bit24:	pushm	ecx,edi
	mov	eax,ebx
	shr	eax,16
@@bit24_0:	mov	[edi],bx
	mov	[edi+2],al
	add	edi,3
	dec	ecx
	jnz	@@bit24_0
	popm	ecx,edi
	add	edi,esi
	dec	edx
	jnz	@@bit24
;
@@fillfinished: popad
	ret
_BitmapFillRectangle endp


	efile
	end


