	include ..\cwlib.inc

BMHD	struc
BMHD_Wide	dw ?		;X size of data
BMHD_Depth	dw ?		;Y size of data
BMHD_UnDef2	dd ?
BMHD_Planes	db ?		;Number of colour planes
BMHD_Brush	db ?		;0 for picture,1 for with stencil,2 for brush?
BMHD_Comp	db ?		;0 for Camera, 1 for compresion?
BMHD_UnDef3	db ?
BMHD_UnDef4	dd ?
BMHD_dWide	dw ?		;X resolution drawn in ?
BMHD_dDepth	dw ?		;Y resolution drawn in?
BMHD	ends


OutSize	equ	4096*2*3


	scode


;------------------------------------------------------------------------------
;
;Loads and unpacks an IFF image file.
;
;On Entry:
;
;EDX	- pointer to name of file to load.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- Bitmap pointer.
;
;ALL other registers preserved.
;
LoadIFF	proc	near
	local @@bitmap:dword, @@handle:dword, @@linebuffer:dword, \
	@@format:dword, @@body:dword
	pushad
	mov	@@handle,0
	mov	@@bitmap,0
	mov	@@linebuffer,0
	mov	@@body,0
;
;Open the file.
;
	call	OpenFile
	jc	@@file_error
	mov	@@Handle,ebx
;
;See if we can find a main description.
;
	mov	eax,"ILBM"		;Try planar format first.
	mov	ecx,"BMHD"
	xor	edx,edx
	mov	ebx,@@handle
	call	FindIFFChunk
	or	ecx,ecx		;Find anything?
	jnz	@@gBMHD
	or	edx,edx
	jz	@@tPBM
	dec	edx
	jz	@@form_error
	jmp	@@file_error
	;
@@tPBM:	mov	eax,"PBM "		;Try none planar format.
	mov	ecx,"BMHD"
	xor	edx,edx
	mov	ebx,@@handle
	call	FindIFFChunk
	or	ecx,ecx		;Find anything?
	jnz	@@gBMHD
	or	edx,edx
	jz	@@form_error
	dec	edx
	jz	@@form_error
	jmp	@@file_error
@@gBMHD:	mov	@@format,eax		;Store format type.
;
;Read the header info.
;
	cmp	edx,size BMHD	;check size
	jnz	@@form_error
	mov	al,0
	mov	ebx,@@handle
	call	SetFilePointer	;move to the data.
	mov	ecx,edx		;get length.
	mov	edx,offset BMHD_Buffer
	call	ReadFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
;
;Change sizes round so they make sense.
;
	mov	esi,edx
	mov	ax,BMHD_Depth[esi]
	xchg	ah,al
	mov	BMHD_Depth[esi],ax
	movzx	edx,ax
	mov	ax,BMHD_Wide[esi]
	xchg	ah,al
	mov	BMHD_Wide[esi],ax
	movzx	ecx,ax
;
;Create blank bitmap.
;
	xor	ebx,ebx
	cmp	BMHD_Planes[esi],8+1	;Get number of planes.
	jc	@@gPlanes
	inc	ebx
	cmp	BMHD_Planes[esi],15+1	;Get number of planes.
	jc	@@gPlanes
	inc	ebx
	cmp	BMHD_Planes[esi],16+1	;Get number of planes.
	jc	@@gPlanes
	inc	ebx
	cmp	BMHD_Planes[esi],24+1	;Get number of planes.
	jc	@@gPlanes
	jmp	@@form_error
	;
@@gPlanes:	call	CreateBitmap
	jc	@@mem_error
	mov	@@bitmap,eax
;
;See if we can find a palette.
;
	mov	eax,@@format		;use correct format.
	mov	ecx,"CMAP"
	xor	edx,edx
	mov	ebx,@@handle
	call	FindIFFChunk
	or	ecx,ecx		;Find anything?
	jnz	@@gCMAP
	or	edx,edx		;make sure wasn't a file error.
	jz	@@nCMAP
	dec	edx
	jz	@@form_error
	jmp	@@file_error
	;
