	include ..\cwlib.inc
	scode

;------------------------------------------------------------------------------
;
;Copy and re-map "system" bitmap duplicate.
;
;On Entry:
;
;EDI	- pointer to "system" bitmap table entry.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
RemapSystemBitmap proc near
	pushm	eax,ebx,ecx,esi,edi
	mov	esi,BMT_Pointer[edi]
	mov	eax,BM_Wide[esi]
	mul	BM_Depth[esi]
	mov	ecx,eax
	mov	edi,BMT_Duplicate[edi]
	xchg	esi,edi
	add	edi,size BM
	mov	ebx,offset SystemXLat
l0:	mov	al,[esi]
	inc	esi
	xlat
	mov	[edi],al
	inc	edi
	dec	ecx
	jnz	l0
	popm	eax,ebx,ecx,esi,edi
	ret
RemapSystemBitmap endp


	efile
	end

