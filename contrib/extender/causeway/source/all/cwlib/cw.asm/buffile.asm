	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Initialise file buffer.
;
;On Entry:
;
;EBX	- File handle.
;
;On Exit:
;
;ALL registers preserved.
;
InitFileBuffer	proc	near
	mov	FileBufferCount,0	;reset buffer count.
	mov	FileBufferHandle,ebx	;store handle.
	ret
InitFileBuffer	endp


;-------------------------------------------------------------------------
;
;Read a byte from the file buffer.
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
;eg, if AX=0 then EOF reached.
;
;Don't mix normal ReadFile call's with this call, they don't take account of
;each other.
;
ReadBufferByte	proc	near
	xor	ax,ax
	cmp	FileBufferCount,0	;anything in the buffer?
	jnz	r0
	;
	;Need to re-fill the buffer.
	;
	pushm	eax,ebx,ecx,edx
	mov	edx,offset FileBuffer
	mov	ebx,FileBufferHandle
	mov	ecx,1024
	call	ReadFile
	mov	FileBufferCount,eax
	mov	FileBufferPosition,offset FileBuffer
	popm	eax,ebx,ecx,edx
	;
r0:	cmp	FileBufferCount,0	;still zero?
	jz	r8
	inc	ah
	push	esi
	mov	esi,FileBufferPosition
	lodsb
	mov	FileBufferPosition,esi
	dec	FileBufferCount
	pop	esi
	;
r8:	ret
ReadBufferByte	endp


;-------------------------------------------------------------------------
;
;Read a line of text from input file specified.
;
;On Entry:
;
;EDI	- Buffer.
;
;On Exit:
;
;ECX	- Bytes read, 0=EOF or error.
;
ReadBufferLine	proc	near
	pushm	eax,ebx,edx
	mov	ebx,FileBufferHandle
	xor	ecx,ecx		;reset bytes so far count.
l0:	call	ReadBufferByte
	mov	[edi],al
	or	ax,ax		;read anything?
	jz	CheckEOF
	inc	ecx
	cmp	b[edi],13		;EOL?
	jz	CheckLF
	cmp	b[edi],26
	jz	SoftEOF		;move to the end of the file, then do an EOL.
	inc	edi
	cmp	ecx,1024		;line getting a bit long?
	jnc	SoftEOF
	jmp	l0		;keep going.
	;
CheckLF:	mov	b[edi],0		;terminate this line.
	inc	edi
	call	ReadBufferByte
	mov	b[edi],al
	or	ax,ax		;read anything?
	jz	l9		;no LF here is an error.
	inc	ecx
	cmp	b[edi],10		;/
	jnz	l9		;/
	;
EOL:	mov	b[edi],0		;terminate the line.
	xor	ax,ax
	jmp	l10
	;
SoftEOF:	pushm	ebx,ecx,edi
	xor	ecx,ecx
	mov	al,2
	call	SetFilePointer
	popm	ebx,ecx,edi		;fall through to treat like hard EOF.
	mov	FileBufferCount,0
	;
CheckEOF:	or	ecx,ecx		;read anything on this line yet?
	jnz	EOL		;do EOL this time.
	;
EOF:	mov	b[edi],0		;terminate the line.
l9:	mov	ax,1
	;
l10:	popm	eax,ebx,edx
	ret
ReadBufferLine	endp


	sdata

FileBufferCount dd 0
FileBufferHandle dd 0
FileBufferPosition dd 0
FileBuffer	db 1024 dup (?)


	efile
	end