@@gCMAP:	mov	al,0
	mov	ebx,@@handle
	call	SetFilePointer	;move to the data.
	mov	ecx,edx		;get length.
	cmp	ecx,256*3		;check it's not too long.
	jc	@@rCMAP
	mov	ecx,256*3
@@rCMAP:	mov	esi,@@bitmap
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	add	eax,size BM
	add	eax,esi
	mov	edx,eax		;point to palette destination.
	call	ReadFile		;read the palette
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
@@cCMAP:	shr	b[edx],2		;scale data.
	inc	edx
	dec	ecx
	jnz	@@cCMAP
@@nCMAP:	;
;
;Better find the main data.
;
	mov	eax,@@format		;use correct format.
	mov	ecx,"BODY"
	xor	edx,edx
	mov	ebx,@@handle
	call	FindIFFChunk
	or	ecx,ecx
	jnz	@@gBODY
	or	edx,edx
	jz	@@form_error
	dec	edx
	jz	@@form_error
	jmp	@@file_error
	;
@@gBODY:	mov	al,0
	mov	ebx,@@handle
	call	SetFilePointer	;move to the data.
	mov	ecx,edx		;get length.
	call	Malloc		;find somewhere to put it.
	jc	@@mem_error
	mov	@@body,esi
	mov	edx,esi
	call	ReadFile		;read the data.
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
	mov	ecx,OutSize
	call	Malloc		;need a line buffer.
	jc	@@mem_error
	mov	@@linebuffer,esi
;
;Now decode the data according to its format.
;
	cmp	@@format,"PBM "	;none planar?
	jnz	@@gPlanar
;
;none planar mode.
;
	mov	ebx,offset BMHD_Buffer
	mov	esi,@@body
	mov	edi,@@bitmap
	mov	ecx,BM_Wide[edi]
	mov	edx,BM_Depth[edi]
	add	edi,size BM
@@0:	pushm	ebx,ecx,edx,edi
	movzx	ecx,BMHD_Wide[ebx]
	test	BMHD_Comp[ebx],1	;compressed?
	jnz	@@1
	rep_movsb
	clc
	jmp	@@2
@@1:	add	ecx,15		;round it up.
	and	ecx,not 15
	mov	edi,@@linebuffer
	call	DecompRLE
	popm	ebx,ecx,edx,edi
	pushm	ebx,ecx,edx,edi
	push	esi
	mov	esi,@@linebuffer
	rep_movsb		;copy the real data.
	pop	esi
@@2:	popm	ebx,ecx,edx,edi
	jc	@@form_error
	add	edi,ecx
	dec	edx
	jnz	@@0
	;
	xor	ebx,ebx
	jmp	@@exit
;
;planar mode stuff.
;
@@gPlanar:	mov	ebx,offset BMHD_Buffer
	movzx	eax,BMHD_Wide[ebx]
	add	eax,15
	and	eax,not 15
	shr	eax,3
	movzx	ecx,BMHD_Planes[ebx]
	mul	ecx
	mov	ecx,eax
	mov	esi,@@body
	mov	edi,@@bitmap
	mov	edx,BM_Depth[edi]
	add	edi,size BM
@@3:	pushm	ebx,ecx,edx,edi
	push	ecx
	mov	edi,@@linebuffer
	xor	eax,eax
	mov	ecx,OutSize/(2*4)
	rep	stosd
	pop	ecx
	mov	edi,@@linebuffer
	test	BMHD_Comp[ebx],1	;compressed?
	jnz	@@4
	rep_movsb
	clc
	jmp	@@5
@@4:	call	DecompRLE		;decompress the data.
@@5:	popm	ebx,ecx,edx,edi
	jc	@@form_error
	pushm	ebx,ecx,edx,edi,esi
	movzx	eax,BMHD_Planes[ebx]
	mov	esi,@@linebuffer
	push	edi
	mov	edi,esi
	add	edi,OutSize/2
	call	MergePlanes		;put the planes together.
	pop	edi
	mov	esi,@@bitmap
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mov	ecx,eax
	mov	esi,@@linebuffer
	add	esi,OutSize/2
	rep_movsb
	popm	ebx,ecx,edx,edi,esi
	push	edx
	mov	edx,@@bitmap
	mov	eax,BM_PWide[edx]
	mul	BM_Wide[edx]
	pop	edx
	add	edi,eax
	dec	edx
	jnz	@@3
	;
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
	jz	@@freed0
	call	Free
