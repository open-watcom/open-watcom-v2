	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Blit a bitmap into another bitmap.
;
;On Entry:
;
;C style stack parameters as follows:
;
;Flags	- Flags, bit significant if set, may be combined.
;	0-Reserved.
;	1-OR the bitmap, "Chroma" contains transparent colour.
;	2-Scaled blit, DWide & DDepth contain destination sizes.
;SBitmap	-Pointer to source bitmap.
;DBitmap	-Pointer to destination bitmap.
;XCoord	-X co-ord in destination to blit to.
;YCoord	-Y co-ord in destination to blit to.
;XOff	-X offset within source to blit from.
;YOff	-Y offset within source to blit from.
;Wide	-Width to blit, -1 for entire source width.
;Depth	-Depth to blit, -1 for entire destination width.
;DWide	-Destination width of blit when scaleing.
;DDepth	-Destination depth of blit when scaleing.
;Chroma	-Transparent colour when OR'ing.
;
;On Exit:
;
;ALL registers preserved.
;
BitmapBlitBitmap:
_BitmapBlitBitmap proc syscall flags:dword, SBitmap:dword, DBitmap:dword, \
	XCoord:dword, YCoord:dword, XOff:dword, YOff:dword, \
	Wide:dword, Depth:dword, DWide:dword, DDepth:dword, \
	Chroma:dword
	pushm	_BBB_Source, _BBB_Dest, _BBB_Chroma
	pushad
	;
	mov	eax,Chroma
	mov	_BBB_Chroma,eax
	;
	mov	esi,SBitmap
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
;If scaleing, work out scale values needed.
;
@@3:	test	Flags,4
	jz	@@11
	mov	eax,Wide
	shl	eax,16
	xor	edx,edx
	mov	ebx,DWide
	div	ebx
	movzx	edx,ax
	shl	edx,16
	mov	_BBB_wfrac,edx
	shr	eax,16
	mov	_BBB_wwhole,eax
	;
	mov	eax,Depth
	shl	eax,16
	xor	edx,edx
	mov	ebx,DDepth
	div	ebx
	movzx	edx,ax
	shl	edx,16
	mov	_BBB_dfrac,edx
	shr	eax,16
	mov	_BBB_dwhole,eax
	;
	mov	_BBB_dacc,0
;
;Clip coords/width/depth to fit the destination.
;
@@11:	mov	edi,DBitmap
	cmp	XCoord,0
	jns	@@4
	mov	eax,XCoord
	neg	eax
	test	Flags,4
	jz	@@3_0
	sub	DWide,eax
	jmp	@@3_1
@@3_0:	sub	Wide,eax
@@3_1:	js	@@BlitFinished
	jz	@@BlitFinished
	add	XOff,eax
	mov	XCoord,0
;
@@4:	cmp	YCoord,0
	jns	@@5
	mov	eax,YCoord
	neg	eax
	test	Flags,4
	jz	@@4_0
	sub	DDepth,eax
	jmp	@@4_1
@@4_0:	sub	Depth,eax
@@4_1:	js	@@BlitFinished
	jz	@@BlitFinished
	add	YOff,eax
	mov	YCoord,0
;
@@5:	test	Flags,4
	jz	@@5_0
	mov	eax,XCoord
	add	eax,DWide
	cmp	eax,BM_Wide[edi]
	jl	@@6
	sub	eax,BM_Wide[edi]
	sub	DWide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	jmp	@@6
@@5_0:	mov	eax,XCoord
	add	eax,Wide
	cmp	eax,BM_Wide[edi]
	jl	@@6
	sub	eax,BM_Wide[edi]
	sub	Wide,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
@@6:	test	Flags,4
	jz	@@6_0
	mov	eax,YCoord
	add	eax,DDepth
	cmp	eax,BM_Depth[edi]
	jl	@@7
	sub	eax,BM_Depth[edi]
	sub	DDepth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
	jmp	@@7
@@6_0:	mov	eax,YCoord
	add	eax,Depth
	cmp	eax,BM_Depth[edi]
	jl	@@7
	sub	eax,BM_Depth[edi]
	sub	Depth,eax
	js	@@BlitFinished
	jz	@@BlitFinished
;
;Now work out the source address and width.
;
@@7:	mov	eax,esi
	add	eax,size BM
	mov	_BBB_Source,eax
	mov	eax,YOff
	mul	BM_PWide[esi]
	mul	BM_Wide[esi]
	add	_BBB_Source,eax
	mov	eax,XOff
	mul	BM_PWide[esi]
	add	_BBB_Source,eax
