	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get depth of a dialog entry.
;
;Calling:
;
;DialogGetButtonD(button,dialog);
;
;On Entry:
;
;button	- Entry number.
;dialog	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Depth.
;
;ALL other registers preserved.
;
_DialogGetButtonD proc syscall button:dword, dialog:dword
	public _DialogGetButtonD
	pushm	ecx,esi
	mov	ecx,button
	mov	esi,dialog
	call	DialogGetButtonD
	popm	ecx,esi
	ret
_DialogGetButtonD endp


	efile
	end




