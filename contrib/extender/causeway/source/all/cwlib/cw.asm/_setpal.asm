	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set hardware palette. Generates new system to hardware translation table
;and re-maps all "system" bitmaps.
;
;Calling:
;
;VideoSetPalette(base,count,palette);
;
;On Entry:
;
;base	- Base pen to set.
;count	- Number of pens to set.
;palette	- pointer to palette data.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_VideoSetPalette proc syscall base:dword, count:dword, palette:dword
	public _VideoSetPalette
	pushm	eax,ecx,esi
	mov	eax,base
	mov	ecx,count
	mov	esi,palette
	call	VideoSetPalette
	popm	eax,ecx,esi
	ret
_VideoSetPalette endp


	efile
	end


