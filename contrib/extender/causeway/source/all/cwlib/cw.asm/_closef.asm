	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Close a file.
;
;Calling:
;
;CloseFile(handle);
;
;On Entry:
;
;handle	- file handle.
;
_CloseFile	proc	syscall handle:dword
	public _CloseFile
	push	ebx
	mov	ebx,handle
	call	CloseFile
	pop	ebx
	ret
_CloseFile	endp


	efile
	end