;
;Work out the destination address.
;
	mov	eax,edi
	add	eax,size BM
	mov	_BBB_Dest,eax
	mov	eax,YCoord
	mul	BM_PWide[edi]
	mul	BM_Wide[edi]
	add	_BBB_Dest,eax
	mov	eax,XCoord
	mul	BM_PWide[edi]
	add	_BBB_Dest,eax
;
;Get on with it.
;
	mov	ebx,SBitmap
	mov	eax,BM_Flags[edi]
	and	eax,15
	mov	esi,_BBB_Source
	mov	edi,_BBB_Dest
	mov	ecx,Wide
	mov	edx,Depth
	test	Flags,4
	jz	@@Put
;
;Put it on with scaleing.
;
	push	eax
	mov	ebx,SBitmap
	mov	eax,BM_PWide[ebx]
	mul	BM_Wide[ebx]
	mov	ebx,eax
	pop	eax
	mov	ecx,DWide
	mov	edx,DDepth
@@Puts0:	push	edx
	push	eax
	pushm	esi,edi,ebx,ecx
	mov	ebx,SBitmap
	test	Flags,2
	jnz	@@Puts1
	call	[PutScaleRoutines+eax*4]
	jmp	@@Puts2
@@Puts1:	call	[PutORScaleRoutines+eax*4]
@@Puts2:	mov	edi,DBitmap
	mov	eax,BM_PWide[edi]
	mul	BM_Wide[edi]
	popm	esi,edi,ebx,ecx
	add	edi,eax
	mov	eax,_BBB_dfrac
	add	_BBB_dacc,eax
	mov	eax,0
	adc	eax,_BBB_dwhole
	mul	ebx
	add	esi,eax
	pop	eax
	pop	edx
	dec	edx
	jnz	@@Puts0
	jmp	@@BlitFinished
;
;Put it on as normal.
;
@@Put:	push	ebp
	test	Flags,2
	mov	ebp,DBitmap
	jnz	@@PutOR
	call	[PutRoutines+eax*4]
	pop	ebp
	jmp	@@BlitFinished
;
;Or it on.
;
@@PutOR:	call	[PutORRoutines+eax*4]
	pop	ebp
@@BlitFinished: ;
	popad
	popm	_BBB_Source, _BBB_Dest, _BBB_Chroma
	ret
_BitmapBlitBitmap endp


;-------------------------------------------------------------------------
PutBitmap256	proc	near
	mov	eax,BM_Flags[ebx]
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
;256 to 256 blit.
;
@@256:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
@@256_0:	pushm	ecx,esi,edi
	rep_movsb
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@256_0
	jmp	@@9
;
;32k to 256 blit.
;
@@32k:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	shl	ebx,1
@@32k_0:	pushm	ebx,ecx,esi,edi
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
	call	SearchRGB
	stosb
	loop	@@32k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@32k_0
	jmp	@@9
;
;64k to 256 blit.
;
@@64k:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	shl	ebx,1
@@64k_0:	pushm	ebx,ecx,esi,edi
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
	call	SearchRGB
	stosb
	loop	@@64k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@64k_0
	jmp	@@9
;
;16m to 256 blit.
;
@@16m:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebx
	shl	ebx,1
	add	ebx,eax
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	mov	eax,[esi]
	add	esi,3
	and	eax,0FFFFFFh
	call	SearchRGB
	stosb
	loop	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@16m_0
;
@@9:	ret
PutBitmap256	endp


;-------------------------------------------------------------------------
PutBitmap32k	proc	near
	mov	eax,BM_Flags[ebx]
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
;256 to 32k blit.
;
@@256:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@256_0:	pushm	ebx,ecx,esi,edi
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
	loop	@@256_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@256_0
	jmp	@@9
;
;32k to 32k blit.
;
@@32k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@32k_0:	pushm	ecx,esi,edi
	rep_movsw
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@32k_0
	jmp	@@9
;
;64k to 32k blit.
;
@@64k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@64k_0:	pushm	ebx,ecx,esi,edi
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
	loop	@@64k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@64k_0
	jmp	@@9
;
;16m to 32k blit.
;
@@16m:	mov	ebx,BM_Wide[ebx]
	mov	eax,ebx
	shl	ebx,1
	add	ebx,eax
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@16m_0:	pushm	ebx,ecx,esi,edi
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
	loop	@@16m_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@16m_0
