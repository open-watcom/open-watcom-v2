	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Get current drive and path.
;
;Calling:
;
;GetPath(buffer);
;
;On Entry:
;
;buffer	- pointer to buffer.
;
;On Exit:
;
;ALL registers preserved.
;
_GetPath	proc	syscall buffer:dword
	public _GetPath
	push	edx
	mov	edx,buffer
	call	GetPath
	pop	edx
	ret
_GetPath	endp


	efile
	end


