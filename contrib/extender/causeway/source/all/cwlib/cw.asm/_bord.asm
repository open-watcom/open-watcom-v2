	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Change the border colour. Provided mainly for simplistic debugging.
;
;Calling:
;
;Bord(pen);
;
;On Entry:
;
;pen	- colour to set.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_Bord	proc	syscall pen:dword
	public _Bord
	push	eax
	mov	eax,pen
	call	bord
	pop	eax
	ret
_Bord	endp


	efile
	end


