	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Initialise video stuff, includeing VESA extension.
;
;Calling:
;
;VideoInitialise();
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error and EAX=error code.
;
;Error codes:
;
;0	- no error.
;1	- couldn't allocate selector for video memory.
;2	- not enough memory for bitmap control table.
;3	- video system not supported, ie, not (S)VGA.
;
;ValidModeList will be filled in with all modes supported.
;
_VideoInitialise proc syscall
	public _VideoInitialise
	call	VideoInitialise
	ret
_VideoInitialise endp


;-------------------------------------------------------------------------
;
;Remove video stuff, includeing VESA support if installed.
;
;Calling:
;
;VideoRemove();
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;ALL registers preserved.
;
_VideoRemove	proc	syscall
	public _VideoRemove
	call	VideoRemove
	ret
_VideoRemove	endp


	efile
	end

