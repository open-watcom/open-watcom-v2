	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Build a list of all valid drives on the system.
;
;Calling:
;
;GetDrives(buffer);
;
;On Entry:
;
;buffer	- Address to build list of drives.
;
;On Exit:
;
;EAX	- number of drives.
;
;ALL other registers preserved.
;
;The drive list uses real drive codes (not ASCII). Each entry uses 1 byte
;and the list is terminated with -1.
;
_GetDrives	proc	syscall buffer:dword
	public _GetDrives
	pushm	ecx,edx
	mov	edx,buffer
	call	GetDrives
	mov	eax,ecx
	popm	ecx,edx
	ret
_GetDrives	endp


	efile
	end