;
@@9:	ret
PutBitmap32k	endp


;-------------------------------------------------------------------------
PutBitmap64k	proc	near
	mov	eax,BM_Flags[ebx]
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
;256 to 64k blit.
;
@@256:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@256_0:	pushm	ebx,ecx,esi,edi
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
	loop	@@256_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@256_0
	jmp	@@9
;
;32k to 64k blit.
;
@@32k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@32k_0:	pushm	ebx,ecx,esi,edi
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
	loop	@@32k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@32k_0
	jmp	@@9
;
;64k to 64k blit.
;
@@64k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@64k_0:	pushm	ecx,esi,edi
	rep_movsw
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@64k_0
	jmp	@@9
;
;16m to 64k blit.
;
@@16m:	mov	ebx,BM_Wide[ebx]
	mov	eax,ebx
	shl	ebx,1
	add	ebx,eax
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@16m_0:	pushm	ebx,ecx,esi,edi
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
	loop	@@16m_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@16m_0
;
@@9:	ret
PutBitmap64k	endp


;-------------------------------------------------------------------------
PutBitmap16m	proc	near
	mov	eax,BM_Flags[ebx]
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
;256 to 16m blit.
;
@@256:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebp
	shl	ebp,1
	add	ebp,eax
@@256_0:	pushm	ebx,ecx,esi,edi
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
	loop	@@256_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@256_0
	jmp	@@9
;
;32k to 16m blit.
;
@@32k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebp
	shl	ebp,1
	add	ebp,eax
@@32k_0:	pushm	ebx,ecx,esi,edi
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
	loop	@@32k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@32k_0
	jmp	@@9
;
;64k to 16m blit.
;
@@64k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebp
	shl	ebp,1
	add	ebp,eax
@@64k_0:	pushm	ebx,ecx,esi,edi
@@64k_1:	lodsw
	movzx	ebx,ax
	xor	eax,eax
	shl	ebx,16
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
	loop	@@64k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@64k_0
	jmp	@@9
;
;16m to 16m blit.
;
@@16m:	mov	ebx,BM_Wide[ebx]
	mov	eax,ebx
	shl	ebx,1
	add	ebx,eax
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebp
	shl	ebp,1
	add	ebp,eax
	lea	ecx,[ecx+ecx*2]
@@16m_0:	pushm	ecx,esi,edi
	rep_movsb
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@16m_0
;
@@9:	ret
PutBitmap16m	endp


;-------------------------------------------------------------------------
PutORBitmap256	proc	near
	mov	eax,BM_Flags[ebx]
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
;256 to 256 blit.
;
@@256:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
@@256_0:	pushm	ecx,esi,edi
@@256_1:	lodsb
	cmp	al,b[_BBB_Chroma]
	jz	@@256_3
	stosb
@@256_2:	loop	@@256_1
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@256_0
	jmp	@@9
@@256_3:	inc	edi
	jmp	@@256_2
;
;32k to 256 blit.
;
@@32k:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	shl	ebx,1
@@32k_0:	pushm	ebx,ecx,esi,edi
@@32k_1:	lodsw
	cmp	ax,w[_BBB_Chroma]
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
	call	SearchRGB
	stosb
@@32k_2:	loop	@@32k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@32k_0
	jmp	@@9
@@32k_3:	inc	edi
	jmp	@@32k_2
;
;64k to 256 blit.
;
@@64k:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	shl	ebx,1
@@64k_0:	pushm	ebx,ecx,esi,edi
@@64k_1:	lodsw
	cmp	ax,w[_BBB_Chroma]
	jz	@@64k_3
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
	call	SearchRGB
	stosb
@@64k_2:	loop	@@64k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@64k_0
	jmp	@@9
@@64k_3:	inc	edi
	jmp	@@64k_2
;
;16m to 256 blit.
;
@@16m:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebx
	shl	ebx,1
	add	ebx,eax
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	and	eax,0FFFFFFh
	cmp	eax,d[_BBB_Chroma]
	jz	@@16m_3
	or	ah,al
	shr	eax,8
	or	al,ah
	stosb
@@16m_2:	loop	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@16m_0
	jmp	@@9
@@16m_3:	inc	edi
	jmp	@@16m_2
;
@@9:	ret
PutORBitmap256	endp


