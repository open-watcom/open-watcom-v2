	include ..\cwlib.inc
	scode

;------------------------------------------------------------------------------
;
;Re-map all bitmaps registered as useing "system" palette.
;
;Calling:
;
;RemapSystemBitmaps();
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_RemapSystemBitmaps proc syscall
	public _RemapSystemBitmaps
	call	RemapSystemBitmaps
	ret
_RemapSystemBitmaps endp


	efile
	end


