	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Delete a file.
;
;On Entry:
;
;EDX	- File name to delete.
;
;On Exit:
;
;Carry set on error.
;
;ALL registers preserved.
;
DeleteFile	proc	near
	push	eax
	mov	ah,41h
	int	21h
	pop	eax
	ret
DeleteFile	endp


	efile
	end

