	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Send a message to a window.
;
;Calling:
;
;WinMessageWindow(message,window);
;
;On Entry:
;
;message	- message to send.
;window	- window to send it to.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_WinMessageWindow proc syscall message:dword, window:dword
	pushm	eax,esi
	mov	eax,message
	mov	esi,window
	call	WinMessageWindow
	popm	eax,esi
	ret
_WinMessageWindow endp


	efile
	end


