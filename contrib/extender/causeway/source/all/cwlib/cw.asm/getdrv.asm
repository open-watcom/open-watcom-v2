	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Get the current drive.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;AL	- Drive code.
;
GetDrive	proc	near
	mov	ah,19h	;get current disc
	int	21h
	movzx	eax,al
	ret
GetDrive	endp


	efile
	end