@@freed0:	mov	esi,@@linebuffer
	or	esi,esi
	jz	@@freed1
	call	Free
@@freed1:	cmp	@@handle,0
	jz	@@freed2
	xor	esi,esi
	xchg	esi,@@bitmap
	or	esi,esi
	jz	@@freed2
	call	Free
@@freed2:	popad
	mov	eax,@@handle
	mov	esi,@@bitmap
	ret
LoadIFF	endp


;------------------------------------------------------------------------------
;
;On Entry:
;
;EAX	- planes.
;ECX	- data length in bytes.
;EDI	- source data.
;ESI	- buffer to use.
;
;On Exit:
;
;All registers preserved.
;
MergePlanes	proc	near
	pushad
	xchg	ecx,eax
	xor	edx,edx
	div	ecx
	xchg	eax,ecx		;get plane length.
	mov	ebx,eax		;save planes.
	mov	ebp,ecx		;get plane size.
@@003:	push	ecx
	push	esi
	mov	ah,128		;bit being tested this time
	mov	ecx,8		;bits per byte of plane info.
@@002:	push	ecx
	push	esi
	mov	dh,1		;bit to be added
	xor	dl,dl		;made up byte
	mov	ecx,ebx		;planes per pixel
@@001:	test	b[esi],ah
	jz	@@000
	or	dl,dh
@@000:	add	esi,ebp		;move to next plane
	rol	dh,1		;next plane bit
	jnc	@@nodump
	mov	[edi],dl
	inc	edi
	xor	dl,dl
@@nodump:	dec	ecx
	jnz	@@001
	cmp	dh,1	;start of plane?
	jz	@@NoExtra
	mov	[edi],dl
	inc	edi
@@NoExtra:	cmp	ebx,24
	jnz	@@NoSwitchBytes
	sub	edi,3
	mov	al,[edi+0]
	xchg	al,[edi+2]
	mov	[edi+0],al
	add	edi,3
@@NoSwitchBytes:
	pop	esi
	ror	ah,1	;next bit to be done
	pop	ecx
	dec	ecx
	jnz	@@002
	pop	esi
	inc	esi
	pop	ecx
	dec	ecx
	jnz	@@003
	popad
	ret
MergePlanes	endp


;------------------------------------------------------------------------------
;
;Decompress RLE data.
;
;On Entry:
;
;ECX	- target data length
;ESI	- source data.
;EDI	- destination.
;
;On Exit:
;
;Carry set on format error else,
;
;ESI	- next byte in source.
;EDI	- next byte in buffer.
;
;All other registers preserved.
;
DecompRLE	proc	near
	pushm	eax,ebx,ecx,edx,ebp
	;
@@100:	mov	al,b[esi]
	inc	esi
	cmp	al,80h
	jz	@@100		;padding byte so skip it
	or	al,al
	js	@@102		;Negative so repeat next byte ((-al)+1) times
	;
	;Copy next AL+1 bytes
	;
	inc	al
	movzx	edx,al
	sub	ecx,edx		;update the width counter
	js	@@form_error
	xchg	ecx,edx
	rep_movsb
	mov	ecx,edx
	jmp	@@104
	;
@@102:	;Repeat next byte ((-AL)+1) times
	;
	neg	al
	inc	al
	movzx	edx,al
	sub	ecx,edx		;update width counter
	js	@@form_error
	xchg	edx,ecx
	mov	al,[esi]
	inc	esi
	rep_stosb
	mov	ecx,edx
	;
@@104:	or	ecx,ecx
	jnz	@@100		;Get complete plane
	clc
	jmp	@@exit
	;
@@form_error:	stc
	;
@@exit:	popm	eax,ebx,ecx,edx,ebp
	ret
DecompRLE	endp


