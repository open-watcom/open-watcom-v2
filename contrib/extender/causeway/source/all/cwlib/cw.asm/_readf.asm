	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Read some data from a file.
;
;Calling:
;
;ReadFile(handle,dest,bytes);
;
;On Entry:
;
;handle	- file handle.
;dest	- Address to read to.
;bytes	- length to read.
;
;On Exit:
;
;EAX	- bytes read.
;
_ReadFile	proc	syscall handle:dword, dest:dword, bytes:dword
	public _ReadFile
	pushm	ebx,ecx,edx
	mov	ebx,handle
	mov	ecx,bytes
	mov	edx,dest
	call	ReadFile
	popm	ebx,ecx,edx
	ret
_ReadFile	endp


	efile
	end


