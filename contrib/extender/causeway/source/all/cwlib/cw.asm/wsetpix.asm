	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set a pixel to specified colour. Cheats by useing WinFillRectangle for now.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - FillValue is RGB.
;EBX	- Fill value.
;ECX	- X co-ord.
;EDX	- Y co-ord.
;EBP	- Bitmap.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
WinSetPixel	proc	near
	push	1
	push	1
	push	edx
	push	ecx
	push	ebx
	push	eax
	push	ebp
	call	WinFillRectangle
	add	esp,4*7
	ret
WinSetPixel	endp


	efile
	end


