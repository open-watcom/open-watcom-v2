	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Load an image file.
;
;Calling:
;
;LoadImageFile(filename,bitmap);
;
;On Entry:
;
;filename	- pointer to name of file to load.
;bitmap	- pointer to variable to store bitmap address in.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - Bad file format.
;	2 - File access error.
;	3 - Not enough memory.
;bitmap	- pointer to bitmap.
;
_LoadImageFile	proc	syscall filename:dword, bitmap:dword
	public _LoadImageFile
	pushm	edx,esi
	mov	edx,filename
	call	LoadImageFile
	mov	edx,bitmap
	mov	[edx],esi
	popm	edx,esi
	ret
_LoadImageFile	endp


	efile
	end

