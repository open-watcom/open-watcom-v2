	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get X coord of a dialog entry.
;
;Calling:
;
;DialogGetButtonX(button,dialog);
;
;On Entry:
;
;button	- Entry number.
;dialog	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- X coord.
;
;ALL other registers preserved.
;
_DialogGetButtonX proc syscall button:dword, dialog:dword
	public _DialogGetButtonX
	pushm	ecx,esi
	mov	ecx,button
	mov	esi,dialog
	call	_DialogGetButtonX
	popm	ecx,esi
	ret
_DialogGetButtonX endp


	efile
	end




