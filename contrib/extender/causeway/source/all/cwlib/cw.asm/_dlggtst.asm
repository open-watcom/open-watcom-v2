	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get state field of a dialog entry.
;
;Calling:
;
;DialogGetState(button,dialog);
;
;On Entry:
;
;button	- Entry number.
;dialog	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- State.
;
;ALL other registers preserved.
;
_DialogGetState proc syscall button:dword, dialog:dword
	public _DialogGetState
	pushm	ecx,esi
	mov	ecx,button
	mov	esi,dialog
	call	DialogGetState
	popm	ecx,esi
	ret
_DialogGetState endp


	efile
	end



