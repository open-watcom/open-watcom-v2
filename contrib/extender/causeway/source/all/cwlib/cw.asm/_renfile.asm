	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Rename a file.
;
;Calling:
;
;DeleteFile(source,dest);
;
;On Entry:
;
;source	- Source file name.
;dest	- New file name.
;
;On Exit:
;
;Carry set on error.
;
;ALL registers preserved.
;
_RenameFile	proc	syscall source:dword, dest:dword
	public _RenameFile
	pushm	edx,edi
	mov	edx,source
	mov	edi,dest
	call	RenameFile
	popm	edx,edi
	ret
_RenameFile	endp


	efile
	end



