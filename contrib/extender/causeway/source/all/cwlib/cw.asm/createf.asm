	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Create a file.
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
CreateFile	proc	near
	pushm	eax,ecx
	mov	ah,3ch		;Create function.
	xor	cx,cx		;normal attributes.
	int	21h
	mov	bx,ax
	jnc	l0
	xor	bx,bx
	stc
l0:	movzx	ebx,bx
	popm	eax,ecx
	ret
CreateFile	endp


	efile
	end

