	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Wait for a while.
;
;Calling:
;
;Delay(count);
;
;On Entry:
;
;count	- time to wait for.
;
_Delay	proc	syscall count:dword
	public _Delay
	push	ecx
	mov	ecx,count
	call	Delay
	pop	ecx
	ret
_Delay	endp


	efile
	end



