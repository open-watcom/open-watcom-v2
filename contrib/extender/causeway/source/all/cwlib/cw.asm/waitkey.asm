	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Wait for a key press.
;
;On Entry:
;
;none.
;
;On Exit:
;
;EAX	- Key code, same format as GetKey.
;
WaitKey	proc	near
	mov	ah,0
	int	16h
	call	GetKey
	ret
WaitKey	endp


	efile
	end

