	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Set state field of a dialog entry.
;
;Calling:
;
;DialogSetState(state,button,dialog);
;
;On Entry:
;
;state	- State to set.
;button	- Entry number.
;dialog	- Pointer to dialog structure.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_DialogSetState proc	syscall state:dword, button:dword, dialog:dword
	public _DialogSetState
	pushm	eax,ecx,esi
	mov	eax,state
	mov	ecx,button
	mov	esi,dialog
	call	DialogSetState
	popm	eax,ecx,esi
	ret
_DialogSetState endp


	efile
	end


