	include ..\cwlib.inc


PCX		struc
PCX_ID		db ?	;Manufacturer       Constant Flag, 10 = ZSoft .pcx 
PCX_Version		db ?	;Version      1      Version information 
			;0 = Version 2.5 of PC Paintbrush 
			;2 = Version 2.8 w/palette information 
			;3 = Version 2.8 w/o palette information 
			;4 = PC Paintbrush for Windows(Plus for
			;   Windows uses Ver 5) 
			;5 = Version 3.0 and > of PC Paintbrush
			;   and PC Paintbrush +, includes
			;   Publisher's Paintbrush . Includes
			;   24-bit .PCX files 
PCX_Encoding		db ?	;Encoding      1     1 = .PCX run length encoding 
PCX_BitsPerPixel	db ?	;BitsPerPixel  1     Number of bits to represent a pixel
			;(per Plane) - 1, 2, 4, or 8 
PCX_Xmin		dw ?
PCX_Ymin		dw ?
PCX_Xmax		dw ?
PCX_Ymax		dw ?	;Window        8     Image Dimensions: Xmin,Ymin,Xmax,Ymax 
PCX_HDpi		dw ?	;HDpi          2     Horizontal Resolution of image in DPI* 
PCX_VDpi		dw ?	;VDpi          2     Vertical Resolution of image in DPI* 
PCX_ColourMap		db 48 dup (0) ;Colormap     48     Color palette setting, see text 
PCX_Res1		db ?	;Reserved      1     Should be set to 0. 
PCX_NPlanes		db ?	;NPlanes       1     Number of color planes
PCX_BytesPerLine	dw ?	;BytesPerLine  2     Number of bytes to allocate for a scanline
			;plane.  MUST be an EVEN number.  Do NOT
			;calculate from Xmax-Xmin. 
PCX_PaletteInfo	dw ?	;PaletteInfo   2     How to interpret palette- 1 = Color/BW,
			;2 = Grayscale (ignored in PB IV/ IV +) 
PCX_HscreenSize	dw ?	;HscreenSize   2     Horizontal screen size in pixels. New field
			;found only in PB IV/IV Plus 
PCX_VscreenSize	dw ?	;VscreenSize   2     Vertical screen size in pixels. New field
			;found only in PB IV/IV Plus 
PCX_Res2		db 54 dup (0) ;Filler       54     Blank to fill out 128 byte header.  Set all
			;bytes to 0 
PCX		ends


	scode


;------------------------------------------------------------------------------
;
;Load a PCX file.
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
LoadPCX	proc	near
	local @@handle:dword, @@bitmap:dword, @@body:dword, @@length:dword, \
	@@linebuffer:dword, @@width:dword, @@depth:dword, @@datawidth:dword, \
	@@bitmapdest:dword, @@bitmapwide:dword, @@pwide:dword
	pushad
	mov	@@handle,0
	mov	@@bitmap,0
	mov	@@body,0
	mov	@@lineBuffer,0
	;
	;Open the file.
	;
	call	OpenFile
	jc	@@file_error
	mov	@@handle,ebx
	;
	;Read main header.
	;
	mov	edx,offset PCX_Header
	mov	ecx,size PCX
	call	ReadFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
	;Check it's a PCX file and that we know how to deal with it.
	;
	mov	esi,offset PCX_Header
	cmp	PCX_ID[esi],10
	jnz	@@form_error
	cmp	PCX_Encoding[esi],1
	jnz	@@form_error
	cmp	PCX_BitsPerPixel[esi],8
	jnz	@@form_error
	cmp	PCX_NPlanes[esi],1
	jz	@@pok
	cmp	PCX_NPlanes[esi],3
	jz	@@pok
	jmp	@@form_error
	;
@@pok:	;Get size of the rest of the file.
	;
	xor	ecx,ecx
	mov	al,2
	call	SetFilePointer
	push	ecx
	mov	ecx,size PCX
	xor	al,al
	call	SetFilePointer
	pop	ecx
	sub	ecx,size PCX
	mov	@@length,ecx
	;
	;Get image dimensions & flags.
	;
	mov	esi,offset PCX_Header
	movzx	eax,PCX_BitsPerPixel[esi]
	movzx	edx,PCX_NPlanes[esi]
	mul	edx
	xor	ebx,ebx
	cmp	eax,8+1
	jc	@@0
	mov	ebx,3
