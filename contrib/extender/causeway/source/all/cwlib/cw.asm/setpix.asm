	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set a pixel to specified colour. Cheats by useing VideoFillRectangle for now.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - FillValue is RGB.
;EBX	- Fill value.
;ECX	- X co-ord.
;EDX	- Y co-ord.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
VideoSetPixel	proc	near
	push	1
	push	1
	push	edx
	push	ecx
	push	ebx
	push	eax
	call	VideoFillRectangle
	add	esp,4*6
	ret
VideoSetPixel	endp


	efile
	end
