	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set the current drive.
;
;Calling:
;
;SetDrive(drive);
;
;On Entry:
;
;drive	- drive to set.
;
;On Exit:
;
;ALL registers preserved.
;
_SetDrive	proc	syscall drive:dword
	public _SetDrive
	push	eax
	mov	eax,drive
	call	SetDrive
	pop	eax
	ret
_SetDrive	endp


	efile
	end


