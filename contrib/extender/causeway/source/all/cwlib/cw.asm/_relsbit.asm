	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Release a bitmap previously regeistered with the bitmap manager. Duplicate
;memory is free'd and entry in table is marked free.
;
;Calling:
;
;ReleaseSystemBitmap(handle);
;
;On Entry:
;
;handle	- Handle returned by RegisterSystemBitmap
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_ReleaseSystemBitmap proc syscall handle:dword
	public _ReleaseSystemBitmap
	push	esi
	mov	esi,handle
	call	ReleaseSystemBitmap
	pop	esi
	ret
_ReleaseSystemBitmap endp


	efile
	end


