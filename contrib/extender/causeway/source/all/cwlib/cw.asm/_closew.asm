	include ..\cwlib.inc
	scode

;------------------------------------------------------------------------------
;
;Close a window and update the area underneath it if possible.
;
;Calling:
;
;WinCloseWindow(handle);
;
;On Entry:
;
;handle	- Pointer to window structure (handle).
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_WinCloseWindow proc syscall handle:dword
	public _WinCloseWindow
	push	eax
	mov	eax,handle
	call	WinCloseWindow
	pop	eax
	ret
_WinCloseWindow endp


	efile
	end