;-------------------------------------------------------------------------
PutORBitmap32k	proc	near
	mov	eax,BM_Flags[ebx]
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
;256 to 32k blit.
;
@@256:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@256_0:	pushm	ebx,ecx,esi,edi
@@256_1:	xor	eax,eax
	lodsb
	cmp	al,b[_BBB_Chroma]
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
@@256_2:	loop	@@256_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@256_0
	jmp	@@9
@@256_3:	add	edi,2
	jmp	@@256_2
;
;32k to 32k blit.
;
@@32k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@32k_0:	pushm	ecx,esi,edi
@@32k_1:	lodsw
	cmp	ax,w[_BBB_Chroma]
	jz	@@32k_3
	stosw
@@32k_2:	loop	@@32k_1
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@32k_0
	jmp	@@9
@@32k_3:	add	edi,2
	jmp	@@32k_2
;
;64k to 32k blit.
;
@@64k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@64k_0:	pushm	ebx,ecx,esi,edi
@@64k_1:	lodsw
	cmp	ax,w[_BBB_Chroma]
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
@@64k_2:	loop	@@64k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@64k_0
	jmp	@@9
@@64k_3:	add	edi,2
	jmp	@@64k_2
;
;16m to 32k blit.
;
@@16m:	mov	ebx,BM_Wide[ebx]
	mov	eax,ebx
	shl	ebx,1
	add	ebx,eax
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@16m_0:	pushm	ebx,ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	and	eax,0FFFFFFh
	cmp	eax,d[_BBB_Chroma]
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
@@16m_2:	loop	@@16m_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@16m_0
	jmp	@@9
@@16m_3:	add	edi,2
	jmp	@@16m_2
;
@@9:	ret
PutORBitmap32k	endp


;-------------------------------------------------------------------------
PutORBitmap64k	proc	near
	mov	eax,BM_Flags[ebx]
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
;256 to 64k blit.
;
@@256:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@256_0:	pushm	ebx,ecx,esi,edi
@@256_1:	xor	eax,eax
	lodsb
	cmp	al,b[_BBB_Chroma]
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
@@256_2:	loop	@@256_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@256_0
	jmp	@@9
@@256_3:	add	edi,2
	jmp	@@256_2
;
;32k to 64k blit.
;
@@32k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@32k_0:	pushm	ebx,ecx,esi,edi
@@32k_1:	lodsw
	cmp	ax,w[_BBB_Chroma]
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
@@32k_2:	loop	@@32k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@32k_0
	jmp	@@9
@@32k_3:	add	edi,2
	jmp	@@32k_2
;
;64k to 64k blit.
;
@@64k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@64k_0:	pushm	ecx,esi,edi
@@64k_1:	lodsw
	cmp	ax,w[_BBB_Chroma]
	jz	@@64k_3
	stosw
@@64k_2:	loop	@@64k_1
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@64k_0
	jmp	@@9
@@64k_3:	add	edi,2
	jmp	@@64k_2
;
;16m to 64k blit.
;
@@16m:	mov	ebx,BM_Wide[ebx]
	mov	eax,ebx
	shl	ebx,1
	add	ebx,eax
	mov	ebp,BM_Wide[ebp]
	shl	ebp,1
@@16m_0:	pushm	ebx,ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	and	eax,0FFFFFFh
	cmp	eax,d[_BBB_Chroma]
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
@@16m_2:	loop	@@16m_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@16m_0
	jmp	@@9
@@16m_3:	add	edi,2
	jmp	@@16m_2
;
@@9:	ret
PutORBitmap64k	endp


;-------------------------------------------------------------------------
PutORBitmap16m	proc	near
	mov	eax,BM_Flags[ebx]
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
;256 to 16m blit.
;
@@256:	mov	ebx,BM_Wide[ebx]
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebp
	shl	ebp,1
	add	ebp,eax
@@256_0:	pushm	ebx,ecx,esi,edi
@@256_1:	lodsb
	cmp	al,b[_BBB_Chroma]
	jz	@@256_3
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
@@256_2:	loop	@@256_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@256_0
	jmp	@@9
@@256_3:	add	edi,3
	jmp	@@256_2
;
;32k to 16m blit.
;
@@32k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebp
	shl	ebp,1
	add	ebp,eax
@@32k_0:	pushm	ebx,ecx,esi,edi
@@32k_1:	lodsw
	cmp	ax,w[_BBB_Chroma]
	jz	@@32k_3
	movzx	ebx,ax
	shl	ebx,1
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
@@32k_2:	loop	@@32k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@32k_0
	jmp	@@9
