	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Create a file.
;
;Calling:
;
;CreateFile(filename);
;
;On Entry:
;
;filename	- File name.
;
;On Exit:
;
;Carry set on error and EAX=0 else,
;
;EAX	- File handle.
;
_CreateFile	proc	syscall filename:dword
	public _CreateFile
	pushm	ebx,edx
	mov	edx,filename
	call	CreateFile
	mov	eax,ebx
	popm	ebx,edx
	ret
_CreateFile	endp


	efile
	end


