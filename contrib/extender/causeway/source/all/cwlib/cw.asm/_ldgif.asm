	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Loads and unpacks a GIF file.
;
;Calling:
;
;LoadGIF(filename,bitmap);
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
;bitmap	- Bitmap pointer.
;
;ALL other registers preserved.
;
_LoadGIF	proc	syscall filename:dword, bitmap:dword
	public _LoadGIF
	pushm	edx,esi
	mov	edx,filename
	call	LoadGIF
	mov	edx,bitmap
	mov	[edx],esi
	popm	edx,esi
	ret
_LoadGIF	endp


	efile
	end


