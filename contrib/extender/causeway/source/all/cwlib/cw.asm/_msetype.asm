	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set the type of graphic used for the mouse pointer. Only 2 are currently
;supported but others can be added to the table.
;
;Calling:
;
;MouseSetType(ptype);
;
;On Entry:
;
;type	- Pointer number.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_MouseSetType	proc	syscall ptype:dword
	public _MouseSetType
	push	eax
	mov	eax,ptype
	call	MouseSetType
	pop	eax
	ret
_MouseSetType	endp


	efile
	end

