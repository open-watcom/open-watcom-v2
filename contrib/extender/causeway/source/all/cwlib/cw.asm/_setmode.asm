	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Attempt to set specified video mode.
;
;Calling:
;
;VideoSetMode(mode);
;
;On Entry:
;
;mode	- Mode to set.
;
;On Exit:
;
;EAX	- zero on error else mode number.
;
;ALL other registers preserved.
;
_VideoSetMode	proc	syscall mode:dword
	public _VideoSetMode
	mov	eax,mode
	call	VideoSetMode
	ret
_VideoSetMode	endp


	efile
	end


