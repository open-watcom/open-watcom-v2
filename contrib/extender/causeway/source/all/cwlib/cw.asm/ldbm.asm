	include ..\cwlib.inc


	scode


;******************************************************************************
;Loads and unpacks a BM image file.
;
;On Entry:
;
;EDX	- pointer to name of file to load.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- Bitmap pointer.
;
;ALL other registers preserved.
;
;******************************************************************************
LoadBM	proc	near
	public LoadBM
	local @@bitmap:dword, @@handle:dword, @@flags:dword
	pushad
	mov	@@handle,0
	mov	@@bitmap,0
;
;Open the file.
;
	call	OpenFile
	jc	@@file_error
	mov	@@Handle,ebx
;
;Read flags dword.
;
	lea	edx,@@flags
	mov	ecx,4
	call	ReadFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
;
;See how long it is.
;
	xor	ecx,ecx
	mov	al,2
	call	SetFilePointer
	push	ecx
	xor	al,al
	xor	ecx,ecx
	call	SetFilePointer
	pop	ecx
;
;Allocate some memory for it.
;
	mov	edi,ecx
	sub	edi,4+4+4+4
	add	ecx,4+4+4+4
	add	ecx,256		;make space for the file name.
	test	@@flags,7		;8-bit?
	jz	@@nopal
	add	ecx,256*3
@@nopal:	call	Malloc
	jc	@@mem_error
	mov	@@bitmap,esi
	mov	ecx,edi
;
;Clear the header
;
	push	ecx
	mov	edi,esi
	xor	eax,eax
	mov	ecx,size BM
	rep	stosb
	pop	ecx
;
;Read the header.
;
	push	ecx
	mov	edx,esi
	mov	ecx,4*4
	call	ReadFile
	pop	ecx
	jc	@@file_error
	cmp	eax,4*4
	jnz	@@file_error
;
;Read it in.
;
	mov	edx,esi
	add	edx,size BM
	call	ReadFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
;
;Close it & done.
;
	call	CloseFile
	xor	ebx,ebx
	mov	@@handle,ebx
	jmp	@@exit
;
@@file_error:	mov	ebx,1
	jmp	@@exit
;
@@mem_error:	mov	ebx,2
	jmp	@@exit
;
@@exit:	xchg	ebx,@@handle
	or	ebx,ebx
	jz	@@closed
	call	CloseFile
@@closed:	cmp	@@handle,0
	jz	@@freed
	xor	esi,esi
	xchg	esi,@@bitmap
	or	esi,esi
	jz	@@freed
	call	Free
@@freed:	popad
	mov	eax,@@handle
	mov	esi,@@bitmap
	ret
LoadBM	endp


	efile
	end

