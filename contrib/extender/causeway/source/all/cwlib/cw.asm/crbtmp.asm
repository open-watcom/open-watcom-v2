	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Create a new bitmap.
;
;On Entry:
;
;EBX	- bitmap flags entry.
;ECX	- bitmap width.
;EDX	- bitmap depth.
;
;On Exit:
;
;Carry set on error and EAX=0 else,
;
;EAX	- pointer to bitmap.
;
;ALL other registers preserved.
;
CreateBitmap	proc near
	pushm	ebx,ecx,edx,esi,edi
	pushm	ebx,ecx,edx
	xor	eax,eax		;Work out bytes per pixel
	inc	eax		;needed and how much memory
	and	ebx,15		;we need to allocate.
	jz	@@0		;8-bit?
	inc	eax
	cmp	ebx,2+1		;15- or 16-bit?
	jc	@@0
	inc	eax		;must be 24-bit then.
@@0:	mov	edi,eax
	mul	edx
	mul	ecx
	add	eax,size BM		;allow for header.
	add	eax,256*3		;leave space for 256 colour palette.
	add	eax,256		;leave space for path.
	mov	ecx,eax
	call	Malloc		;try and allocate the memory.
	popm	ebx,ecx,edx
	jc	@@9
	mov	BM_Flags[esi],ebx
	mov	BM_PWide[esi],edi
	mov	BM_Wide[esi],ecx
	mov	BM_Depth[esi],edx
	mov	BM_XOffset[esi],0
	mov	BM_YOffset[esi],0
	;
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	add	eax,esi
	add	eax,size BM
	mov	edi,eax
	push	esi
	mov	esi,offset HardwarePalette
	mov	ecx,256*3
	rep	movsb
	pop	esi
	;
	clc
@@9:	mov	eax,esi
	popm	ebx,ecx,edx,esi,edi
	ret
CreateBitmap	endp


	efile
	end

