	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Write some data to a file.
;
;Calling:
;
;WriteFile(handle,source,bytes);
;
;On Entry:
;
;handle	- file handle.
;source	- Address to write from.
;bytes	- Length to write.
;
;On Exit:
;
;EAX	- Bytes written.
;
_WriteFile	proc	syscall handle:dword, source:dword, bytes:dword
	public _WriteFile
	pushm	ebx,ecx,edx
	mov	ebx,handle
	mov	ecx,bytes
	mov	edx,source
	call	WriteFile
	popm	ebx,ecx,edx
	ret
_WriteFile	endp


	efile
	end


