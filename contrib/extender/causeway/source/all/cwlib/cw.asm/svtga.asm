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
;Save a TGA file.
;
;On Entry:
;
;EAX	- Flags,
;	0 - Compress.
;EDX	- File name.
;ESI	- bitmap to compress.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file error.
;	2 - not enough memory (for buffers).
;	3 - format error.
;
;All other registers preserved.
;
SaveTGA	proc	near
	local @@flags:dword, @@bitmap:dword, @@handle:dword, @@linebuffer:dword, \
	@@lwidth:dword
	pushad
	mov	@@flags,eax
	mov	@@bitmap,esi
	;
	mov	@@linebuffer,0
	mov	@@handle,0
	call	CreateFile
	jc	@@file_error
	mov	@@handle,ebx
;
;Setup and write the main header.
;
	mov	edi,offset TG_Header
	mov	esi,@@bitmap
	mov	TGA_IdentSize[edi],0
	mov	TGA_ColourMapStart[edi],0
	mov	TGA_XStart[edi],0
	mov	TGA_YStart[edi],0
	mov	eax,BM_Wide[esi]
	mov	TGA_Wide[edi],ax
	mov	eax,BM_Depth[esi]
	mov	TGA_Depth[edi],ax
	mov	TGA_Descriptor[edi],32
	mov	eax,BM_Flags[esi]
	and	eax,15
	jz	@@8bit
	dec	eax
	jz	@@form_error
	dec	eax
	jz	@@form_error
	dec	eax
	jz	@@24bit
	jmp	@@form_error
	;
@@8bit:	mov	TGA_ColourMapType[edi],1
	mov	TGA_ImageType[edi],1
	mov	TGA_ColourMapLength[edi],256
	mov	TGA_ColourMapBits[edi],24
	mov	TGA_Bits[edi],8
	jmp	@@0bits
	;
@@24bit:	mov	TGA_ColourMapType[edi],0
	mov	TGA_ImageType[edi],2
	mov	TGA_ColourMapLength[edi],0
	mov	TGA_ColourMapBits[edi],24
	mov	TGA_Bits[edi],24
	;
@@0bits:	test	@@flags,1		;compress output?
	jz	@@nocomp
	or	TGA_ImageType[edi],8
@@nocomp:	mov	edx,edi
	mov	ecx,size TGA
	mov	ebx,@@handle
	call	WriteFile		;Write the header.
	jc	@@file_error
	cmp	eax,ecx
	jc	@@file_error
	;
	cmp	TGA_ColourMapLength[edi],0 ;any palette?
	jz	@@nopal
	pushm	esi,edi
	mov	ecx,256*3
	call	Malloc
	mov	eax,esi
	popm	esi,edi
	jc	@@mem_error
	pushm	esi,edi
	mov	@@linebuffer,eax
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	add	eax,size BM
	add	esi,eax
	mov	edi,@@linebuffer
	mov	ecx,256*3
@@gpal:	lodsb
	mov	ah,al
	and	ah,1
	shl	al,2
	or	al,ah
	shl	ah,1
	or	al,ah
	stosb
	loop	@@gpal
	sub	edi,256*3
	mov	ecx,256
@@spal:	mov	al,[edi+0]
	xchg	al,[edi+2]
	mov	[edi+0],al
	add	edi,3
	loop	@@spal
	popm	esi,edi
	mov	edx,@@linebuffer
	mov	ecx,256*3
	mov	ebx,@@handle
	call	WriteFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	push	esi
	xor	esi,esi
	xchg	esi,@@linebuffer
	call	Free
	pop	esi
	;
@@nopal:	test	TGA_ImageType[edi],8	;Compressed?
	jnz	@@compress
	;
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	mov	ecx,eax
	mov	edx,esi
	add	edx,size BM
	call	WriteFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	xor	ebx,ebx
	jmp	@@exit
	;
@@compress:	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	mov	ecx,eax
	shr	eax,1
	add	ecx,eax
	call	Malloc
	jc	@@mem_error
	mov	@@linebuffer,esi
	;
	mov	esi,@@bitmap
	mov	eax,BM_Wide[esi]
	mul	BM_Depth[esi]
	mov	ecx,eax
	mov	eax,BM_Flags[esi]
	and	eax,15
	add	esi,size BM
	mov	edi,@@linebuffer
	or	eax,eax
	jnz	@@cpr24
@@cpr8:	call	TGAComp8Bit
	jmp	@@cpr1
@@cpr24:	call	TGAComp24Bit
@@cpr1:	sub	edi,@@linebuffer
	mov	ecx,edi
	mov	edx,@@linebuffer
	mov	ebx,@@handle
	call	WriteFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
	xor	ebx,ebx
	jmp	@@exit
	;
@@file_error:	mov	ebx,1
	jmp	@@exit
@@mem_error:	mov	ebx,2
	jmp	@@exit
@@form_error:	mov	ebx,3
	;
