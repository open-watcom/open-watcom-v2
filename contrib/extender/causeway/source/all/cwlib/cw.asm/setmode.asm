	include ..\cwlib.inc
	include ..\cw.inc
	scode

;-------------------------------------------------------------------------
;
;Attempt to set specified video mode.
;
;On Entry:
;
;EAX	- Mode.
;
;On Exit:
;
;EAX	- zero on error else mode number.
;
;ALL other registers preserved.
;
VideoSetMode	proc	near
	pushm	ebx,ecx,edx,esi,edi,ebp
	call	MouseOffCall
;
;Check for a valid mode first.
;
	mov	VideoMode,eax
	mov	ebx,eax
	mov	esi,offset ValidModeList
@@0:	lodsw
	cmp	ax,-1		;End of the list?
	jz	@@9
	cmp	ax,bx		;Mode we're after?
	jnz	@@0
	push	ebp
;
;Check if its a VESA mode or standard VGA.
;
	cmp	ax,Video320x200x256
	jnz	@@VESA
	int	10h
	mov	VideoPixelWidth,1	;Set details for standard VGA
	mov	VideoXResolution,320	;320x200x256 mode.
	mov	VideoYResolution,200
	mov	VideoHardwareWidth,320
	mov	VideoModeFlags,0
	mov	VideoBase,0a0000h
	jmp	@@SetBase
;
;Use VESA functions to get the mode's details.
;
@@VESA:	push	ebx
	mov	ax,4f02h		;Use VESA function.
	int	10h
	pop	ebx
	mov	ax,4f01h
	mov	edi,offset ModeInfoBuffer
	mov	cx,bx
	int	10h
	;
	mov	esi,offset Forced32kList
	mov	edi,offset ModeInfoBuffer
	mov	bx,15
@@32k:	lodsw
	cmp	ax,cx
	jz	@@Forced
	cmp	ax,-1
	jnz	@@32k
	mov	esi,offset Forced64kList
	inc	bx
@@64k:	lodsw
	cmp	ax,cx
	jz	@@Forced
	cmp	ax,-1
	jnz	@@64k
	movzx	bx,MIB_BitsPerPixel[edi]
@@Forced:	mov	MIB_BitsPerPixel[edi],bl
;
;Transfer mode details into relavent variables.
;
	mov	edi,offset ModeInfoBuffer
	movzx	eax,MIB_BytesPerScanLine[edi]
	mov	VideoHardwareWidth,eax
	movzx	eax,MIB_XResolution[edi]
	mov	VideoXResolution,eax
	movzx	eax,MIB_YResolution[edi]
	mov	VideoYResolution,eax
	movzx	eax,MIB_WinGranularity[edi]
	shl	eax,10
	xor	ecx,ecx
@@Bits0:	cmp	eax,65536
	jnc	@@Bits1
	shl	eax,1
	inc	ecx
	jmp	@@Bits0
@@Bits1:	mov	BankShift,cl
;
;Need to know if it's text or graphics mode.
;
	test	MIB_ModeAttributes[edi],16
	jnz	@@Graphics
	mov	VideoBase,0b8000h
	mov	VideoModeFlags,8+1	;Flag text mode, attrib & char.
	mov	VideoPixelWidth,2
	jmp	@@SetBase
	;
@@Graphics:	mov	VideoBase,0a0000h
;
;Need to know the size of a pixel.
;
@@SetPixSize:	movzx	eax,MIB_BitsPerPixel[edi]
	add	eax,7
	shr	eax,3
	mov	VideoPixelWidth,eax
;
;Need to know what format we're dealing with.
;
	mov	ebx,0
	movzx	eax,MIB_BitsPerPixel[edi]
	cmp	eax,8		;8-bit?
	jz	@@GotBits
	jc	@@GotBits
	inc	ebx
	cmp	eax,15		;15 bit? (5-5-5)
	jz	@@GotBits
	jc	@@GotBits
	inc	ebx
	cmp	eax,16		;16 bit? (5-5-6)
	jz	@@GotBits
	inc	ebx
	cmp	eax,24		;24 bit? (true colour)
	jz	@@GotBits
	jc	@@GotBits
	inc	ebx
	cmp	eax,32		;32 bit? (24 bit plus padding)
	jz	@@GotBits
	jc	@@GotBits
	xor	ebx,ebx
@@GotBits:	mov	VideoModeFlags,ebx
;
;Set the video selector base according to mode type.
;
@@SetBase:	mov	bx,VideoSelector
	mov	edx,VideoBase
	mov	ecx,65535
	sys	SetSelDet32
;
;Now set the palette again.
;
@@SetPal:	pop	ebp
	mov	edi,offset HardwarePalette+(256*3)
	mov	ecx,256*3
	mov	al,-1
	rep	stosb
	mov	esi,offset HardwarePalette
	xor	eax,eax
	mov	ecx,256
	call	VideoSetPalette
	;
	mov	eax,VideoMode
	jmp	@@exit
;
;An error occured so return failure status.
;
@@9:	xor	eax,eax
	mov	VideoMode,eax
;
@@exit:	call	MouseOnCall
	popm	ebx,ecx,edx,esi,edi,ebp
	ret
VideoSetMode	endp


	efile
	end

