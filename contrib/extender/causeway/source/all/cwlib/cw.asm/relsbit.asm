	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Release a bitmap previously registered with the bitmap manager. Duplicate
;memory is free'd and entry in table is marked free.
;
;On Entry:
;
;ESI	- Handle
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
ReleaseSystemBitmap proc near
	push	esi
	mov	esi,BMT_Duplicate[esi]
	call	Free
	pop	esi
	mov	BMT_Flags[esi],0
	ret
ReleaseSystemBitmap endp


	efile
	end

