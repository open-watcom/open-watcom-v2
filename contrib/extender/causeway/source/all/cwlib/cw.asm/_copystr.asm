	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Copy a string from one place to another.
;
;On Entry:
;
;source	- Source string.
;destination	- Destination string.
;
;On Exit:
;
;ALL registers preserved.
;
_CopyString	proc	syscall source:dword, dest:dword
	public _CopyString
	pushm	esi,edi
	mov	esi,source
	mov	edi,dest
	call	CopyString
	popm	esi,edi
	ret
_CopyString	endp


	efile
	end


