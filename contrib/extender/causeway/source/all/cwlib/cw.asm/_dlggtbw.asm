	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get width of a dialog entry.
;
;Calling:
;
;DialogGetButtonW(button,dialog);
;
;On Entry:
;
;button	- Entry number.
;dialog	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Width.
;
;ALL other registers preserved.
;
_DialogGetButtonW proc syscall button:dword, dialog:dword
	public _DialogGetButtonW
	pushm	ecx,esi
	mov	ecx,button
	mov	esi,dialog
	call	DialogGetButtonW
	popm	ecx,esi
	ret
_DialogGetButtonW endp


	efile
	end



