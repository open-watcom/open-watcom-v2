	include ..\cwlib.inc


TGA		struc
TGA_IdentSize		db ?	;Wasted space after this header.
TGA_ColourMapType	db ?	;Type of colour map if any.
			;0-mono or RGB data (no palette)
			;1-Palette is present.
TGA_ImageType		db ?	;Type of image data.
			;1-Uncompressed palette-driven image.
			;2-Uncompressed RGB image.
			;3-Uncompressed monochrome image.
			;9-Run-length encoded palette-driven image.
			;10-Run length encoded RGB image
			;11-Run length encoded monochrome image.
TGA_ColourMapStart	dw ?	;Starting colour defined.
TGA_ColourMapLength	dw ?	;Number of colours.defined in colour map.
TGA_ColourMapBits	db ?	;Bits for each colour map entry?
TGA_XStart		dw ?	;X display offset.
TGA_YStart		dw ?	;Y display offset.
TGA_Wide		dw ?	;Image width
TGA_Depth		dw ?	;Image depth.
TGA_Bits		db ?	;Bits of colour used (1,8,16,24,32)
TGA_Descriptor		db ?	;Flag bits.
			;0-\
			;1---Number of bits available for overlays.
			;2-/
			;3-/
			;4-Left/Right flip.
			;5-Top/Bottom flip.
			;6-
			;7-
TGA		ends


	scode


;------------------------------------------------------------------------------
;
;Load a TGA file.
;
;On Entry:-
;
;EDX	- File name.
;
;On Exit:-
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- Address of bitmap loaded.
;
;ALL other registers preserved.
;
LoadTGA	proc	near
	local @@handle:dword, @@bitmap:dword, @@body:dword
	pushad
	mov	@@handle,0
	mov	@@bitmap,0
	mov	@@body,0
	;
	;Open the file.
	;
	call	OpenFile
	jc	@@file_error
	mov	@@handle,ebx
	;
	;Read main header.
	;
	mov	edx,offset TG_Header
	mov	ecx,size TGA
	call	ReadFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
	;Work out start of real data.
	;
	mov	esi,offset TG_Header	;work out offset to image data.
	movzx	ecx,TGA_IDentSize[esi]
	add	ecx,size TGA
	mov	al,0
	call	SetFilePointer
	;
	;Check data is in a format we can use.
	;
	mov	esi,offset TG_Header
	cmp	TGA_ImageType[esi],1	;palette based.
	jz	@@ok
	cmp	TGA_ImageType[esi],2	;RGB data.
	jz	@@ok
	cmp	TGA_ImageType[esi],9	;compressed palette based.
	jz	@@ok
	cmp	TGA_ImageType[esi],10	;compressed RGB?
	jnz	@@form_error
;
;Get image dimensions/type and create a bitmap.
;
@@ok:	movzx	ecx,TGA_Wide[esi]
	movzx	edx,TGA_Depth[esi]
	mov	ebx,0
	cmp	TGA_Bits[esi],8+1
	jc	@@DoPage
	mov	ebx,2
	cmp	TGA_Bits[esi],16+1
	jc	@@DoPage
	mov	ebx,3
@@DoPage:	call	CreateBitmap
	jc	@@mem_error
	mov	@@bitmap,eax
	;
	;Read the palette.
	;
	mov	esi,offset Tg_Header
	cmp	TGA_ColourMapLength[esi],0
	jz	@@NoPalette
	movzx	eax,TGA_ColourMapBits[esi] ;get bits per entry.
	shr	eax,3		;make byte value.
	movzx	edx,TGA_ColourMapLength[esi] ;get number of entries.
	mul	edx
	mov	ecx,eax
	mov	esi,@@bitmap
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	add	eax,size BM
	add	eax,esi
	mov	edx,eax
	mov	ebx,@@handle
	call	ReadFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
	;Re-order palette info.
	;
	mov	esi,edx
	mov	ecx,256
@@p0:	mov	al,0[esi]
	shr	al,2
	xchg	al,[esi+2]
	shr	al,2
	mov	[esi],al
	shr	b[esi+1],2
	add	esi,3
	dec	ecx
	jnz	@@p0
	;
@@NoPalette:	;Fetch the image data.
	;
	mov	ebx,@@handle
	xor	ecx,ecx
	mov	al,1
	call	SetFilePointer	;get current position.
	mov	edx,ecx
	xor	ecx,ecx
	mov	al,2
	call	SetFilePointer	;find end of file.
	sub	ecx,edx		;data in between must be ours.
	xchg	ecx,edx
	xor	al,al
	call	SetFilePointer	;now put it back where it was.
	xchg	ecx,edx
	mov	esi,offset Tg_Header
	movzx	eax,TGA_ImageType[esi]	;get type.
	test	eax,8		;compressed data?
	jnz	@@comp
	mov	esi,@@bitmap
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	mov	ecx,eax
	add	esi,size BM
	jmp	@@ncomp
@@comp:	call	Malloc
	jc	@@mem_error
	mov	@@body,esi
