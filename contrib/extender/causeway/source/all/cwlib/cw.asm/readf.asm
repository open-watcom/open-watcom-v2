	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Read some data from a file.
;
;On Entry:
;
;EDX	- Address to read to.
;ECX	- length to read.
;EBX	- file handle.
;
;On Exit:
;
;EAX	- bytes read.
;
ReadFile	proc	near
	pushm	ecx,edx,esi
	xor	esi,esi		;reset length read.
@@0:	pushm	ebx,ecx,edx,esi
	cmp	ecx,65535		;size of chunks to load.
	jc	@@1
	mov	ecx,65535		;as close to 64k as can get.
@@1:	mov	ah,3fh
	int	21h		;read from the file.
	popm	ebx,ecx,edx,esi
	jc	@@2		;DOS error so exit NOW.
	movzx	eax,ax		;get length read.
	add	esi,eax		;update length read counter.
	add	edx,eax		;move memory pointer.
	sub	ecx,eax		;update length counter.
	jz	@@2		;read as much as was wanted.
	or	eax,eax		;did we read anything?
	jnz	@@0
@@2:	mov	eax,esi
	popm	ecx,edx,esi
	ret
ReadFile	endp


	efile
	end

