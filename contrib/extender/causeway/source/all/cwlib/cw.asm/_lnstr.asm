	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Work out length of a string.
;
;Calling:
;
;LenString(stringptr);
;
;On Entry:
;
;stringptr	- pointer to string.
;
;On Exit:
;
;EAX	- length of string.
;
;ALL other registers preserved.
;
_LenString	proc	syscall stringptr:dword
	public _LenString
	push	esi
	mov	esi,stringptr
	call	LenString
	pop	esi
	ret
_LenString	endp


	efile
	end


