
	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Blit a bitmap to/from video memory.
;
;On Entry:
;
;C style stack with following parameters,
;
;flags	- flags, bit significant if set.
;	0 - blit video to system memory.
;	1 - OR bitmap on, chroma value is used.
;bitmap	- pointer to bitmap.
;xcoord	- X co-ord to blit at.
;ycoord	- Y co-ord to blit at.
;xoff	- X offset within bitmap.
;yoff	- Y offset within bitmap.
;wide	- width to blit.
;depth	- depth to blit.
;chroma	- chroma value to use when OR'ing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;Note: If bit 0 of flags is set all other flag bits are ignored.
;
VideoBlitBitmap:
_VideoBlitBitmap proc syscall Flags:dword, Bitmap:dword, XCoord:dword, \
	YCoord:dword, XOff:dword, YOff:dword, Wide:dword, \
	Depth:dword, Chroma:dword
	mov	eax,Flags
	pop	ebp
	test	eax,1
	jnz	VideoGetBitmap
	jmp	VideoPutBitmap
_VideoBlitBitmap endp


;-------------------------------------------------------------------------
;
;Blit a bitmap to video memory.
;
;On Entry:
;
;C style stack with following parameters,
;
;flags	- flags, bit significant if set.
;	0 - blit video to system memory.
;	1 - OR bitmap on, chroma value is used.
;bitmap	- pointer to bitmap.
;xcoord	- X co-ord to blit at.
;ycoord	- Y co-ord to blit at.
;xoff	- X offset within bitmap.
;yoff	- Y offset within bitmap.
;wide	- width to blit.
;depth	- depth to blit.
;chroma	- chroma value to use when OR'ing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;Note: If bit 0 of flags is set all other flag bits are ignored.
;
VideoPutBitmap	proc syscall Flags:dword, Bitmap:dword, XCoord:dword, \
	YCoord:dword, XOff:dword, YOff:dword, Wide:dword, \
	Depth:dword, Chroma:dword
	local	@@Source:dword, @@Dest:dword, @@MouseHandle:dword, \
	@@YBreak:dword, @@Depth:dword
	pushad
	mov	eax,Chroma
	mov	_VBB_Chroma,eax
;
;Check for maximum width/depth
;
	mov	esi,Bitmap
	cmp	Wide,-1
	jnz	@@8
	mov	eax,BM_Wide[esi]
	mov	Wide,eax
@@8:	cmp	Depth,-1
	jnz	@@9
	mov	eax,BM_Depth[esi]
	mov	Depth,eax
;
;Check for daft width/depth.
;
@@9:	cmp	Wide,0
	js	@@BlitFinished
	jz	@@BlitFinished
	cmp	Depth,0
	js	@@BlitFinished
	jz	@@BlitFinished
;
;Clip offsets/width/depth to fit bitmap.
;
	cmp	XOff,0
	jns	@@0
	mov	eax,XOff
	neg	eax
	sub	Wide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	add	XCoord,eax
	mov	XOff,0
;
@@0:	cmp	YOff,0
	jns	@@1
	mov	eax,YOff
	neg	eax
	sub	Depth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	add	YCoord,eax
	mov	YOff,0
;
@@1:	mov	eax,XOff
	add	eax,Wide
	cmp	eax,BM_Wide[esi]
	jl	@@2
	sub	eax,BM_Wide[esi]
	sub	Wide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
@@2:	mov	eax,YOff
	add	eax,Depth
	cmp	eax,BM_Depth[esi]
	jl	@@3
	sub	eax,BM_Depth[esi]
	sub	Depth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
;Clip coords/width/depth to fit the screen.
;
@@3:	cmp	XCoord,0
	jns	@@4
	mov	eax,XCoord
	neg	eax
	sub	Wide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	add	XOff,eax
	mov	XCoord,0
;
@@4:	cmp	YCoord,0
	jns	@@5
	mov	eax,YCoord
	neg	eax
	sub	Depth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	add	YOff,eax
	mov	YCoord,0
;
@@5:	mov	eax,XCoord
	add	eax,Wide
	cmp	eax,VideoXResolution
	jl	@@6
	sub	eax,VideoXResolution
	sub	Wide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
@@6:	mov	eax,YCoord
	add	eax,Depth
	cmp	eax,VideoYResolution
	jl	@@7
	sub	eax,VideoYResolution
	sub	Depth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
;Now work out the source address and width.
;
@@7:	mov	eax,Bitmap
	add	eax,size BM
	mov	@@Source,eax
	mov	eax,YOff
	mul	BM_PWide[esi]
	mul	BM_Wide[esi]
	add	@@Source,eax
	mov	eax,XOff
	mul	BM_PWide[esi]
	add	@@Source,eax
;
;Work out the destination address.
;
	mov	eax,YCoord
	mul	VideoHardwareWidth
	mov	@@Dest,eax
	mov	eax,XCoord
	mul	VideoPixelWidth
	add	@@Dest,eax