@@32k_3:	add	edi,3
	jmp	@@32k_2
;
;64k to 16m blit.
;
@@64k:	mov	ebx,BM_Wide[ebx]
	shl	ebx,1
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebp
	shl	ebp,1
	add	ebp,eax
@@64k_0:	pushm	ebx,ecx,esi,edi
@@64k_1:	lodsw
	cmp	ax,w[_BBB_Chroma]
	jz	@@64k_3
	movzx	ebx,ax
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
@@64k_2:	loop	@@64k_1
	popm	ebx,ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@64k_0
	jmp	@@9
@@64k_3:	add	edi,3
	jmp	@@64k_2
;
;16m to 16m blit.
;
@@16m:	mov	ebx,BM_Wide[ebx]
	mov	eax,ebx
	shl	ebx,1
	add	ebx,eax
	mov	ebp,BM_Wide[ebp]
	mov	eax,ebp
	shl	ebp,1
	add	ebp,eax
@@16m_0:	pushm	ecx,esi,edi
@@16m_1:	lodsd
	dec	esi
	and	eax,0FFFFFFh
	cmp	eax,d[_BBB_Chroma]
	jz	@@16m_3
	stosw
	shr	eax,16
	stosb
@@16m_2:	loop	@@16m_1
	popm	ecx,esi,edi
	add	esi,ebx
	add	edi,ebp
	dec	edx
	jnz	@@16m_0
	jmp	@@9
@@16m_3:	add	edi,3
	jmp	@@16m_2
;
@@9:	ret
PutORBitmap16m	endp


;-------------------------------------------------------------------------
PutScaleBitmap256 proc near
	mov	_BBB_wacc,0
	mov	eax,BM_Flags[ebx]
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
;256 to 256 blit.
;
@@256:
@@256_0:	mov	al,[esi]
	stosb
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	loop	@@256_0
	jmp	@@9
;
;32k to 256 blit.
;
@@32k:
@@32k_1:	mov	ax,[esi]
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
	call	SearchRGB
	stosb
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@32k_1
	jmp	@@9
;
;64k to 256 blit.
;
@@64k:
@@64k_1:	mov	ax,[esi]
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
	call	SearchRGB
	stosb
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@64k_1
	jmp	@@9
;
;16m to 256 blit.
;
@@16m:
@@16m_1:	mov	al,[esi]
	or	al,[esi+1]
	or	al,[esi+2]
	stosb
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@16m_1
;
@@9:	ret
PutScaleBitmap256 endp


;-------------------------------------------------------------------------
PutScaleBitmap32k proc near
	mov	_BBB_wacc,0
	mov	eax,BM_Flags[ebx]
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
;256 to 32k blit.
;
@@256:
@@256_1:	movzx	eax,b[esi]
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
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	loop	@@256_1
	jmp	@@9
;
;32k to 32k blit.
;
@@32k:
@@32k_0:	mov	ax,[esi]
	stosw
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@32k_0
	jmp	@@9
;
;64k to 32k blit.
;
@@64k:
@@64k_1:	movzx	ebx,w[esi]
	xor	eax,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	ebx,6
	shld	eax,ebx,5
	stosw
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@64k_1
	jmp	@@9
;
;16m to 32k blit.
;
@@16m:
@@16m_1:	mov	eax,[esi]
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@16m_1
;
@@9:	ret
PutScaleBitmap32k endp


;-------------------------------------------------------------------------
PutScaleBitmap64k proc near
	mov	_BBB_wacc,0
	mov	eax,BM_Flags[ebx]
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
;256 to 64k blit.
;
@@256:
@@256_1:	movzx	eax,b[esi]
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
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	loop	@@256_1
	jmp	@@9
;
;32k to 64k blit.
;
@@32k:
@@32k_1:	movzx	ebx,w[esi]
	xor	eax,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,1
	shl	ebx,5
	shld	eax,ebx,5
	stosw
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@32k_1
	jmp	@@9
;
;64k to 64k blit.
;
@@64k:
@@64k_0:	mov	ax,[esi]
	stosw
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@64k_0
	jmp	@@9
;
;16m to 64k blit.
;
@@16m:
@@16m_1:	mov	eax,[esi]
	shl	eax,8
	mov	ebx,eax
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	stosw
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	shl	eax,1
	add	esi,eax
	loop	@@16m_1
