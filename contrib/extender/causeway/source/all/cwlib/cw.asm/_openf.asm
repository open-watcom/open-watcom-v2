	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Open a file.
;
;On Entry:
;
;filename	- pointer to file name.
;
;On Exit:
;
;Carry set on error and EAX=0 else,
;
;EAX	- File handle.
;
_OpenFile	proc	syscall filename:dword
	public _OpenFile
	pushm	ebx,edx
	mov	edx,filename
	call	OpenFile
	mov	eax,ebx
	popm	ebx,edx
	ret
_OpenFile	endp


	efile
	end