;
;Remove mouse if installed.
;
	push	ebp
	mov	eax,0		;setting it.
	mov	ecx,XCoord
	mov	ebx,YCoord
	mov	edx,Wide
	mov	ebp,Depth
	call	MouseExcludeCall
	pop	ebp
	mov	@@MouseHandle,ebx
;
;Start copying.
;
	mov	edi,@@Dest
	mov	esi,@@Source
@@CarryON:	bank	edi		;make sure we start in the right bank.
;
;work out line we split on in this bank.
;
	mov	eax,edi		;get current destination.
	mov	ax,-1		;and extremity in low word.
	mov	ecx,VideoHardwareWidth	;width of each line.
	xor	edx,edx
	div	ecx		;get last Y coord.
	mov	@@YBreak,eax		;line on which to break.
	cmp	eax,YCoord
	jnz	@@GetDepth
	mov	@@Depth,1
	jmp	@@ok0
;
;Now work out how much of this block we can do as normal.
;
@@GetDepth:	sub	eax,YCoord		;get distance from current position.
	jz	@@Force
	js	@@Force
	mov	@@Depth,eax		;and set depth.
	cmp	eax,Depth		;>than remaining depth?
	jc	@@ok0
@@Force:	mov	eax,Depth		;get whats left.
	mov	@@Depth,eax
@@ok0:	cmp	@@Depth,0
	jz	@@BlitDone
	js	@@BlitDone
	mov	ebx,YCoord
	xor	ebx,@@YBreak
	mov	ecx,Wide
	mov	edx,@@Depth
;
;Select appropriate method.
;
	test	Flags,2
	jz	@@PUT
;
;Now select appropriate blit routine.
;
@@PUTOR:	push	es
	push	ebp
	mov	es,VideoSelector
	mov	ebp,Bitmap
	mov	eax,VideoModeFlags
	and	eax,15
	call	d[PutOrRoutines+eax*4]
	pop	ebp
	pop	es
	jmp	@@BlitNext
;
;Now select appropriate blit routine.
;
@@PUT:	push	es
	push	ebp
	mov	es,VideoSelector
	mov	ebp,Bitmap
	mov	eax,VideoModeFlags
	and	eax,15
	call	d[PutRoutines+eax*4]
	pop	ebp
	pop	es
;
;Move onto next section.
;
@@BlitNext:	mov	eax,@@Depth
	add	YCoord,eax
	sub	Depth,eax
	jnz	@@CarryON
;
;Common exits for all of them.
;
@@BlitDone:	mov	ebx,@@MouseHandle
	mov	eax,1
	call	MouseExcludeCall
;
;Common exit for not on screen.
;
@@BlitFinished:
	popad
	ret
VideoPutBitmap	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap from video memory.
;
;On Entry:
;
;C style stack with following parameters,
;
;flags	- flags, bit significant if set.
;	0 - blit video to system memory.
;	1 - OR bitmap on, chroma value is used.
;bitmap	- pointer to bitmap.
;xcoord	- X co-ord to blit at.
;ycoord	- Y co-ord to blit at.
;xoff	- X offset within bitmap.
;yoff	- Y offset within bitmap.
;wide	- width to blit.
;depth	- depth to blit.
;chroma	- chroma value to use when OR'ing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;Note: If bit 0 of flags is set all other flag bits are ignored.
;
VideoGetBitmap	proc syscall Flags:dword, Bitmap:dword, XCoord:dword, \
	YCoord:dword, XOff:dword, YOff:dword, Wide:dword, \
	Depth:dword, Chroma:dword
	local	@@Source:dword, @@Dest:dword, @@MouseHandle:dword, \
	@@YBreak:dword, @@Depth:dword
	pushad
;
;Check for daft width/depth.
;
	cmp	Wide,0
	js	@@BlitFinished
	jz	@@BlitFinished
	cmp	Depth,0
	js	@@BlitFinished
	jz	@@BlitFinished
;
;Clip offsets/width/depth to fit bitmap.
;
	mov	esi,Bitmap
	cmp	XOff,0
	jns	@@0
	mov	eax,XOff
	neg	eax
	sub	Wide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	mov	XOff,0
;
@@0:	cmp	YOff,0
	jns	@@1
	mov	eax,YOff
	neg	eax
	sub	Depth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	mov	YOff,0
;
@@1:	mov	eax,XOff
	add	eax,Wide
	cmp	eax,BM_Wide[esi]
	jl	@@2
	sub	eax,BM_Wide[esi]
	sub	Wide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
@@2:	mov	eax,YOff
	add	eax,Depth
	cmp	eax,BM_Depth[esi]
	jl	@@3
	sub	eax,BM_Depth[esi]
	sub	Depth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
;Clip coords/width/depth to fit the screen.
;
@@3:	cmp	XCoord,0
	jns	@@4
	mov	eax,XCoord
	neg	eax
	sub	Wide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	add	XOff,eax
	mov	XCoord,0
;
@@4:	cmp	YCoord,0
	jns	@@5
	mov	eax,YCoord
	neg	eax
	sub	Depth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	add	YOff,eax
	mov	YCoord,0
