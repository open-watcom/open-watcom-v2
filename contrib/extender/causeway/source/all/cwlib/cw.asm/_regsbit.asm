	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Register a bitmap that needs to be re-mapped whenever the hardware palette
;changes so it stays in the system palette scheme.
;
;Calling:
;
;RegisterSystemBitmap(bitmap);
;
;On Entry:
;
;bitmap	- Pointer to bitmap.
;
;On Exit:
;
;EAX	- handle.
;
;ALL other registers preserved.
;
_RegisterSystemBitmap proc syscall bitmap:dword
	public _RegisterSystemBitmap
	push	esi
	mov	esi,bitmap
	call	RegisterSystemBitmap
	pop	esi
	ret
_RegisterSystemBitmap endp


	efile
	end


