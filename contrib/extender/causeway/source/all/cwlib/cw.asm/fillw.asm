	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Fill a rectangular region of specified window.
;
;On Entry:
;
;C style stack parameters as follows,
;
;window	- pointer to window structure (handle).
;flags	- flags, bit significant if set.
;	0 - fillvalue is RGB.
;fillvalue	- fill value to use.
;xcoord	- x co-ord to fill at.
;ycoord	- y co-ord to fill at.
;wide	- width in pixels.
;depth	- depth in pixels.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
WinFillRectangle:
_WinFillRectangle proc syscall window:dword, flags:dword, fillvalue:dword, \
	xcoord:dword, ycoord:dword, wide:dword, depth:dword
	local	@@X:dword, @@Y:dword, @@W:dword, @@D:dword
	public _WinFillRectangle
	;
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
	mov	ebx,xcoord
	add	ebx,Win_XCoord[edi]
	mov	ecx,wide
	cmp	ebx,Split_XCoord[esi]
	jge	@@1
	sub	ebx,Split_XCoord[esi]
	add	ecx,ebx
	js	@@8
	jz	@@8
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
	mov	@@X,ecx
	mov	@@Y,edx
	popm	esi,edi
	jc	@@8
	;
	push	@@D
	push	@@W
	push	@@Y
	push	@@X
	push	fillvalue
	push	flags
	call	VideoFillRectangle
	add	esp,4*6
@@8:	add	esi,size Split
	jmp	@@0
	;
@@9:	popad
	ret
_WinFillRectangle endp


	efile
	end

