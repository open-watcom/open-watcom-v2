	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Beep, useful for simplistic debugging.
;
;Calling:
;
;Beep();
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;ALL registers preserved.
;
_Beep	proc	syscall
	public _Beep
	call	Beep
	ret
_Beep	endp

	efile

	end


