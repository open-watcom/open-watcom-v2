	include ..\cwlib.inc
	scode

;------------------------------------------------------------------------------
;
;Re-map all bitmaps registered as useing "system" palette.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
RemapSystemBitmaps proc near
	pushad
;
;Work out new translation table.
;
	mov	esi,offset SystemPalette+3
	mov	edi,offset HardwarePalette+3
	mov	ebx,offset SystemXLat+1
	mov	edx,1
	mov	ecx,255
l2:	push	ebx
	mov	eax,d[esi]
	mov	ebx,d[edi]
	shl	eax,2
	and	eax,0FFFFFFh
	shl	ebx,2
	and	ebx,0FFFFFFh
	cmp	eax,ebx
	jz	l3
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	call	SearchRGB
	jmp	l4
l3:	mov	eax,edx
l4:	pop	ebx
	mov	[ebx],al
	add	esi,3
	add	edi,3
	inc	ebx
	inc	edx
	dec	ecx
	jnz	l2
;
;Now re-map them.
;
	mov	edi,BMTable
	mov	ecx,MaxBMEntries
l0:	test	BMT_Flags[edi],BMT_InUse	;This entry in use?
	jz	l1
	call	RemapSystemBitmap
l1:	add	edi,size BMT
	dec	ecx
	jnz	l0
;
	popad
	ret
RemapSystemBitmaps endp


	efile
	end