@@exit:	xchg	ebx,@@handle
	or	ebx,ebx
	jz	@@closed
	call	CloseFile
@@closed:	mov	esi,@@linebuffer
	or	esi,esi
	jz	@@freed
	call	Free
@@freed:	popad
	mov	eax,@@handle
	ret
SaveTGA	endp


;------------------------------------------------------------------------------
;
;bit 7, set=repetition, next byte = repeat value.
;       zero=raw, next &127 bytes= raw data.
;
;counts=+1
;
;values are grouped, eg, 24-bit is compressed in 3 byte chunks.
;
;On Entry:
;
;ESI	- source.
;EDI	- destination.
;ECX	- width.
;
;On Exit:
;
;EDI	- next byte in output.
;
TGAComp8Bit	proc	near
	push	ebp
	mov	dh,0		;reset copy flag.
@@CompLoop:	or	ecx,ecx		;finished this line?
	jz	@@Finished		;/
	mov	al,[esi]		;Fetch a byte.
	cmp	ecx,1		;Last byte on the line?
	jz	@@CopyToFin		;Copy last byte.
	cmp	al,[esi+1]		;any repeat yet?
	jz	@@CountRep		;/
@@CountCopy:	mov	dl,-1		;reset the counter.
	mov	ebp,esi		;store position.
@@CntCop0:	cmp	ecx,1
	jz	@@CntCop1
	cmp	dl,127		;check the length.
	jz	@@CntCop1		;/
	mov	al,[esi]		;get current byte.
	cmp	al,[esi+1]		;repeated still?
	jz	@@CntCop1		;put in the output string.
	inc	esi		;next byte.
	inc	dl		;update counter.
	loop	@@CntCop0		;scan to line end.
@@CntCop1:	or	dh,dh		;Last action copy?
	jz	@@CntCop2		;no,do as normal.
	mov	al,dl		;get the count value
	inc	al		;get number of bytes to add.
	add	al,[ebx]		;add previouse length.
	inc	al
	cmp	al,127		;check its not too long.
	jnc	@@CntCop2		;do as normal if it is.
	dec	al
	mov	[ebx],al		;Set the new count value.
	mov	esi,ebp		;Get start of this copy
	push	ecx		;save width left to do
	movzx	ecx,dl		;Set up the loop counter
	inc	ecx		;/
	rep	movsb		;and shift it!
	pop	ecx		;get width remaining back
	jmp	@@CompLoop		;go back to main loop.
@@CntCop2:	mov	esi,ebp		;point at the data again.
	mov	ebx,edi		;store command byte position.
	mov	[edi],dl		;store copy count value.
	inc	edi		;/
	push	ecx		;save length remaining.
	movzx	ecx,dl		;get count value.
	inc	ecx		;/
	rep	movsb		;copy it!
	pop	ecx		;get remaining width back
	mov	dh,1		;Set copy flag.
	jmp	@@CompLoop		;back to main loop.
@@CountRep:	mov	dl,-1		;reset the counter
@@CntRep0:	cmp	dl,127		;maximum count?
	jz	@@CntRep1		;do it.
	cmp	al,[esi]		;still repeating?
	jnz	@@CntRep1		;dump this value.
	inc	esi		;next byte.
	inc	dl		;update counter.
	loop	@@CntRep0		;scan to line end.
@@CntRep1:	cmp	dl,2		;short repeat?
	jnc	@@CntRep2		;no, do as normal.
	or	dh,dh		;did a copy last?
	jz	@@CntRep2		;no, do as normal.
	cmp	b[ebx],126		;enough space for this?
	jnc	@@CntRep2		;no, do as normal.
	add	b[ebx],2		;set new length
	stosb			;Store this byte twice.
	stosb			;/
	jmp	@@CompLoop		;Go back to main loop!
@@CntRep2:	mov	[edi],dl		;repeat count
	or	b[edi],128
	inc	edi		;/
	stosb			;repeat value.
	mov	dh,0		;reset copy flag.
	jmp	@@CompLoop		;back to main loop
@@CopyToFin:	or	dh,dh		;did copy last?
	jz	@@CopyToFin0		;/
	cmp	b[ebx],127		;enough space for this one?
	jnc	@@CopyToFin0		;/
	inc	b[ebx]		;update count value.
	stosb			;Store this byte.
	jmp	@@Finished		;all over!
@@CopyToFin0:	mov	b[edi],0		;set command byte.
	inc	edi		;/
	stosb			;Store byte to copy.
@@Finished:	pop	ebp
	ret
TGAComp8Bit	endp


;------------------------------------------------------------------------------
;
;bit 7, set=repetition, next byte = repeat value.
;       zero=raw, next &127 bytes= raw data.
;
;counts=+1
;
;values are grouped, eg, 24-bit is compressed in 3 byte chunks.
;
;On Entry:
;
;ESI	- source.
;EDI	- destination.
;ECX	- width.
;
;On Exit:
;
;EDI	- next byte in output.
;
TGAComp24Bit	proc	near
@@start	equ	d[esp+(4*1)+0]
	sub	esp,4
	push	ebp
	xor	dh,dh		;reset copy flag.
