	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Print null terminated string on screen via DOS.
;
;Inputs:
;
;stringptr	- pointer to string.
;
_PrintString	proc syscall string:dword
	public _PrintString
	push	esi
	mov	esi,string
	call	PrintString
	pop	esi
	ret
_PrintString	endp


	efile
	end


