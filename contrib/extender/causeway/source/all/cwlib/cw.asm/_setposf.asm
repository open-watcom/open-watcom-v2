	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set the file pointer position for a file.
;
;Calling:
;
;SetFilePointer(handle,method,position);
;
;On Entry:
;
;handle	- file handle.
;method	- method to use.
;position	- position to set.
;
;Methods are:
;
;0	- Absolute offset from start.
;1	- signed offset from current position.
;2	- signed offset from end of file.
;
;On Exit:
;
;EAX	- new absolute offset from start of file.
;
_SetFilePointer proc	syscall handle:dword, method:dword, position:dword
	public _SetFilePointer
	pushm	ebx,ecx
	mov	eax,method
	mov	ebx,handle
	mov	ecx,position
	call	SetFilePointer
	mov	eax,ecx
	popm	ebx,ecx
	ret
_SetFilePointer endp


	efile
	end


