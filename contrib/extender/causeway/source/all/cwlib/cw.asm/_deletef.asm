	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Delete a file.
;
;Calling:
;
;DeleteFile(filename);
;
;On Entry:
;
;filename	- File name to delete.
;
;On Exit:
;
;Carry set on error and EAX=0 else EAX=-1.
;
_DeleteFile	proc	syscall filename:dword
	public _DeleteFile
	push	edx
	mov	edx,filename
	call	DeleteFile
	jc	l0
	mov	eax,-1
	jmp	l1
l0:	xor	eax,eax
l1:	pop	edx
	ret
_DeleteFile	endp


	efile
	end


