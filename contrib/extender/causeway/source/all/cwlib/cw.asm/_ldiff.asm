	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Loads and unpacks an IFF file.
;
;Calling:
;
;LoadIFF(filename,bitmap);
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
_LoadIFF	proc	syscall filename:dword, bitmap:dword
	public _LoadIFF
	pushm	edx,esi
	mov	edx,filename
	call	LoadIFF
	mov	edx,bitmap
	mov	[edx],esi
	popm	edx,esi
	ret
_LoadIFF	endp


	efile
	end

