	include ..\cwlib.inc
	scode

;------------------------------------------------------------------------------
;
;Encodes and writes a bitmap in IFF format.
;
;Calling:
;
;SaveIFF(flags,filename,bitmap);
;
;On Entry:
;
;flags	- flags, bit significant if set.
;	0 - compress output.
;	1 - old format.
;filename	- pointer to name of file to load.
;bitmap	- pointer to bitmap.
;
;On Exit:
;
;EAX	- Status,
;	0 - no error.
;	1 - file error.
;	2 - not enough memory (for buffers).
;	3 - format error.
;
;ALL other registers preserved.
;
_SaveIFF	proc	syscall flags:dword, filename:dword, bitmap:dword
	public _SaveIFF
	pushm	edx,esi
	mov	eax,flags
	mov	edx,filename
	mov	esi,bitmap
	call	SaveIFF
	popm	edx,esi
	ret
_SaveIFF	endp


	efile
	end

