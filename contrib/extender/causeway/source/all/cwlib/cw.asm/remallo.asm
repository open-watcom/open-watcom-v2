	include ..\cwlib.inc
	include ..\..\cw.inc
	scode

;-------------------------------------------------------------------------
;
;Re-size previously allocated memory.
;
;On Entry:
;
;ECX	- New size.
;ESI	- Current address of memory.
;
;On Exit:
;
;Carry set on error and ESI=0 else,
;
;ESI	- New address of memory.
;
;All other registers preserved.
;
ReMalloc	proc	near
	pushm	eax,edx
	add	esi,DataLinearBase
	sys	ResMemLinear32
	jc	l0
	sub	esi,DataLinearBase
	clc
	jmp	l1
l0:	xor	esi,esi
	stc
l1:	popm	eax,edx
	ret
ReMalloc	endp


	efile
	end

