	include ..\cwlib.inc
	include ..\..\cw.inc
	scode

;-------------------------------------------------------------------------
;
;Allocate some DS relative memory.
;
;On Entry:
;
;ECX	- Bytes required.
;
;On Exit:
;
;Carry set on error and ESI=0 else,
;
;ESI	- DS relative address of allocated memory.
;
;All other registers preserved.
;
Malloc	proc	near
	push	eax
	sys	GetMemLinear32
	jc	l0
	sub	esi,DataLinearBase
	clc
	jmp	l1
l0:	xor	esi,esi
	stc
l1:	pop	eax
	ret
Malloc	endp


	efile
	end


