	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get Y coord of a dialog entry.
;
;Calling:
;
;DialogGetButtonY(button,dialog);
;
;On Entry:
;
;button	- Entry number.
;dialog	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Y coord.
;
;ALL other registers preserved,
;
_DialogGetButtonY proc syscall button:dword, dialog:dword
	public _DialogGetButtonY
	pushm	ecx,esi
	mov	ecx,button
	mov	esi,dialog
	call	DialogGetButtonY
	popm	ecx,esi
	ret
_DialogGetButtonY endp


	efile
	end


