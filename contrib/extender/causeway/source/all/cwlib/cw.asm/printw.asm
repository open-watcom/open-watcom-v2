	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Print a string of bitmaps in a window.
;
;On Entry:
;
;C style stack parameters as follows,
;
;window	- pointer to window structure (handle).
;flags	- control flags, bit significant if set,
;	0 - N/A
;	1 - OR bitmaps on, use chroma.
;	4 - used fixed spaceing.
;pen	- pen to print in.
;font	- font table to use.
;string	- string to print.
;xcoord	- x co-ord to print at.
;ycoord	- y co-ord to print at.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
WinPrintWindow:
_WinPrintWindow proc syscall window:dword, flags:dword, pen:dword, font:dword, \
	string:dword, xcoord:dword, ycoord:dword
	local	@@Flags:dword
	pushad
	mov	eax,flags
	and	eax,15
	mov	@@flags,eax
	;
@@0:	mov	esi,string
	inc	string
	cmp	b[esi],0		;end of the string?
	jz	@@9
	movzx	eax,b[esi]
	mov	esi,font
	mov	esi,[esi+eax*4]	;point to this character.
	mov	edi,offset FontBuffer
	test	flags,16
	jz	@@3
	mov	eax,BM_XOffset[esi]
	add	xcoord,eax
@@3:	mov	eax,BM_Wide[esi]
	mul	BM_PWide[esi]
	mul	BM_Depth[esi]
	mov	ecx,size BM
	rep	movsb
	mov	ecx,eax
	mov	eax,pen
	xchg	ah,al
	test	VideoModeFlags,8
	jnz	@@1
	dec	ah
@@1:	lodsb
	or	al,al
	jz	@@2
	add	al,ah
@@2:	stosb
	loop	@@1
	;
	mov	esi,offset FontBuffer
	mov	ebx,ycoord
	add	ebx,BM_YOffset[esi]
	push	0
	push	-1
	push	-1
	push	0
	push	0
	push	ebx
	push	xcoord
	push	esi
	push	@@flags
	push	window
	call	WinBlitBitmap
	add	esp,4*10
	;
	test	flags,16
	jz	@@4
	mov	esi,offset FontBuffer
	mov	eax,BM_XOffset[esi]
	sub	xcoord,eax
	mov	esi,font
	movzx	eax,b[esi+0]
	jmp	@@5
@@4:	mov	eax,BM_Wide[esi]
@@5:	add	xcoord,eax
	jmp	@@0
	;
@@9:	popad
	ret
_WinPrintWindow endp


	efile
	end

