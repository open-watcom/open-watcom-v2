	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Convert character to upper case.
;
;Calling:
;
;UpperChar(charcter);
;
;On Entry:
;
;character	- character code.
;
;On Exit:
;
;AL	- upper case character code.
;
;ALL other registers preserved.
;
_UpperChar	proc	syscall character:dword
	public _UpperChar
	mov	al,b[character]
	call	UpperChar
	movzx	eax,al
	ret
_UpperChar	endp


	efile
	end