;
@@5:	mov	eax,XCoord
	add	eax,Wide
	cmp	eax,VideoXResolution
	jl	@@6
	sub	eax,VideoXResolution
	sub	Wide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
@@6:	mov	eax,YCoord
	add	eax,Depth
	cmp	eax,VideoYResolution
	jl	@@7
	sub	eax,VideoYResolution
	sub	Depth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
;Now work out the destination address and width.
;
@@7:	mov	eax,Bitmap
	add	eax,size BM
	mov	@@Dest,eax
	mov	eax,YOff
	mul	BM_PWide[esi]
	mul	BM_Wide[esi]
	add	@@Dest,eax
	mov	eax,XOff
	mul	BM_PWide[esi]
	add	@@Dest,eax
;
;Work out the source address.
;
	mov	eax,YCoord
	mul	VideoHardwareWidth
	mov	@@Source,eax
	mov	eax,XCoord
	mul	VideoPixelWidth
	add	@@Source,eax
;
;Remove mouse if installed.
;
	push	ebp
	mov	eax,0		;setting it.
	mov	ecx,XCoord
	mov	ebx,YCoord
	mov	edx,Wide
	mov	ebp,Depth
	call	MouseExcludeCall
	pop	ebp
	mov	@@MouseHandle,ebx
;
;Start copying.
;
	mov	edi,@@Dest
	mov	esi,@@Source
@@CarryON:	bank	esi		;make sure we start in the right bank.
;
;work out line we split on in this bank.
;
	mov	eax,esi		;get current destination.
	mov	ax,-1		;and extremity in low word.
	mov	ecx,VideoHardwareWidth	;width of each line.
	xor	edx,edx
	div	ecx		;get last Y coord.
	mov	@@YBreak,eax		;line on which to break.
	cmp	eax,YCoord
	jnz	@@GetDepth
	mov	@@Depth,1
	jmp	@@ok0
;
;Now work out how much of this block we can do as normal.
;
@@GetDepth:	sub	eax,YCoord		;get distance from current position.
	jz	@@Force
	js	@@Force
	mov	@@Depth,eax		;and set depth.
	cmp	eax,Depth		;>than remaining depth?
	jc	@@ok0
@@Force:	mov	eax,Depth		;get whats left.
	mov	@@Depth,eax
@@ok0:	cmp	@@Depth,0
	jz	@@BlitDone
	js	@@BlitDone
	mov	ebx,YCoord
	xor	ebx,@@YBreak
	mov	ecx,Wide
	mov	edx,@@Depth
;
;Now select appropriate blit routine.
;
	push	es
	push	ebp
	mov	es,VideoSelector
	mov	ebp,Bitmap
	mov	eax,VideoModeFlags
	and	eax,15
	call	d[GetRoutines+eax*4]
	pop	ebp
	pop	es
;
;Move onto next section.
;
@@BlitNext:	mov	eax,@@Depth
	add	YCoord,eax
	sub	Depth,eax
	jnz	@@CarryON
;
;Common exits for all of them.
;
@@BlitDone:	mov	ebx,@@MouseHandle
	mov	eax,1
	call	MouseExcludeCall
;
;Common exit for not on screen.
;
@@BlitFinished:
	popad
	ret
VideoGetBitmap	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 256 colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutBitmap256	proc	near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
	rep_movsb
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b256:	pushm	ecx,esi,edi
	sub	edi,d[CurrentBankBig]
	movsx	eax,di
	neg	eax
	cmp	eax,ecx
	jc	@@b256_1
	mov	eax,ecx
@@b256_1:	push	ecx
	mov	ecx,eax
	rep_movsb
	pop	ecx
	sub	ecx,eax
	jz	@@b256_2
	;
	add	edi,d[CurrentBankBig]
	bank	edi
	sub	edi,d[CurrentBankBig]
	rep_movsb
	;
@@b256_2:	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;32k colour bitmap.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	mov	ebx,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
;	call	SearchRGB
	stosb
	loop	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	mov	ebx,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
;	call	SearchRGB
	stosb
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;64k colour bitmap.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	mov	ebx,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
;	call	SearchRGB
	stosb
	dec	ecx
	jnz	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	mov	ebx,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
;	call	SearchRGB
	stosb
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;16m colour bitmap.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi

	or	ah,al
	shr	eax,8
	or	al,ah

;	call	SearchRGB

	stosb
	dec	ecx
	jnz	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsd
	dec	esi

	or	ah,al
	shr	eax,8
	or	al,ah

;	call	SearchRGB

	stosb
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
;
@@9:	ret
vPutBitmap256	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 32k colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutBitmap32k	proc	near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	xor	eax,eax
	lodsb
	mov	eax,d[HardwarePalette+eax+eax*2]
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	ebx,eax
	shl	ebx,2+8		;Convert 0-63 value to 0-255 value.
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	dec	ecx
	jnz	@@256_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	xor	eax,eax
	lodsb
	mov	eax,d[HardwarePalette+eax+eax*2]
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	ebx,eax
	shl	ebx,2+8		;Convert 0-63 value to 0-255 value.
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;32k colour version.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
	rep_movsw
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b32k:	pushm	ecx,esi,edi
	sub	edi,d[CurrentBankBig]
	movsx	eax,di
	neg	eax
	shr	eax,1
	cmp	eax,ecx
	jc	@@b32k_0
	mov	eax,ecx
