	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Convert a string into upper case.
;
;Inputs:
;
;string	- String to convert.
;
_UpperString	proc	syscall string:dword
	public _UpperString
	push	esi
	mov	esi,string
	call	UpperString
	pop	esi
	ret
_UpperString	endp


	efile
	end


