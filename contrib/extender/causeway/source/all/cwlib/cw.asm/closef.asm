	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Close a file.
;
;On Entry:
;
;EBX	- file handle.
;
CloseFile	proc	near
	push	eax
	mov	ah,3eh
	int	21h
	pop	eax
	ret
CloseFile	endp


	efile
	end