;
@@9:	ret
PutScaleBitmap64k endp


;-------------------------------------------------------------------------
PutScaleBitmap16m proc near
	mov	_BBB_wacc,0
	mov	eax,BM_Flags[ebx]
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
;256 to 16m blit.
;
@@256:
@@256_1:	movzx	eax,b[esi]
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
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	loop	@@256_1
	jmp	@@9
;
;32k to 16m blit.
;
@@32k:
@@32k_1:	mov	ax,[esi]
	movzx	ebx,ax
	shl	ebx,1
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
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@32k_1
	jmp	@@9
;
;64k to 16m blit.
;
@@64k:
@@64k_1:	mov	ax,[esi]
	movzx	ebx,ax
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
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@64k_1
	jmp	@@9
;
;16m to 16m blit.
;
@@16m:
@@16m_0:	mov	eax,[esi]
	stosw
	shr	eax,16
	stosb
	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	shl	eax,1
	add	esi,eax
	loop	@@16m_0
;
@@9:	ret
PutScaleBitmap16m endp


;-------------------------------------------------------------------------
PutORScaleBitmap256 proc near
	mov	_BBB_wacc,0
	mov	eax,BM_Flags[ebx]
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
;256 to 256 blit.
;
@@256:
@@256_0:	mov	al,[esi]
	cmp	al,b[_BBB_Chroma]
	jz	@@256_3
	stosb
@@256_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	loop	@@256_0
	jmp	@@9
@@256_3:	inc	edi
	jmp	@@256_2
;
;32k to 256 blit.
;
@@32k:
@@32k_1:	mov	ax,[esi]
	cmp	ax,w[_BBB_Chroma]
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
	call	SearchRGB
	stosb
@@32k_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@32k_1
	jmp	@@9
@@32k_3:	inc	edi
	jmp	@@32k_2
;
;64k to 256 blit.
;
@@64k:
@@64k_1:	mov	ax,[esi]
	cmp	ax,w[_BBB_Chroma]
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
	call	SearchRGB
	stosb
@@64k_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@64k_1
	jmp	@@9
@@64k_3:	inc	edi
	jmp	@@64k_2
;
;16m to 256 blit.
;
@@16m:
@@16m_1:	mov	eax,[esi]
	and	eax,0FFFFFFh
	cmp	eax,d[_BBB_Chroma]
	jz	@@16m_3
	mov	ah,al
	shr	eax,8
	or	al,ah
	stosb
@@16m_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@16m_1
	jmp	@@9
@@16m_3:	inc	edi
	jmp	@@16m_2
;
@@9:	ret
PutORScaleBitmap256 endp


;-------------------------------------------------------------------------
PutORScaleBitmap32k proc near
	mov	_BBB_wacc,0
	mov	eax,BM_Flags[ebx]
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
;256 to 32k blit.
;
@@256:
@@256_1:	movzx	eax,b[esi]
	cmp	al,b[_BBB_Chroma]
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
@@256_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	loop	@@256_1
	jmp	@@9
@@256_3:	add	edi,2
	jmp	@@256_2
;
;32k to 32k blit.
;
@@32k:
@@32k_0:	mov	ax,[esi]
	cmp	ax,w[_BBB_Chroma]
	jz	@@32k_3
	stosw
@@32k_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@32k_0
	jmp	@@9
@@32k_3:	add	edi,2
	jmp	@@32k_2
;
;64k to 32k blit.
;
@@64k:
@@64k_1:	movzx	ebx,w[esi]
	cmp	bx,w[_BBB_Chroma]
	jz	@@64k_3
	xor	eax,eax
	shl	ebx,16
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	ebx,6
	shld	eax,ebx,5
	stosw
@@64k_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@64k_1
	jmp	@@9
@@64k_3:	add	edi,2
	jmp	@@64k_2
;
;16m to 32k blit.
;
@@16m:
@@16m_1:	mov	eax,[esi]
	and	eax,0FFFFFFh
	cmp	eax,d[_BBB_Chroma]
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
@@16m_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@16m_1
	jmp	@@9
@@16m_3:	add	edi,2
	jmp	@@16m_2
;
@@9:	ret
PutORScaleBitmap32k endp


;-------------------------------------------------------------------------
PutORScaleBitmap64k proc near
	mov	_BBB_wacc,0
	mov	eax,BM_Flags[ebx]
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
;256 to 64k blit.
;
@@256:
@@256_1:	movzx	eax,b[esi]
	cmp	al,b[_BBB_Chroma]
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
@@256_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	loop	@@256_1
	jmp	@@9
