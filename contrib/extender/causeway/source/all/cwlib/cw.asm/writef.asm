	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Write some data to a file.
;
;On Entry:
;
;EDX	- Address to write from.
;ECX	- Length to write.
;EBX	- file handle.
;
;On Exit:
;
;EAX	- Bytes written.
;
WriteFile	proc	near
	pushm	ecx,edx,esi
	xor	esi,esi
@@0:	pushm	ebx,ecx,edx,esi
	cmp	ecx,65535		;size of chunks to load.
	jc	@@1
	mov	ecx,65535		;as close to 64k as can get.
@@1:	mov	ah,40h
	int	21h		;read from the file.
	popm	ebx,ecx,edx,esi
	jc	@@2
	movzx	eax,ax		;get length read.
	add	esi,eax		;update length read counter.
	add	edx,eax		;move memory pointer.
	sub	ecx,eax		;update length counter.
	jz	@@2		;read as much as was wanted.
	or	eax,eax		;did we write anything?
	jz	@@2
	jmp	@@0
@@2:	mov	eax,esi
	popm	ecx,edx,esi
	ret
WriteFile	endp


	efile
	end