@@0:	movzx	ecx,PCX_Xmax[esi]
	movzx	eax,PCX_Xmin[esi]
	sub	ecx,eax
	inc	ecx
	mov	@@width,ecx
	movzx	edx,PCX_Ymax[esi]
	movzx	eax,PCX_Ymin[esi]
	sub	edx,eax
	inc	edx
	mov	@@depth,edx
	call	CreateBitmap
	jc	@@mem_error
	mov	@@bitmap,eax
	mov	esi,eax
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mov	@@bitmapwide,eax
	;
	;Get some memory for the rest of the file.
	;
	mov	ecx,@@length
	call	Malloc
	jc	@@mem_error
	mov	@@body,esi
	;
	;Read the rest of the file.
	;
	mov	ebx,@@handle
	mov	edx,esi
	call	ReadFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
	;Allocate a line buffer to allow for padding.
	;
	mov	esi,offset PCX_Header
	movzx	eax,PCX_BytesPerLine[esi]
	mov	@@pwide,eax
	movzx	edx,PCX_NPlanes[esi]
	mul	edx
	mov	ecx,eax
	shl	ecx,1
	mov	@@datawidth,eax
	call	Malloc
	jc	@@mem_error
	mov	@@lineBuffer,esi
	;
	;Start decoding the data a line at a time.
	;
	mov	edi,@@bitmap
	add	edi,size BM
	mov	@@bitmapdest,edi
	mov	esi,@@body
	;
@@d3:	;Decode 1 lines worth of data.
	;
	mov	edi,@@linebuffer
	mov	ecx,@@datawidth
	xor	al,al
	rep_stosb
	mov	edi,@@linebuffer
	mov	ecx,@@datawidth
@@d0:	mov	al,[esi]
	mov	ah,al
	inc	esi
	and	ah,11000000b
	cmp	ah,11000000b
	jnz	@@d1
	;do a repeat.
	and	eax,63
	jz	@@d2
	sub	ecx,eax
	js	@@form_error
	push	ecx
	mov	ecx,eax
	mov	al,[esi]
	inc	esi
	rep_stosb
	pop	ecx
	jmp	@@d2
@@d1:	;do single byte.
	mov	[edi],al
	inc	edi
	dec	ecx
@@d2:	;check for end of line and carry on if not there.
	or	ecx,ecx
	jnz	@@d0
	;
	;Now convert the data into our internal format.
	;
	mov	edi,@@bitmap
	mov	eax,BM_Flags[edi]
	and	eax,15
	jz	@@256
	cmp	eax,3
	jnz	@@form_error
@@16m:	push	esi
	mov	ecx,BM_Wide[edi]
	mov	edx,@@pwide
	mov	esi,@@linebuffer
	mov	edi,@@bitmapdest
@@16m0:	mov	al,[esi+edx*2]
	mov	[edi],al
	inc	edi
	mov	al,[esi+edx]
	mov	[edi],al
	inc	edi
	mov	al,[esi]
	mov	[edi],al
	inc	edi
	inc	esi
	dec	ecx
	jnz	@@16m0
	pop	esi
	jmp	@@gotline
	;
@@256:	push	esi
	mov	esi,@@linebuffer
	mov	edi,@@bitmapdest
	mov	ecx,@@bitmapwide
	rep_movsb
	pop	esi
@@gotline:	;
	mov	eax,@@bitmapwide
	add	@@bitmapdest,eax
	;
	;Do all lines.
	;
	dec	@@depth
	jnz	@@d3
;
;Time to fetch the palette if there is one.
;
	mov	esi,offset PCX_Header
	cmp	PCX_Version[esi],5
	jc	@@NoPal
	cmp	PCX_BitsPerPixel[esi],8
	jnz	@@NoPal
	cmp	PCX_NPlanes[esi],1
	jnz	@@NoPal
	mov	esi,@@body
	add	esi,@@length
	sub	esi,768+1
	cmp	b[esi],12
	jnz	@@NoPal
	inc	esi
	mov	edi,@@bitmap
	mov	eax,BM_PWide[edi]
	mul	BM_Wide[edi]
	mul	BM_Depth[edi]
	add	eax,size BM
	add	edi,eax
	mov	ecx,256*3
@@gp0:	mov	al,[esi]
	shr	al,2
	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@gp0
@@NoPal:	;
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
@@freed2:	mov	esi,@@linebuffer
	or	esi,esi
	jz	@@freed3
	call	Free
@@freed3:	popad
	mov	eax,@@handle
	mov	esi,@@bitmap
	ret
LoadPCX	endp


	sdata

PCX_Header	db size PCX dup (0)


	efile
	end

