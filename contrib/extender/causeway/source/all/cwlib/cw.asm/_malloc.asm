	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Allocate some DS relative memory.
;
;On Entry:
;
;bytes	- Bytes required.
;
;On Exit:
;
;Carry set on error and EAX=0 undefined else,
;
;EAX	- DS relative address of allocated memory.
;
;All other registers preserved.
;
_Malloc	proc	syscall bytes:dword
	public _Malloc
	pushm	ecx,esi
	mov	ecx,bytes
	call	Malloc
	mov	eax,esi
	popm	ecx,esi
	ret
_Malloc	endp


	efile
	end



