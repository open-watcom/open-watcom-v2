	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Send a message to a window.
;
;On Entry:
;
;EAX	- message
;ESI	- window
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
WinMessageWindow proc near
	pushad
	cmp	Win_Handler[esi],0
	jz	@@9
	call	Win_Handler[esi]
@@9:	popad
	ret
WinMessageWindow endp


	efile
	end

