	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set the current drive.
;
;On Entry:
;
;AL	- drive to set.
;
;On Exit:
;
;ALL registers preserved.
;
SetDrive	proc	near
	pushm	eax,edx
	mov	dl,al
	mov	ah,0eh	;Set drive
	int	21h
	popm	eax,edx
	ret
SetDrive	endp


	efile
	end

