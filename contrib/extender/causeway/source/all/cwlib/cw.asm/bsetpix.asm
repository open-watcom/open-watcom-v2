	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set a pixel to specified colour. Cheats by useing BitmapFillRectangle for now.
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
BitmapSetPixel	proc	near
	push	ebp
	push	1
	push	1
	push	edx
	push	ecx
	push	ebx
	push	eax
	call	BitmapFillRectangle
	add	esp,4*7
	ret
BitmapSetPixel	endp


	efile
	end

