
	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Blit a bitmap into a window.
;
;On Entry:
;
;C style stack parameters as follows,
;
;window	- pointer to window structure (handle).
;flags	- control flags, bit significant if set.
;	0 - N/A
;	1 - OR the bitmap on, use chroma.
;bitmap	- pointer to bitmap.
;xcoord	- x co-ord to blit at within window.
;ycoord	- y co-ord to blit at within window.
;xoff	- x offset within bitmap to blit from.
;yoff	- y offset within bitmap to blit from.
;wide	- with in pixels.
;Depth	- depth in pixels.
;chroma	- Chroma key if needed.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
WinBlitBitmap:
_WinBlitBitmap proc syscall window:dword, flags:dword, bitmap:dword, \
	xcoord:dword, ycoord:dword, xoff:dword, yoff:dword, \
	wide:dword, depth:dword, chroma:dword
	local	@@X:dword, @@Y:dword, @@XO:dword, @@YO:dword, \
	@@W:dword, @@D:dword
	public _WinBlitBitmap
	pushad
	mov	edi,window
	cmp	wide,-1
	jnz	@@5
	mov	eax,Win_Wide[edi]
	mov	wide,eax
@@5:	cmp	depth,-1
	jnz	@@6
	mov	eax,Win_Depth[edi]
	mov	depth,eax
@@6:	;
	lea	esi,Win_Splits[edi]
@@0:	cmp	Split_Wide[esi],0
	jz	@@9
	;
	mov	eax,xoff
	mov	@@XO,eax
	mov	eax,yoff
	mov	@@YO,eax
	;
	mov	ebx,xcoord
	add	ebx,Win_XCoord[edi]
	mov	ecx,wide
	cmp	ebx,Split_XCoord[esi]
	jge	@@1
	sub	ebx,Split_XCoord[esi]
	add	ecx,ebx
	js	@@8
	jz	@@8
	sub	@@XO,ebx
	mov	ebx,Split_XCoord[esi]
	;
@@1:	mov	eax,ebx
	add	eax,ecx
	mov	edx,Split_XCoord[esi]
	add	edx,Split_Wide[esi]
	cmp	eax,edx
	jl	@@2
	sub	eax,edx
	sub	ecx,eax
	js	@@8
	jz	@@8
	;
@@2:	mov	@@X,ebx
	mov	@@W,ecx
	;
	mov	ebx,ycoord
	add	ebx,Win_YCoord[edi]
	mov	ecx,depth
	cmp	ebx,Split_YCoord[esi]
	jge	@@3
	sub	ebx,Split_YCoord[esi]
	add	ecx,ebx
	js	@@8
	jz	@@8
	sub	@@YO,ebx
	mov	ebx,Split_YCoord[esi]
	;
@@3:	mov	eax,ebx
	add	eax,ecx
	mov	edx,Split_YCoord[esi]
	add	edx,Split_Depth[esi]
	cmp	eax,edx
	jl	@@4
	sub	eax,edx
	sub	ecx,eax
	js	@@8
	jz	@@8
	;
@@4:	mov	@@Y,ebx
	mov	@@D,ecx
	;
	pushm	esi,edi
	mov	ecx,@@X
	mov	edx,@@Y
	mov	esi,@@W
	mov	edi,@@D
	call	WindowRegionClip
	mov	@@W,esi
	mov	@@D,edi
	popm	esi,edi
	jc	@@8
	sub	ecx,@@X
	add	@@X,ecx
	add	@@XO,ecx
	sub	edx,@@Y
	add	@@Y,edx
	add	@@YO,edx
	;
	push	chroma
	push	@@D
	push	@@W
	push	@@YO
	push	@@XO
	push	@@Y
	push	@@X
	push	bitmap
	push	flags
	call	VideoBlitBitmap
	add	esp,4*9
@@8:	add	esi,size Split
	jmp	@@0
	;
@@9:	popad
	ret
_WinBlitBitmap	endp


	efile
	end

