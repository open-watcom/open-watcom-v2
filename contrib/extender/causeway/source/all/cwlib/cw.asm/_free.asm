	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Release some memory.
;
;On Entry:
;
;memory	- Address to release.
;
;On Exit:
;
;All registers preserved.
;
_Free	proc	syscall memory:dword
	public _Free
	push	esi
	mov	esi,memory
	call	Free
	pop	esi
	ret
_Free	endp


	efile
	end


