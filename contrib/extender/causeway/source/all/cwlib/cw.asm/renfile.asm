	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Rename a file.
;
;On Entry:
;
;EDX	- Source file name.
;EDI	- New file name.
;
;On Exit:
;
;Carry set on error.
;
;ALL registers preserved.
;
RenameFile	proc	near
	push	eax
	mov	ah,56h
	int	21h
	pop	eax
	ret
RenameFile	endp


	efile
	end


