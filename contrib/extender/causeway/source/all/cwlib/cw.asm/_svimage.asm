	include ..\cwlib.inc
	scode

;------------------------------------------------------------------------------
;
;Encodes and writes a bitmap in chosen format.
;
;Calling:
;
;SaveImageFile(flags,filename,bitmap);
;
;On Entry:
;
;flags	- flags, bit significant if set.
;	0 - compress output.
;	1 - alternate format.
;filename	- pointer to name of file to create.
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
_SaveImageFile	proc	syscall flags:dword, filename:dword, bitmap:dword
	public _SaveImageFile
	pushm	edx,esi
	mov	eax,flags
	mov	edx,filename
	mov	esi,bitmap
	call	SaveImageFile
	popm	edx,esi
	ret
_SaveImageFile	endp


	efile
	end

