	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Wait for a key press.
;
;On Entry:
;
;none.
;
;On Exit:
;
;EAX	- Key code, same format as GetKey.
;
_WaitKey	proc	syscall
	public _WaitKey
	call	WaitKey
	ret
_WaitKey	endp


	efile
	end


