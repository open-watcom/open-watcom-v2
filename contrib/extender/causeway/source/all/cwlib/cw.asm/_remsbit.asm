	include ..\cwlib.inc
	scode

;------------------------------------------------------------------------------
;
;Copy and re-map "system" bitmap duplicate.
;
;Calling:
;
;RemapSystemBitmap(tableptr);
;
;On Entry:
;
;tableptr	- pointer to "system" bitmap table entry.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_RemapSystemBitmap proc syscall tableptr:dword
	public _RemapSystemBitmap
	push	esi
	mov	esi,tableptr
	call	RemapSystemBitmap
	pop	esi
	ret
_RemapSystemBitmap endp


	efile
	end


