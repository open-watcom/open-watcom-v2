	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set a pixel to specified colour. Cheats by useing VideoFillRectangle for now.
;
;On Entry:
;
;C style stack with following parameters,
;
;flags	- Flags, bit significant if set.
;	0 - FillValue is RGB.
;fillvalue	- Fill value.
;xcoord	- X co-ord.
;ycoord	- Y co-ord.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
_VideoSetPixel:
	public _VideoSetPixel
_VideoSetPixel_ proc	syscall flags:dword, fillvalue:dword, xcoord:dword, ycoord:dword
	pushm	eax,ebx,ecx,edx
	mov	eax,flags
	mov	ebx,fillvalue
	mov	ecx,xcoord
	mov	edx,ycoord
	call	VideoSetPixel
	popm	eax,ebx,ecx,edx
	ret
_VideoSetPixel_ endp


	efile
	end

