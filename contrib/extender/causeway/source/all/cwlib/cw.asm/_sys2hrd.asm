	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Convert system pen number into hardware pen number.
;
;Calling:
;
;VideoSys2Hard(pen);
;
;On Entry:
;
;pen	- pen number to convert.
;
;On Exit:
;
;AL	- new pen number.
;
;ALL other registers preserved.
;
_VideoSys2Hard	proc	syscall pen:dword
	public _VideoSys2Hard
	mov	eax,pen
	call	VideoSys2Hard
	ret
_VideoSys2Hard	endp


	efile
	end


