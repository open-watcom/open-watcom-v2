	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Loads and unpacks a TGA file.
;
;Calling:
;
;LoadTGA(filename,bitmap);
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
_LoadTGA	proc	syscall filename:dword, bitmap:dword
	public _LoadTGA
	pushm	edx,esi
	mov	edx,filename
	call	LoadTGA
	mov	edx,bitmap
	mov	[edx],esi
	popm	edx,esi
	ret
_LoadTGA	endp


	efile
	end


