	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Beep, useful for simplistic debugging or indicateing an error.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;ALL registers preserved.
;
beep	proc	near
	pushm	eax,edx
	mov	dl,7
	mov	ah,2
	int	21h
	popm	eax,edx
	ret
beep	endp


	efile
	end

