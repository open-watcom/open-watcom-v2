	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Make a piece of window look like a button.
;
;On Entry:
;
;EAX	- Type.
;ECX	- X coord
;EBX	- Y coord
;EDX	- width.
;EDI	- depth.
;ESI	- Pointer to window structure.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
ButtonWindow	proc	near
	pushad
	cmp	edx,-1
	jnz	@@00
	mov	edx,Win_Wide[esi]
@@00:	cmp	edi,-1
	jnz	@@01
	mov	edi,Win_Depth[esi]
@@01:	;
	or	eax,eax
	jnz	@@1
	push	1
	mov	eax,edx
	dec	eax
	push	eax
	push	ebx
	push	ecx
	mov	al,254
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	dec	eax
	push	eax
	push	1
	push	ebx
	push	ecx
	mov	al,254
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	dec	eax
	push	eax
	push	1
	mov	eax,ebx
	inc	eax
	push	eax
	mov	eax,edx
	dec	eax
	add	eax,ecx
	push	eax
	mov	al,252
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	mov	eax,edx
	sub	eax,2
	push	eax
	mov	eax,edi
	dec	eax
	add	eax,ebx
	push	eax
	mov	eax,ecx
	inc	eax
	push	eax
	mov	al,252
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	push	1
	push	ebx
	mov	eax,edx
	dec	eax
	add	eax,ecx
	push	eax
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	push	1
	mov	eax,edi
	dec	eax
	add	eax,ebx
	push	eax
	push	ecx
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	sub	eax,2
	push	eax
	mov	eax,edx
	sub	eax,2
	push	eax
	mov	eax,ebx
	inc	eax
	push	eax
	mov	eax,ecx
	inc	eax
	push	eax
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	jmp	@@9
	;
@@1:	cmp	eax,1
	jnz	@@2
	push	1
	mov	eax,edx
	dec	eax
	push	eax
	push	ebx
	push	ecx
	mov	al,251
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	dec	eax
	push	eax
	push	1
	push	ebx
	push	ecx
	mov	al,251
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	dec	eax
	push	eax
	push	1
	mov	eax,ebx
	inc	eax
	push	eax
	mov	eax,edx
	dec	eax
	add	eax,ecx
	push	eax
	mov	al,254
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	mov	eax,edx
	sub	eax,2
	push	eax
	mov	eax,edi
	dec	eax
	add	eax,ebx
	push	eax
	mov	eax,ecx
	inc	eax
	push	eax
	mov	al,254
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	push	1
	push	ebx
	mov	eax,edx
	dec	eax
	add	eax,ecx
	push	eax
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	push	1
	mov	eax,edi
	dec	eax
	add	eax,ebx
	push	eax
	push	ecx
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	sub	eax,2
	push	eax
	mov	eax,edx
	sub	eax,2
	push	eax
	mov	eax,ebx
	inc	eax
	push	eax
	mov	eax,ecx
	inc	eax
	push	eax
	mov	al,252
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	jmp	@@9
	;
@@2:	cmp	eax,2
	jnz	@@3
	push	1
	mov	eax,edx
	dec	eax
	push	eax
	push	ebx
	push	ecx
	mov	al,251
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	dec	eax
	push	eax
	push	1
	push	ebx
	push	ecx
	mov	al,251
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	dec	eax
	push	eax
	push	1
	mov	eax,ebx
	inc	eax
	push	eax
	mov	eax,edx
	dec	eax
	add	eax,ecx
	push	eax
	mov	al,254
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	mov	eax,edx
	sub	eax,2
	push	eax
	mov	eax,edi
	dec	eax
	add	eax,ebx
	push	eax
	mov	eax,ecx
	inc	eax
	push	eax
	mov	al,254
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	push	1
	push	ebx
	mov	eax,edx
	dec	eax
	add	eax,ecx
	push	eax
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	push	1
	mov	eax,edi
	dec	eax
	add	eax,ebx
	push	eax
	push	ecx
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	sub	eax,2
	push	eax
	mov	eax,edx
	sub	eax,2
	push	eax
	mov	eax,ebx
	inc	eax
	push	eax
	mov	eax,ecx
	inc	eax
	push	eax
	mov	al,252
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	jmp	@@9
	;
@@3:	cmp	eax,-1
	jnz	@@9
	push	1
	push	edx
	push	ebx
	push	ecx
	mov	al,251
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	push	edx
	mov	eax,edi
	dec	eax
	add	eax,ebx
	push	eax
	push	ecx
	mov	al,251
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	edi
	push	1
	push	ebx
	push	ecx
	mov	al,251
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	edi
	push	1
	push	ebx
	mov	eax,edx
	dec	eax
	add	eax,ecx
	push	eax
	mov	al,251
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	mov	eax,edx
	sub	eax,3
	push	eax
	mov	eax,ebx
	inc	eax
	push	eax
	mov	eax,ecx
	inc	eax
	push	eax
	mov	al,254
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	sub	eax,3
	push	eax
	push	1
	mov	eax,ebx
	inc	eax
	push	eax
	mov	eax,ecx
	inc	eax
	push	eax
	mov	al,254
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	mov	eax,edx
	sub	eax,3
	push	eax
	mov	eax,edi
	sub	eax,2
	add	eax,ebx
	push	eax
	mov	eax,ecx
	add	eax,2
	push	eax
	mov	al,252
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	sub	eax,3
	push	eax
	push	1
	mov	eax,ebx
	add	eax,2
	push	eax
	mov	eax,edx
	sub	eax,2
	add	eax,ecx
	push	eax
	mov	al,252
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	push	1
	mov	eax,ebx
	inc	eax
	push	eax
	mov	eax,edx
	sub	eax,2
	add	eax,ecx
	push	eax
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	push	1
	push	1
	mov	eax,edi
	sub	eax,2
	add	eax,ebx
	push	eax
	mov	eax,ecx
	inc	eax
	push	eax
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
	mov	eax,edi
	sub	eax,4
	push	eax
	mov	eax,edx
	sub	eax,4
	push	eax
	mov	eax,ebx
	add	eax,2
	push	eax
	mov	eax,ecx
	add	eax,2
	push	eax
	mov	al,253
	call	VideoSys2Hard
	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
	;
@@9:	popad
	ret
ButtonWindow	endp


	efile
	end

