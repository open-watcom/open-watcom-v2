	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get main width of a dialog.
;
;Calling:
;
;DialogGetWidth(dialog);
;
;On Entry:
;
;dialog	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Width.
;
;ALL other registers preserved.
;
_DialogGetWidth proc syscall dialog:dword
	public _DialogGetWidth
	push	esi
	mov	esi,dialog
	call	DialogGetWidth
	pop	esi
	ret
_DialogGetWidth endp


	efile
	end


