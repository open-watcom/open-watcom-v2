	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Convert number into Hex ASCII.
;
;Calling:
;
;Bin2Hex(number,digits,buffer);
;
;On Entry:
;
;number	- Number to convert.
;digits	- digits to produce.
;buffer	- Buffer to put it in.
;
;On Exit:
;
;EAX	- Next available byte in buffer.
;
_Bin2Hex	proc	syscall number:dword, digits:dword, buffer:dword
	public _Bin2Hex
	pushm	ecx,edi
	mov	eax,number
	mov	ecx,digits
	mov	edi,buffer
	call	Bin2Dec
	mov	eax,edi
	popm	ecx,edi
	ret
_Bin2Hex	endp

	efile

	end




