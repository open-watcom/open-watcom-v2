	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Open a file.
;
;On Entry:
;
;EDX	- File name.
;
;On Exit:
;
;Carry set on error and EBX=0 else,
;
;EBX	- File handle.
;
OpenFile	proc	near
	push	eax
	mov	ax,3d02h		;Open with read & write access.
	int	21h
	mov	bx,ax
	jnc	l0
	xor	bx,bx
	stc
l0:	pop	eax
	movzx	ebx,bx
	ret
OpenFile	endp


	efile
	end