@@ncomp:	mov	edx,esi
	call	ReadFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
	mov	esi,offset Tg_Header
	movzx	eax,TGA_ImageType[esi]	;get type.
	test	eax,8		;compressed data?
	jz	@@flips
;
;Decompress the data.
;
	mov	edi,@@bitmap
	mov	eax,BM_PWide[edi]
	mul	BM_Wide[edi]
	mov	ebx,eax		;get line width in bytes.
	add	edi,size BM
	mov	esi,offset Tg_Header
	movzx	eax,TGA_Wide[esi]
	movzx	edx,TGA_Depth[esi]
	mul	edx
	mov	ecx,eax
	movzx	eax,TGA_ImageType[esi]	;get type.
	mov	esi,@@body
	and	eax,7
	cmp	eax,2
	jz	@@24bit
@@8bit:	call	TGADecomp8Bit	;decompress this line.
	jmp	@@0
@@24Bit:	call	TGADecomp24Bit	;decompress this line.
@@0:	jc	@@form_error
;
;Perform flips as needed.
;
@@flips:	mov	esi,offset Tg_Header
	test	TGA_Descriptor[esi],32	;check for reverse in Y.
	jnz	@@NoYFlip
	mov	esi,@@bitmap
	call	BitmapYFlip
@@NoYFlip:	mov	esi,offset Tg_Header
	test	TGA_Descriptor[esi],16	;check for reverse in X.
	jnz	@@NoXFlip
	mov	esi,@@bitmap
;	call	BitmapXFlip
@@NoXFlip:	;
	xor	ebx,ebx
	jmp	@@exit
	;
@@file_error:	mov	ebx,1
	jmp	@@exit
	;
@@mem_error:	mov	ebx,2
	jmp	@@exit
	;
@@form_error:	mov	ebx,3
	jmp	@@exit
	;
@@exit:	xchg	ebx,@@handle
	or	ebx,ebx
	jz	@@closed
	call	CloseFile
@@closed:	mov	esi,@@body
	or	esi,esi
	jz	@@freed1
	call	Free
@@freed1:	cmp	@@handle,0
	jz	@@freed2
	mov	esi,@@bitmap
	call	Free
@@freed2:	popad
	mov	eax,@@handle
	mov	esi,@@bitmap
	ret
LoadTGA	endp


;------------------------------------------------------------------------------
;
;Decompress 8 bit TGA data.
;
;On Entry:
;
;ECX	- length
;ESI	- source
;EDI	- destination
;
;On Exit:
;
;ESI	- next byte in source.
;
;All other registers preserved.
;
TGADecomp8Bit	proc	near
	pushm	eax,ebx,ecx,edx,edi,ebp
@@0:	movzx	eax,b[esi]
	inc	esi
	or	al,al
	jns	@@raw
	and	al,127
	inc	eax
	sub	ecx,eax
	js	@@form_error
	mov	edx,eax
	mov	al,[esi]
	inc	esi
	xchg	ecx,edx
	rep_stosb
	xchg	ecx,edx
	jmp	@@1
	;
@@raw:	inc	eax
	sub	ecx,eax
	js	@@form_error
	xchg	ecx,eax
	rep_movsb
	xchg	ecx,eax
	;
@@1:	or	ecx,ecx
	jnz	@@0
	clc
	jmp	@@9
	;
@@form_error:	stc
@@9:	popm	eax,ebx,ecx,edx,edi,ebp
	ret
TGADecomp8Bit	endp


;------------------------------------------------------------------------------
;
;Decompress 24 bit TGA data.
;
;On Entry:
;
;ECX	- length
;ESI	- source
;EDI	- destination
;
;On Exit:
;
;ESI	- next byte in source.
;
;All other registers preserved.
;
TGADecomp24Bit	proc	near
	pushm	eax,ebx,ecx,edx,edi,ebp
@@0:	movzx	eax,b[esi]
	inc	esi
	or	al,al
	jns	@@raw
	and	al,127
	inc	eax
	sub	ecx,eax
	js	@@form_error
	mov	edx,eax
	mov	eax,[esi]
	add	esi,3
	and	eax,0FFFFFFh
	mov	ebx,eax
	shr	ebx,16
	xchg	ecx,edx
@@0_0:	mov	[edi],ax
	mov	[edi+2],bl
	add	edi,3
	dec	ecx
	jnz	@@0_0
	xchg	ecx,edx
	jmp	@@1
	;
@@raw:	inc	eax
	sub	ecx,eax
	js	@@form_error
	xchg	ecx,eax
	mov	ebx,ecx
	add	ecx,ecx
	add	ecx,ebx
	rep_movsb
	xchg	ecx,eax
	;
@@1:	or	ecx,ecx
	jnz	@@0
	clc
	jmp	@@9
	;
@@form_error:	stc
@@9:	popm	eax,ebx,ecx,edx,edi,ebp
	ret
TGADecomp24Bit	endp


	sdata
;
TG_Header	db size TGA dup (?)
;
	efile
	end

