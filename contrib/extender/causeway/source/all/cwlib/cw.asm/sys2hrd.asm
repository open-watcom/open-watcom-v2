	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Convert system pen number into hardware pen number.
;
;On Entry:
;
;AL	- pen number to convert.
;
;On Exit:
;
;AL	- new pen number.
;
;ALL other registers preserved.
;
VideoSys2Hard	proc
	push	ebx
	mov	ebx,offset SystemXlat
	xlat
	pop	ebx
	ret
VideoSys2Hard	endp


	efile
	end

