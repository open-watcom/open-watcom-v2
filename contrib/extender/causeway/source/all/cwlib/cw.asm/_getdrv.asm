	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Get the current drive.
;
;Calling:
;
;GetDrive();
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;AL	- Drive code.
;
_GetDrive	proc	syscall
	public _GetDrive
	call	GetDrive
	ret
_GetDrive	endp


	efile
	end


