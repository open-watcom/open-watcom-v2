	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Convert number into decimal ASCII.
;
;Calling:
;
;Bin2Dec(number,buffer);
;
;On Entry:
;
;number	- Number to convert.
;buffer	- Buffer to put it in.
;
;On Exit:
;
;EAX	- Next available byte in buffer.
;
_Bin2Dec	proc	syscall number:dword, buffer:dword
	public _Bin2Dec
	pushm	edi
	mov	eax,number
	mov	edi,buffer
	call	Bin2Dec
	mov	eax,edi
	pop	edi
	ret
_Bin2Dec	endp

	efile

	end



