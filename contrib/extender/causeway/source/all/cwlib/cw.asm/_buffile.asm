	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Initialise file buffer.
;
;Calling:
;
;InitFileBuffer(handle);
;
;On Entry:
;
;handle	- File handle.
;
;On Exit:
;
;ALL registers preserved.
;
_InitFileBuffer proc	syscall handle:dword
	public _InitFileBuffer
	push	ebx
	mov	ebx,handle
	call	InitFileBuffer
	pop	ebx
	ret
_InitFileBuffer endp


;-------------------------------------------------------------------------
;
;Read a byte from the file buffer.
;
;Calling:
;
;ReadBufferByte();
;
;On Entry:
;
;None
;
;On Exit:
;
;AH	- Bytes read, ie, AH=0 indicates EOF or an error.
;AL	- Byte read.
;
;High word of EAX is zero'd.
;
;eg, if AX=0 then EOF reached.
;
;Don't mix normal ReadFile call's with this call, they don't take account of
;each other.
;
_ReadBufferByte proc	syscall
	public _ReadBufferByte
	call	ReadBufferByte
	movzx	eax,ax
	ret
_ReadBufferByte endp


;-------------------------------------------------------------------------
;
;Read a line of text from input file specified.
;
;Calling:
;
;ReadBufferLine(buffer);
;
;On Entry:
;
;buffer	- Buffer to put line into.
;
;On Exit:
;
;EAX	- Bytes read, 0 = EOF or error.
;
_ReadBufferLine proc	syscall buffer:dword
	public _ReadBufferLine
	pushm	ecx,edi
	mov	edi,buffer
	call	ReadBufferLine
	mov	eax,ecx
	popm	ecx,edi
	ret
_ReadBufferLine endp


	efile
	end