@@256_3:	add	edi,2
	jmp	@@256_2
;
;32k to 64k blit.
;
@@32k:
@@32k_1:	movzx	ebx,w[esi]
	cmp	bx,w[_BBB_Chroma]
	jz	@@32k_3
	xor	eax,eax
	shl	ebx,16+1
	shld	eax,ebx,5
	shl	ebx,5
	shld	eax,ebx,5
	shl	eax,1
	shl	ebx,5
	shld	eax,ebx,5
	stosw
@@32k_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@32k_1
	jmp	@@9
@@32k_3:	add	edi,2
	jmp	@@32k_2
;
;64k to 64k blit.
;
@@64k:
@@64k_0:	mov	ax,[esi]
	cmp	ax,w[_BBB_Chroma]
	jz	@@64k_3
	stosw
@@64k_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@64k_0
	jmp	@@9
@@64k_3:	add	edi,2
	jmp	@@64k_2
;
;16m to 64k blit.
;
@@16m:
@@16m_1:	mov	eax,[esi]
	and	eax,0FFFFFFh
	cmp	eax,d[_BBB_Chroma]
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
@@16m_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	shl	eax,1
	add	esi,eax
	loop	@@16m_1
	jmp	@@9
@@16m_3:	add	edi,2
	jmp	@@16m_2
;
@@9:	ret
PutORScaleBitmap64k endp


;-------------------------------------------------------------------------
PutORScaleBitmap16m proc near
	mov	_BBB_wacc,0
	mov	eax,BM_Flags[ebx]
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
;256 to 16m blit.
;
@@256:
@@256_1:	movzx	eax,b[esi]
	cmp	al,b[_BBB_Chroma]
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
@@256_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	loop	@@256_1
	jmp	@@9
@@256_3:	add	edi,3
	jmp	@@256_2
;
;32k to 16m blit.
;
@@32k:
@@32k_1:	movzx	ebx,w[esi]
	cmp	bx,w[_BBB_Chroma]
	jz	@@32k_3
	shl	ebx,1
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
@@32k_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@32k_1
	jmp	@@9
@@32k_3:	add	edi,3
	jmp	@@32k_2
;
;64k to 16m blit.
;
@@64k:
@@64k_1:	movzx	ebx,w[esi]
	cmp	bx,w[_BBB_Chroma]
	jz	@@64k_3
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
@@64k_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	shl	eax,1
	add	esi,eax
	loop	@@64k_1
	jmp	@@9
@@64k_3:	add	edi,3
	jmp	@@64k_2
;
;16m to 16m blit.
;
@@16m:
@@16m_0:	mov	eax,[esi]
	and	eax,0FFFFFFh
	cmp	eax,d[_BBB_Chroma]
	jz	@@16m_3
	stosw
	shr	eax,16
	stosb
@@16m_2:	mov	eax,_BBB_wfrac
	add	_BBB_wacc,eax
	mov	eax,0
	adc	eax,_BBB_wwhole
	add	esi,eax
	shl	eax,1
	add	esi,eax
	loop	@@16m_0
	jmp	@@9
@@16m_3:	add	edi,3
	jmp	@@16m_2
;
@@9:	ret
PutORScaleBitmap16m endp


	sdata
;
_BBB_Source	dd ?
_BBB_Dest	dd ?
_BBB_Chroma	dd ?
;
_BBB_wfrac	dd ?
_BBB_wwhole	dd ?
_BBB_dfrac	dd ?
_BBB_dwhole	dd ?
_BBB_wacc	dd ?
_BBB_dacc	dd ?

PutRoutines	dd PutBitmap256, PutBitmap32k, PutBitmap64k, PutBitmap16m
	dd 12 dup (NearNull)

PutORRoutines	dd PutORBitmap256, PutORBitmap32k, PutORBitmap64k, PutORBitmap16m
	dd 12 dup (NearNull)

PutScaleRoutines dd PutScaleBitmap256, PutScaleBitmap32k, PutScaleBitmap64k, PutScaleBitmap16m
	dd 14 dup (nearNull)

PutORScaleRoutines dd PutORScaleBitmap256, PutORScaleBitmap32k, PutORScaleBitmap64k, PutORScaleBitmap16m
	dd 14 dup (nearNull)


	efile
	end