;------------------------------------------------------------------------------
;
;Find specified chunk type in an IFF file.
;
;On Entry:
;
;EAX	- FORM type, eg, "ILBM"
;ECX	- chunk type, eg, "BMHD"
;EDX	- occurance number.
;EBX	- file handle.
;
;On Exit:
;
;ECX	- offset of chunk data, 0 if none found or error, EDX=error.
;EDX	- length of chunk data.
;
;Errors:
;
;0	- not found.
;1	- FORM error.
;2	- file access error.
;
;All other registers preserved.
;
FindIFFChunk	proc	near
	local @@form:dword, @@chunk:dword, @@count:dword, @@handle:dword, \
	@@pos:dword, @@flength:dword, @@length:dword, @@buffer[12]:byte
	pushad
	;
	xchg	ah,al
	rol	eax,16
	xchg	ah,al
	mov	@@form,eax
	xchg	ch,cl
	rol	ecx,16
	xchg	ch,cl
	mov	@@chunk,ecx
	mov	@@count,edx
	mov	@@handle,ebx
	mov	@@pos,0
	;
	xor	ecx,ecx
	mov	al,2
	call	SetFilePointer
	mov	@@length,ecx		;store file length.
;
;Find desired FORM first.
;
@@lform:	cmp	@@length,0
	jz	@@not_found
	js	@@form_error
	mov	ecx,@@pos		;move back to start of the file.
	mov	al,0
	call	SetFilePointer
	mov	ecx,4+4+4		;FORM+length+TYPE
	lea	edx,@@buffer
	call	ReadFile		;Read form/chunk info.
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	cmp	d[edx],"MROF"	;FORM header?
	jz	@@gform
@@nform:	mov	eax,[edx+4]		;Get length.
	xchg	ah,al
	rol	eax,16
	xchg	ah,al		;convert to "little endian"
	add	eax,1
	and	eax,not 1		;round up to nearest word.
	add	eax,8		;include this header.
	add	@@pos,eax		;update file position.
	sub	@@length,eax
	jmp	@@lform		;keep looking.
	;
@@gform:	mov	eax,[edx+8]		;Get FORM type.
	cmp	eax,@@form		;right type?
	jnz	@@nform
	sub	@@length,4+4+4
	mov	eax,[edx+4]		;Get length.
	xchg	ah,al
	rol	eax,16
	xchg	ah,al		;convert to "little endian"
	add	eax,1
	and	eax,not 1		;round up to nearest word.
	sub	eax,4
	mov	@@flength,eax	;store this FORM's length.
	add	@@pos,4+4+4		;skip over "FORM" etc.
	;
@@lchunk:	cmp	@@flength,0
	jz	@@lform
	js	@@form_error
	mov	ecx,@@pos		;move back to start of the file.
	mov	al,0
	call	SetFilePointer
	mov	ecx,4+4		;chunk+length
	lea	edx,@@buffer
	call	ReadFile		;Read form/chunk info.
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	mov	eax,[edx]		;Get chunk ID.
	cmp	eax,@@chunk		;right chunk?
	jz	@@gchunk
@@nchunk:	mov	eax,[edx+4]		;Get length.
	xchg	ah,al
	rol	eax,16
	xchg	ah,al		;convert to "little endian"
	add	eax,1
	and	eax,not 1		;round up to nearest word
	add	eax,4+4		;include this header.
	add	@@pos,eax		;update file position.
	sub	@@flength,eax
	sub	@@length,eax
	jmp	@@lchunk		;keep looking.
	;
@@gchunk:	dec	@@count
	jns	@@nchunk
	mov	eax,[edx+4]		;Get length.
	xchg	ah,al
	rol	eax,16
	xchg	ah,al		;convert to "little endian"
	mov	@@handle,eax
	add	@@pos,4+4
	jmp	@@exit
	;
@@not_found:	mov	@@pos,0
	mov	@@handle,0
	jmp	@@exit
@@form_error:	mov	@@pos,0
	mov	@@handle,1
	jmp	@@exit
@@file_error:	mov	@@pos,0
	mov	@@handle,2
	;
@@exit:	popad
	mov	ecx,@@pos
	mov	edx,@@handle
	ret
FindIFFChunk	endp


	sdata
;
BMHD_Buffer	BMHD <>
;
	efile

	end

