	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Print null terminated string on screen via DOS.
;
;On Entry:
;
;ESI	- pointer to string to print.
;
PrintString	proc	near
	pushm	eax,esi,edx
l0:	mov	dl,[esi]
	inc	esi
	or	dl,dl
	jz	l2
	mov	ah,2
	int	21h
	jmp	l0
l2:	popm	eax,esi,edx
	ret
PrintString	endp


	efile
	end


