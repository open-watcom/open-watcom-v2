	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Draw a dialog entry.
;
;Calling:
;
;DrawDialog(button,dialog,window);
;
;On Entry:
;
;button	- Button number to draw, -1 for all.
;dialog	- Dialog table to draw from.
;window	- Window to draw in.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_DrawDialog	proc	syscall button:dword, dialog:dword, window:dword
	public _DrawDialog
	pushm	ecx,esi,edi
	mov	ecx,button
	mov	esi,dialog
	mov	edi,window
	call	DrawDialog
	popm	ecx,esi,edi
	ret
_DrawDialog	endp


	efile
	end


