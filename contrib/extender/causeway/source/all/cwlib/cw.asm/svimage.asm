	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Save an image file.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - compress
;	1 - alternate format 1
;ECX	- Format to use,
;	0 - auto
;	1 - IFF/LBM/ILB
;	2 - GIF
;	3 - TGA
;	4 - PCX
;	5 - BM
;EDX	- pointer to name of file to create.
;ESI	- pointer to bitmap.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - format error.
;
;All other registers preserved.
;
SaveImageFile	proc	near
	pushm	ebx,ecx,edx,esi,edi,ebp
	;
	or	ecx,ecx
	jz	@@auto
	dec	ecx
	jz	@@iff
	dec	ecx
	jz	@@gif
	dec	ecx
	jz	@@tga
	dec	ecx
	jz	@@pcx
	dec	ecx
	jz	@@BM
	jmp	@@type_error
	;
@@auto:	mov	_SIF_Exten,'    '
	pushm	eax,esi
	mov	esi,edx
	mov	edi,-1
@@2:	lodsb
	cmp	al,'.'
	jz	@@3
	or	al,al
	jnz	@@2
	jmp	@@4
@@3:	mov	edi,esi
	jmp	@@2
@@4:	popm	eax,esi
	cmp	edi,-1
	jz	@@type_error
	pushm	eax,esi
	mov	esi,edi
	mov	eax,[esi]
	and	eax,0ffffffh
	call	UpperChar
	ror	eax,8
	call	UpperChar
	ror	eax,8
	call	UpperChar
	rol	eax,16
	mov	_SIF_Exten,eax
	popm	eax,esi
	;
	cmp	_SIF_Exten,"FFI"
	jz	@@iff
	cmp	_SIF_Exten,"MBL"
	jz	@@iff
	cmp	_SIF_Exten,"BLI"
	jz	@@iff
	cmp	_SIF_Exten,"FIG"
	jz	@@gif
	cmp	_SIF_Exten,"AGT"
	jz	@@tga
	cmp	_SIF_Exten,"XCP"
	jz	@@pcx
	cmp	_SIF_Exten,"MB"
	jz	@@bm
	cmp	_SIF_Exten," MB"
	jz	@@bm
	jmp	@@type_error
	;
@@iff:	call	SaveIFF
	jmp	@@exit
	;
@@gif:	call	SaveGIF
	jmp	@@exit
	;
@@tga:	call	SaveTGA
	jmp	@@exit
	;
@@pcx:	jmp	@@type_error
	;
@@bm:	call	SaveBM
	jmp	@@exit
	;
@@type_error:	mov	eax,3
	;
@@exit:	popm	ebx,ecx,edx,esi,edi,ebp
	ret
SaveImageFile	endp


	sdata
;
_SIF_Exten	dd ?


	efile
	end

