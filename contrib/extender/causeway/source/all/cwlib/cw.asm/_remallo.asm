	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Re-size previously allocated memory.
;
;On Entry:
;
;memory	- current address of memory
;bytes	- new size.
;
;On Exit:
;
;Carry set on error and EAX=0 else,
;
;EAX	- New address of memory.
;
;All other registers preserved.
;
_ReMalloc	proc	syscall memory:dword, bytes:dword
	public _ReMalloc
	pushm	ecx,esi
	mov	esi,memory
	mov	ecx,bytes
	call	ReMalloc
	mov	eax,esi
	popm	ecx,esi
	ret
_ReMalloc	endp


	efile
	end


