	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Register a bitmap that needs to be re-mapped whenever the hardware palette
;changes so it stays in the system palette scheme.
;
;On Entry:
;
;ESI	- bitmap.
;
;On Exit:
;
;EAX	- handle.
;
;ALL other registers preserved.
;
RegisterSystemBitmap proc near
	pushm	ebx,ecx,edx,esi,edi,ebp
;
;Find a free table entry.
;
	mov	edi,BMTable
	mov	ecx,MaxBMEntries
l0:	test	BMT_Flags[edi],BMT_InUse	;This entry in use?
	jz	l1
	add	edi,(size BMT)
	dec	ecx
	jnz	l0
	jmp	l9_2
	;
l1:	mov	BMT_Pointer[edi],esi
	mov	BMT_Flags[edi],BMT_InUse	;Mark this entry as used.
	;
	;Need to claim memory block to duplicate this bitmap
	;for future remaps.
	;
	pushm	esi,edi
	mov	eax,BM_Wide[esi]	;get bitmap dimensions.
	mul	BM_Depth[esi]
	mov	ecx,eax
	call	Malloc
	mov	eax,esi
	popm	esi,edi
	jc	l9_1
	mov	BMT_Duplicate[edi],eax
	push	edi
	mov	edi,eax
	add	esi,size BM		;skip its header.
	rep_movsb		;make a copy of it.
	pop	edi
	mov	eax,edi
	;
	call	RemapSystemBitmap	;now re-map it.
	jmp	exit
	;
l9_1:	mov	BMT_Flags[edi],0	;Clear flags again.
l9_2:	xor	eax,eax
	;
exit:	popm	ebx,ecx,edx,esi,edi,ebp
	ret
RegisterSystemBitmap endp


	efile
	end