@@CompLoop:	or	ecx,ecx		;finished this line?
	jz	@@Finished		;/
	mov	eax,[esi]		;Fetch a byte.
	and	eax,0FFFFFFh
	cmp	ecx,1		;Last byte on the line?
	jz	@@CopyToFin		;Copy last byte.
	mov	ebp,[esi+3]
	and	ebp,0FFFFFFh
	cmp	eax,ebp		;any repeat yet?
	jz	@@CountRep		;/
@@CountCopy:	or	dl,-1		;reset the counter.
	mov	@@start,esi		;store position.
@@CntCop0:	cmp	ecx,1
	jz	@@CntCop1
	cmp	dl,127		;check the length.
	jz	@@CntCop1		;/
	mov	eax,[esi]		;get current byte.
	and	eax,0FFFFFFh
	mov	ebp,[esi+3]
	and	ebp,0FFFFFFh
	cmp	eax,ebp		;repeated still?
	jz	@@CntCop1		;put in the output string.
	add	esi,3		;next byte.
	inc	dl		;update counter.
	loop	@@CntCop0		;scan to line end.
@@CntCop1:	or	dh,dh		;Last action copy?
	jz	@@CntCop2		;no,do as normal.
	mov	al,dl		;get the count value
	inc	al		;get number of bytes to add.
	add	al,[ebx]		;add previouse length.
	inc	al
	cmp	al,127		;check its not too long.
	jnc	@@CntCop2		;do as normal if it is.
	dec	al
	mov	[ebx],al		;Set the new count value.
	mov	esi,@@start		;Get start of this copy
	push	ecx		;save width left to do
	movzx	ecx,dl		;Set up the loop counter
	inc	ecx		;/
	mov	eax,ecx
	shl	ecx,1
	add	ecx,eax
	rep	movsb		;and shift it!
	pop	ecx		;get width remaining back
	jmp	@@CompLoop		;go back to main loop.
@@CntCop2:	mov	esi,@@start		;point at the data again.
	mov	ebx,edi		;store command byte position.
	mov	[edi],dl		;store copy count value.
	inc	edi		;/
	push	ecx		;save length remaining.
	movzx	ecx,dl		;get count value.
	inc	ecx		;/
	mov	eax,ecx
	shl	ecx,1
	add	ecx,eax
	rep	movsb		;copy it!
	pop	ecx		;get remaining width back
	mov	dh,1		;Set copy flag.
	jmp	@@CompLoop		;back to main loop.
@@CountRep:	mov	dl,-1		;reset the counter
@@CntRep0:	cmp	dl,127		;maximum count?
	jz	@@CntRep1		;do it.
	mov	ebp,[esi]
	and	ebp,0FFFFFFh
	cmp	eax,ebp		;still repeating?
	jnz	@@CntRep1		;dump this value.
	add	esi,3		;next byte.
	inc	dl		;update counter.
	loop	@@CntRep0		;scan to line end.
@@CntRep1:	cmp	dl,2		;short repeat?
	jnc	@@CntRep2		;no, do as normal.
	or	dh,dh		;did a copy last?
	jz	@@CntRep2		;no, do as normal.
	cmp	b[ebx],126		;enough space for this?
	jnc	@@CntRep2		;no, do as normal.
	add	b[ebx],2		;set new length
	stosb
	ror	eax,8
	stosb
	ror	eax,8
	stosb
	rol	eax,16
	stosb
	ror	eax,8
	stosb
	ror	eax,8
	stosb
	jmp	@@CompLoop		;Go back to main loop!
@@CntRep2:	mov	[edi],dl		;repeat count
	or	b[edi],128
	inc	edi		;/
	stosb			;repeat value.
	ror	eax,8
	stosb
	ror	eax,8
	stosb
	mov	dh,0		;reset copy flag.
	jmp	@@CompLoop		;back to main loop
@@CopyToFin:	or	dh,dh		;did copy last?
	jz	@@CopyToFin0		;/
	cmp	b[ebx],127		;enough space for this one?
	jnc	@@CopyToFin0		;/
	inc	b[ebx]		;update count value.
	stosb			;Store this byte.
	ror	eax,8
	stosb
	ror	eax,8
	stosb
	jmp	@@Finished		;all over!
@@CopyToFin0:	mov	b[edi],0		;set command byte.
	inc	edi		;/
	stosb			;Store byte to copy.
	ror	eax,8
	stosb
	ror	eax,8
	stosb
@@Finished:	pop	ebp
	add	esp,4
	ret
TGAComp24Bit	endp


	sdata
;
TG_Header	db size TGA dup (?)
;
	efile
	end
