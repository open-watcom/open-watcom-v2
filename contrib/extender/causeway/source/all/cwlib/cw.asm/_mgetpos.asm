	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Get the current position & button state for the mouse.
;
;Calling:
;
;MouseGet(mdet);
;
;On Entry:
;
;mdet	- pointer to 3 int's to fill in.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_MouseGet	proc	syscall mdet:dword
	public _MouseGet
	pushm	ebx,ecx,edx,esi
	call	MouseGet
	mov	esi,mdet
	mov	[esi+0],ebx
	mov	[esi+4],ecx
	mov	[esi+8],edx
	popm	ebx,ecx,edx,esi
	ret
_MouseGet	endp


	efile
	end

