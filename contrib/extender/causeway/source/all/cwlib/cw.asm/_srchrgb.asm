	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Find closest match in HardwarePalette to RGB value specified. Must be more
;accurate ways of doing it than this but I don't know about them!
;
;Calling:
;
;SearchRGB(rgbvalue);
;
;On Entry:
;
;rgbvalue	- RGB value.
;
;On Exit:
;
;EAX	- Nearest pen value in HardwarePalette
;
_SearchRGB	proc	syscall rgbvalue:dword
	public _SearchRGB
	mov	eax,rgbvalue
	call	SearchRGB
	ret
_SearchRGB	endp


	efile
	end