@@b32k_0:	push	ecx
	mov	ecx,eax
	rep_movsw
	pop	ecx
	sub	ecx,eax
	jz	@@b32k_1
	add	edi,d[CurrentBankBig]
	bank	edi
	sub	edi,d[CurrentBankBig]
	rep_movsw
@@b32k_1:	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;64k colour version.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	movzx	ebx,ax
	xor	eax,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	ebx,6
	shld	eax,ebx,5
	stosw
	dec	ecx
	jnz	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	movzx	ebx,ax
	xor	eax,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	ebx,6
	shld	eax,ebx,5
	stosw
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;16m colour version.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	dec	ecx
	jnz	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsd
	dec	esi
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
;
@@9:	ret
vPutBitmap32k	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 256 colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutBitmap64k	proc	near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	xor	eax,eax
	lodsb
	mov	eax,d[HardwarePalette+eax+eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	ebx,eax
	shl	ebx,8
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	dec	ecx
	jnz	@@256_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	xor	eax,eax
	lodsb
	mov	eax,d[HardwarePalette+eax+eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	ebx,eax
	shl	ebx,8
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;32k colour version.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	movzx	ebx,ax
	xor	eax,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,1
	shl	ebx,5
	shld	eax,ebx,5
	stosw
	dec	ecx
	jnz	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	movzx	ebx,ax
	xor	eax,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,1
	shl	ebx,5
	shld	eax,ebx,5
	stosw
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;64k colour version.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
	rep_movsw
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b64k:	pushm	ecx,esi,edi
	sub	edi,d[CurrentBankBig]
	movsx	eax,di
	neg	eax
	shr	eax,1
	cmp	eax,ecx
	jc	@@b64k_0
	mov	eax,ecx
@@b64k_0:	push	ecx
	mov	ecx,eax
	rep_movsw
	pop	ecx
	sub	ecx,eax
	jz	@@b64k_1
	add	edi,d[CurrentBankBig]
	bank	edi
	sub	edi,d[CurrentBankBig]
	rep_movsw
@@b64k_1:	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;16m colour version.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	dec	ecx
	jnz	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsd
	dec	esi
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
;
@@9:	ret
vPutBitmap64k	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 256 colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutBitmap16m	proc	near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	lodsb
	movzx	eax,al
	mov	eax,d[HardwarePalette+eax][eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	stosw
	shr	eax,16
	stosb
	dec	ecx
	jnz	@@256_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	lodsb
	movzx	eax,al
	mov	eax,d[HardwarePalette+eax][eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	bh,3
@@b256_1:	bank	edi
	sub	edi,d[CurrentBankBig]
	stosb
	shr	eax,8
	add	edi,d[CurrentBankBig]
	dec	bh
	jnz	@@b256_1
	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;32k colour bitmap.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	movzx	ebx,ax
	shl	ebx,1+16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	stosw
	shr	eax,16
	stosb
	dec	ecx
	jnz	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	lodsw
	movzx	ebx,ax
	shl	ebx,1+16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	mov	bl,3
@@b32k_1:	bank	edi
	sub	edi,d[CurrentBankBig]
	stosb
	shr	eax,8
	add	edi,d[CurrentBankBig]
	dec	bl
	jnz	@@b32k_1
	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;64k colour bitmap.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	movzx	ebx,ax
	shl	ebx,16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
	stosw
	shr	eax,16
	stosb
	dec	ecx
	jnz	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	lodsw
	movzx	ebx,ax
	shl	ebx,16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
	mov	bl,3
@@b64k_1:	bank	edi
	sub	edi,d[CurrentBankBig]
	stosb
	shr	eax,8
	add	edi,d[CurrentBankBig]
	dec	bl
	jnz	@@b64k_1
	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;16m colour bitmap.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	lea	ecx,[ecx][ecx*2]
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
	rep_movsb
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b16m:	pushm	ecx,esi,edi
	lea	ecx,[ecx+ecx*2]
@@b16m_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	movsb
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
;
@@9:	ret
vPutBitmap16m	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 16m 32-bit colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutBitmap16m32 proc	near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	lodsb
	movzx	eax,al
	mov	eax,d[HardwarePalette+eax][eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	stosd
	dec	ecx
	jnz	@@256_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsb
	movzx	eax,al
	mov	eax,d[HardwarePalette+eax][eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	stosd
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;32k colour bitmap.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	movzx	ebx,ax
	shl	ebx,1+16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	stosd
	dec	ecx
	jnz	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	movzx	ebx,ax
	shl	ebx,1+16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	stosd
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;64k colour bitmap.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	movzx	ebx,ax
	shl	ebx,16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
	stosd
	dec	ecx
	jnz	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	movzx	ebx,ax
	shl	ebx,16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
	stosd
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
;
;16m colour bitmap.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	stosd
	dec	ecx
	jnz	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsd
	dec	esi
	stosd
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
;
@@9:	ret
vPutBitmap16m32 endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in text mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutBitmapText proc near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,1
	jz	@@chars
	dec	eax
	jz	@@chratt
	jmp	@@9
;
;Just character data.
;
@@chars:	sub	edi,d[CurrentBankBig]
@@chars_0:	pushm	ecx,esi,edi
@@chars_1:	movsb
	inc	edi
	dec	ecx
	jnz	@@chars_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@chars_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
;
;Character and attribute data.
;
@@chratt:	sub	edi,d[CurrentBankBig]
@@chratt_0:	pushm	ecx,esi,edi
	rep_movsw
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@chratt_0
	add	edi,d[CurrentBankBig]
;
@@9:	ret
vPutBitmapText endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 256 colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutORBitmap256 proc	near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	lodsb
	cmp	al,b[_VBB_Chroma]
	jz	@@256_3
	stosb
@@256_2:	dec	ecx
	jnz	@@256_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@256_3:	inc	edi
	jmp	@@256_2
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsb
	cmp	al,b[_VBB_Chroma]
	jz	@@b256_3
	stosb
@@b256_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b256_3:	inc	edi
	jmp	@@b256_2
;
;32k colour bitmap.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@32k_3
	mov	ebx,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
;	call	SearchRGB
	stosb
@@32k_2:	dec	ecx
	jnz	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@32k_3:	inc	edi
	jmp	@@32k_2
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b32k_3
	add	edi,d[CurrentBankBig]
	mov	ebx,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
;	call	SearchRGB
	stosb
@@b32k_2:	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b32k_3:	inc	edi
	jmp	@@b32k_2
;
;64k colour bitmap.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@64k_3
	mov	ebx,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
;	call	SearchRGB
	stosb
@@64k_2:	dec	ecx
	jnz	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@64k_3:	inc	edi
	jmp	@@64k_2
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b64k_3
	add	edi,d[CurrentBankBig]
	mov	ebx,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
;	call	SearchRGB
	stosb
@@b64k_2:	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b64k_3:	inc	edi
	jmp	@@b64k_2
;
;16m colour bitmap.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@16m_3
;	call	SearchRGB
	stosb
@@16m_2:	dec	ecx
	jnz	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@16m_3:	inc	edi
	jmp	@@16m_2
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@b16m_3
;	call	SearchRGB
	stosb
@@b16m_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b16m_3:	inc	edi
	jmp	@@b16m_2
;
@@9:	ret
vPutORBitmap256 endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 32k colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutORBitmap32k proc	near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	xor	eax,eax
	lodsb
	cmp	al,b[_VBB_Chroma]
	jz	@@256_3
	mov	eax,d[HardwarePalette+eax+eax*2]
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	ebx,eax
	shl	ebx,2+8		;Convert 0-63 value to 0-255 value.
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	stosw
@@256_2:	dec	ecx
	jnz	@@256_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@256_3:	add	edi,2
	jmp	@@256_2
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	xor	eax,eax
	lodsb
	cmp	al,b[_VBB_Chroma]
	jz	@@b256_3
	mov	eax,d[HardwarePalette+eax+eax*2]
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	ebx,eax
	shl	ebx,2+8		;Convert 0-63 value to 0-255 value.
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	stosw
@@b256_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b256_3:	add	edi,2
	jmp	@@b256_2
;
;32k colour version.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@32k_3
	stosw
@@32k_2:	dec	ecx
	jnz	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@32k_3:	add	edi,2
	jmp	@@32k_2
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b32k_3
	stosw
@@b32k_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b32k_3:	add	edi,2
	jmp	@@b32k_2
;
;64k colour version.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@64k_3
	movzx	ebx,ax
	xor	eax,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	ebx,6
	shld	eax,ebx,5
	stosw
@@64k_2:	dec	ecx
	jnz	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@64k_3:	add	edi,2
	jmp	@@64k_2
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b64k_3
	movzx	ebx,ax
	xor	eax,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	ebx,6
	shld	eax,ebx,5
	stosw
@@b64k_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b64k_3:	add	edi,2
	jmp	@@b64k_2
;
;16m colour version.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@16m_3
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	stosw
@@16m_2:	dec	ecx
	jnz	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@16m_3:	add	edi,2
	jmp	@@16m_2
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@b16m_3
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	stosw
@@b16m_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b16m_3:	add	edi,2
	jmp	@@b16m_2
;
@@9:	ret
vPutORBitmap32k endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 256 colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutORBitmap64k proc	near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	xor	eax,eax
	lodsb
	cmp	al,b[_VBB_Chroma]
	jz	@@256_3
	mov	eax,d[HardwarePalette+eax+eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	ebx,eax
	shl	ebx,8
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	stosw
@@256_2:	dec	ecx
	jnz	@@256_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@256_3:	add	edi,2
	jmp	@@256_2
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	xor	eax,eax
	lodsb
	cmp	al,b[_VBB_Chroma]
	jz	@@b256_3
	mov	eax,d[HardwarePalette+eax+eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	ebx,eax
	shl	ebx,8
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	stosw
@@b256_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b256_3:	add	edi,2
	jmp	@@b256_2
;
;32k colour version.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@32k_3
	movzx	ebx,ax
	xor	eax,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,1
	shl	ebx,5
	shld	eax,ebx,5
	stosw
@@32k_2:	dec	ecx
	jnz	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@32k_3:	add	edi,2
	jmp	@@32k_2
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b32k_3
	movzx	ebx,ax
	xor	eax,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,1
	shl	ebx,5
	shld	eax,ebx,5
	stosw
@@b32k_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b32k_3:	add	edi,2
	jmp	@@b32k_2
;
;64k colour version.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@64k_3
	stosw
@@64k_2:	dec	ecx
	jnz	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@64k_3:	add	edi,2
	jmp	@@64k_2
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b64k_3
	stosw
@@b64k_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b64k_3:	add	edi,2
	jmp	@@b64k_2
;
;16m colour version.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@16m_3
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	stosw
@@16m_2:	dec	ecx
	jnz	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@16m_3:	add	edi,2
	jmp	@@16m_2
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@b16m_3
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	stosw
@@b16m_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b16m_3:	add	edi,2
	jmp	@@b16m_2
;
@@9:	ret
vPutORBitmap64k endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 256 colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutORBitmap16m proc	near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	lodsb
	movzx	eax,al
	cmp	al,b[_VBB_Chroma]
	jz	@@256_3
	mov	eax,d[HardwarePalette+eax][eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	stosw
	shr	eax,16
	stosb
@@256_2:	dec	ecx
	jnz	@@256_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@256_3:	add	edi,3
	jmp	@@256_2
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	lodsb
	cmp	al,b[_VBB_Chroma]
	jz	@@b256_3
	movzx	eax,al
	mov	eax,d[HardwarePalette+eax][eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	bl,3
@@b256_4:	bank	edi
	sub	edi,d[CurrentBankBig]
	stosb
	shr	eax,8
	add	edi,d[CurrentBankBig]
	dec	bl
	jnz	@@b256_4
@@b256_2:	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b256_3:	add	edi,3
	jmp	@@b256_2
;
;32k colour bitmap.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@32k_3
	movzx	ebx,ax
	shl	ebx,1+16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	stosw
	shr	eax,16
	stosb
@@32k_2:	dec	ecx
	jnz	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@32k_3:	add	edi,3
	jmp	@@32k_2
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b32k_3
	movzx	ebx,ax
	shl	ebx,1+16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	mov	bl,3
@@b32k_4:	bank	edi
	sub	edi,d[CurrentBankBig]
	stosb
	shr	eax,8
	add	edi,d[CurrentBankBig]
	dec	bl
	jnz	@@b32k_4
@@b32k_2:	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b32k_3:	add	edi,3
	jmp	@@b32k_2
;
;64k colour bitmap.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@64k_3
	movzx	ebx,ax
	shl	ebx,16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
	stosw
	shr	eax,16
	stosb
@@64k_2:	dec	ecx
	jnz	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@64k_3:	add	edi,3
	jmp	@@64k_2
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b64k_3
	movzx	ebx,ax
	shl	ebx,16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
	mov	bl,3
@@b64k_4:	bank	edi
	sub	edi,d[CurrentBankBig]
	stosb
	shr	eax,8
	add	edi,d[CurrentBankBig]
	dec	bl
	jnz	@@b64k_4
@@b64k_2:	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b64k_3:	add	edi,3
	jmp	@@b64k_2
;
;16m colour bitmap.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	lea	ecx,[ecx][ecx*2]
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@16m_3
	stosw
	shr	eax,16
	stosb
@@16m_2:	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@16m_3:	add	edi,3
	jmp	@@16m_2
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@b16m_3
	mov	bl,3
@@b16m_4:	bank	edi
	sub	edi,d[CurrentBankBig]
	stosb
	shr	eax,8
	add	edi,d[CurrentBankBig]
	dec	bl
	jnz	@@b16m_4
@@b16m_2:	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b16m_3:	add	edi,3
	jmp	@@b16m_2
;
@@9:	ret
vPutORBitmap16m endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 16m 32-bit colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutORBitmap16m32 proc near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,15
	jz	@@256
	dec	eax
	jz	@@32k
	dec	eax
	jz	@@64k
	dec	eax
	jz	@@16m
	jmp	@@9
;
;256 colour bitmap.
;
@@256:	or	ebx,ebx
	jz	@@b256
	sub	edi,d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	lodsb
	movzx	eax,al
	cmp	al,b[_VBB_Chroma]
	jz	@@256_3
	mov	eax,d[HardwarePalette+eax][eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	stosd
@@256_2:	dec	ecx
	jnz	@@256_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@256_3:	add	edi,4
	jmp	@@256_2
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsb
	movzx	eax,al
	cmp	al,b[_VBB_Chroma]
	jz	@@b256_3
	mov	eax,d[HardwarePalette+eax][eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	stosd
@@b256_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b256_3:	add	edi,4
	jmp	@@b256_2
;
;32k colour bitmap.
;
@@32k:	or	ebx,ebx
	jz	@@b32k
	sub	edi,d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@32k_3
	movzx	ebx,ax
	shl	ebx,1+16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	stosd
@@32k_2:	dec	ecx
	jnz	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@32k_3:	add	edi,4
	jmp	@@32k_2
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b32k_3
	movzx	ebx,ax
	shl	ebx,1+16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,3
	stosd
@@b32k_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b32k_3:	add	edi,4
	jmp	@@b32k_2
;
;64k colour bitmap.
;
@@64k:	or	ebx,ebx
	jz	@@b64k
	sub	edi,d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@64k_3
	movzx	ebx,ax
	shl	ebx,16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
	stosd
@@64k_2:	dec	ecx
	jnz	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@64k_3:	add	edi,4
	jmp	@@64k_2
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@b64k_3
	movzx	ebx,ax
	shl	ebx,16
	xor	eax,eax
	shld	eax,ebx,5
	shl	eax,3
	shl	ebx,5
	shld	eax,ebx,6
	shl	eax,2
	shl	ebx,6
	shld	eax,ebx,5
	shl	eax,3
	stosd
@@b64k_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b64k_3:	add	edi,4
	jmp	@@b64k_2
;
;16m colour bitmap.
;
@@16m:	or	ebx,ebx
	jz	@@b16m
	sub	edi,d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@16m_3
	stosd
@@16m_2:	dec	ecx
	jnz	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@16m_3:	add	edi,4
	jmp	@@16m_2
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	bank	edi
	sub	edi,d[CurrentBankBig]
	lodsd
	dec	esi
	and	eax,0ffffffh
	cmp	eax,d[_VBB_Chroma]
	jz	@@b16m_3
	stosd
@@b16m_2:	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	jmp	@@9
@@b16m_3:	add	edi,4
	jmp	@@b16m_2
;
@@9:	ret
vPutORBitmap16m32 endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in text mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vPutORBitmapText proc near
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,BM_Flags[ebp]
	xchg	ebp,eax
	and	eax,1
	jz	@@chars
	dec	eax
	jz	@@chratt
	jmp	@@9
;
;Just character data.
;
@@chars:	sub	edi,d[CurrentBankBig]
@@chars_0:	pushm	ecx,esi,edi
@@chars_1:	lodsb
	cmp	al,b[_VBB_Chroma]
	jz	@@chars_3
	stosb
	inc	edi
@@chars_2:	dec	ecx
	jnz	@@chars_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@chars_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@chars_3:	add	edi,2
	jmp	@@chars_2
;
;Character and attribute data.
;
@@chratt:	sub	edi,d[CurrentBankBig]
@@chratt_0:	pushm	ecx,esi,edi
@@chratt_1:	lodsw
	cmp	ax,w[_VBB_Chroma]
	jz	@@chratt_3
	stosw
@@chratt_2:	dec	ecx
	jnz	@@chratt_1
	popm	ecx,esi,edi
	add	esi,ebp
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@chratt_0
	add	edi,d[CurrentBankBig]
	jmp	@@9
@@chratt_3:	add	edi,2
	jmp	@@chratt_2
;
@@9:	ret
vPutORBitmapText endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 256 colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vGetBitmap256	proc	near
	mov	BM_Flags[ebp],0
	mov	BM_PWide[ebp],1
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,eax
	;
	assume ds:nothing
	pushm	ds,es
	popm	es,ds
	;
	or	ebx,ebx
	jz	@@b256
	sub	esi,ss:d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
	rep_movsb
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	esi,ss:d[CurrentBankBig]
	jmp	@@9
;
@@b256:	pushm	ecx,esi,edi
@@b256_0:	bank	esi
	sub	esi,ss:d[CurrentBankBig]
	movsb
	add	esi,ss:d[CurrentBankBig]
	dec	ecx
	jnz	@@b256_0
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
;
@@9:	pushm	ds,es
	popm	es,ds
	assume ds:DGROUP
	ret
vGetBitmap256	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 32k colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vGetBitmap32k	proc	near
	mov	BM_Flags[ebp],1
	mov	BM_PWide[ebp],2
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,eax
	;
	assume ds:nothing
	pushm	ds,es
	popm	es,ds
	;
	or	ebx,ebx
	jz	@@b32k
	sub	esi,ss:d[CurrentBankBig]
@@32k_0:	pushm	ecx,esi,edi
	rep_movsw
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
	dec	edx
	jnz	@@32k_0
	add	esi,ss:d[CurrentBankBig]
	jmp	@@9
;
@@b32k:	pushm	ecx,esi,edi
@@b32k_0:	bank	esi
	sub	esi,ss:d[CurrentBankBig]
	movsw
	add	esi,ss:d[CurrentBankBig]
	dec	ecx
	jnz	@@b32k_0
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
;
@@9:	pushm	ds,es
	popm	es,ds
	assume ds:DGROUP
	ret
vGetBitmap32k	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 256 colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vGetBitmap64k	proc	near
	mov	BM_Flags[ebp],2
	mov	BM_PWide[ebp],2
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,eax
	;
	assume ds:nothing
	pushm	ds,es
	popm	es,ds
	;
	or	ebx,ebx
	jz	@@b64k
	sub	esi,ss:d[CurrentBankBig]
@@64k_0:	pushm	ecx,esi,edi
	rep_movsw
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
	dec	edx
	jnz	@@64k_0
	add	esi,ss:d[CurrentBankBig]
	jmp	@@9
;
@@b64k:	pushm	ecx,esi,edi
@@b64k_0:	bank	esi
	sub	esi,ss:d[CurrentBankBig]
	movsw
	add	esi,ss:d[CurrentBankBig]
	dec	ecx
	jnz	@@b64k_0
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
;
@@9:	pushm	ds,es
	popm	es,ds
	assume ds:DGROUP
	ret
vGetBitmap64k	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 256 colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vGetBitmap16m	proc	near
	mov	BM_Flags[ebp],3
	mov	BM_PWide[ebp],3
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,eax
	;
	assume ds:nothing
	pushm	ds,es
	popm	es,ds
	;
	or	ebx,ebx
	jz	@@b16m
	lea	ecx,[ecx][ecx*2]
	sub	esi,ss:d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
	rep_movsb
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	esi,ss:d[CurrentBankBig]
	jmp	@@9
;
@@b16m:	pushm	ecx,esi,edi
	lea	ecx,[ecx+ecx*2]
@@b16m_0:	bank	esi
	sub	esi,ss:d[CurrentBankBig]
	movsb
	add	esi,ss:d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
;
@@9:	pushm	ds,es
	popm	es,ds
	assume ds:DGROUP
	ret
vGetBitmap16m	endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in 16m 32-bit colour mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vGetBitmap16m32 proc	near
	mov	BM_Flags[ebp],3
	mov	BM_PWide[ebp],3
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,eax
	;
	assume ds:nothing
	pushm	ds,es
	popm	es,ds
	;
	or	ebx,ebx
	jz	@@b16m
	sub	esi,ss:d[CurrentBankBig]
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	stosw
	shr	eax,16
	stosb
	dec	ecx
	jnz	@@16m_1
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
	dec	edx
	jnz	@@16m_0
	add	esi,ss:d[CurrentBankBig]
	jmp	@@9
;
@@b16m:	pushm	ecx,esi,edi
@@b16m_0:	bank	esi
	sub	esi,ss:d[CurrentBankBig]
	lodsd
	stosw
	shr	eax,16
	stosb
	add	esi,ss:d[CurrentBankBig]
	dec	ecx
	jnz	@@b16m_0
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
;
@@9:	pushm	ds,es
	popm	es,ds
	assume ds:DGROUP
	ret
vGetBitmap16m32 endp


;-------------------------------------------------------------------------
;
;Blit a bitmap in text mode.
;
;On Entry:
;
;EBX	- state, 0=broken, !=0=clean.
;ECX	- width.
;EDX	- depth.
;ESI	- source.
;EDI	- destination.
;EBP	- Bitmap (BM) header.
;
vGetBitmapText	proc	near
	mov	BM_Flags[ebp],8+1
	mov	BM_PWide[ebp],2
	push	edx
	mov	eax,BM_Wide[ebp]
	mul	BM_PWide[ebp]
	pop	edx
	mov	ebp,eax
	;
	assume ds:nothing
	pushm	ds,es
	popm	es,ds
	;
	sub	esi,ss:d[CurrentBankBig]
@@256_0:	pushm	ecx,esi,edi
	rep_movsw
	popm	ecx,esi,edi
	add	edi,ebp
	add	esi,ss:VideoHardwareWidth
	dec	edx
	jnz	@@256_0
	add	esi,ss:d[CurrentBankBig]
	jmp	@@9
;
@@9:	pushm	ds,es
	popm	es,ds
	assume ds:DGROUP
	ret
vGetBitmapText	endp


	sdata

_VBB_Chroma	dd ?
;
PutRoutines	dd vPutBitmap256,vPutBitmap32k,vPutBitmap64k,vPutBitmap16m
	dd vPutBitmap16m32,3 dup (NearNull)
	dd NearNull
	dd vPutBitmapText
	dd 6 dup (NearNull)
PutOrRoutines	dd vPutORBitmap256,vPutORBitmap32k,vPutORBitmap64k,vPutORBitmap16m
	dd vPutORBitmap16m32,3 dup (NearNull)
	dd NearNull
	dd vPutORBitmapText
	dd 6 dup (NearNull)
GetRoutines	dd vGetBitmap256,vGetBitmap32k,vGetBitmap64k,vGetBitmap16m
	dd vGetBitmap16m32,3 dup (NearNull)
	dd NearNull
	dd vGetBitmapText
	dd 6 dup (NearNull)


	efile
	end

