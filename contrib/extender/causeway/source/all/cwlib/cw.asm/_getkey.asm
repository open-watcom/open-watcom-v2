	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Get key press info.
;
;Calling:
;
;GetKey();
;
;On Entry:
;
;none.
;
;On Exit:
;
;AL	- ASCII character code.
;AH	- Hardware key code.
;EAX high word	- Shift keys status word from 417h.
;
_GetKey	proc	syscall
	public _GetKey
	call	GetKey
	ret
_GetKey	endp


	efile
	end


