	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get main depth of a dialog.
;
;Calling:
;
;DialogGetDepth(dialog);
;
;On Entry:
;
;dialog	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Depth.
;
;ALL other registers preserved.
;
_DialogGetDepth proc syscall dialog:dword
	public _DialogGetDepth
	push	esi
	mov	esi,dialog
	call	DialogGetDepth
	pop	esi
	ret
_DialogGetDepth endp


	efile
	end



