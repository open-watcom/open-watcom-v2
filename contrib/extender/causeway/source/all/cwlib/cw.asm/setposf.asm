	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set the file pointer position for a file.
;
;On Entry:
;
;AL	- method.
;EBX	- handle.
;ECX	- position.
;
;Methods are:
;
;0	- Absolute offset from start.
;1	- signed offset from current position.
;2	- signed offset from end of file.
;
;On Exit:
;
;ECX	- absolute offset from start of file.
;
SetFilePointer	proc	near
	pushm	eax,edx
	mov	dx,cx
	shr	ecx,16
	mov	ah,42h		;set pointer function.
	int	21h
	mov	cx,dx
	shl	ecx,16
	mov	cx,ax		;fetch small result.
	popm	eax,edx
	ret
SetFilePointer	endp


	efile
	end

