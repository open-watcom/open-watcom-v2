	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set current drive and path.
;
;Calling:
;
;SetPath(buffer);
;
;On Entry:
;
;buffer	- pointer to path string.
;
;On Exit:
;
;ALL registers preserved.
;
_SetPath	proc	syscall buffer:dword
	public _SetPath
	push	edx
	mov	edx,buffer
	call	SetPath
	pop	edx
	ret
_SetPath	endp


	efile
	end


