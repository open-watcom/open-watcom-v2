	include ..\cwlib.inc
	scode


;******************************************************************************
;Encodes and writes a bitmap in BM format.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - compress output (ignored).
;EDX	- file name.
;ESI	- Bitmap to write.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file error.
;	2 - not enough memory (for buffers).
;	3 - format error.
;
;All other registers preserved.
;
;******************************************************************************
SaveBM	proc	near
	local @@handle:dword, @@flags:dword
	pushad
	mov	@@flags,eax
	mov	@@handle,0
;
;Try and create the file.
;
	call	CreateFile
	jc	@@file_error
	mov	@@handle,ebx
;
;Work out how much data we need to write.
;
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
;
;Need a palette?
;
	test	BM_Flags[esi],7
	jnz	@@0
	add	eax,256*3
;
;Write the header.
;
@@0:	push	eax
	mov	edx,esi
	mov	ecx,4+4+4+4
	call	WriteFile
	pop	ecx
	jc	@@file_error
	cmp	eax,4+4+4+4
	jnz	@@file_error
;
;Write the main data.
;
	mov	edx,esi
	add	edx,size BM
	call	WriteFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
;
;Close the file & done.
;
	call	CloseFile
	xor	ebx,ebx
	mov	@@handle,ebx
	jmp	@@10
;
@@mem_error:	mov	ebx,2
	jmp	@@error
;
@@file_error:	mov	ebx,1
	jmp	@@error
;
@@error:	stc
;
@@10:	pushf
	xchg	ebx,@@handle
	or	ebx,ebx
	jz	@@10_0
	call	CloseFile
@@10_0:	popf
	popad
	mov	eax,@@handle
	ret
SaveBM	endp


	efile
	end


