	.code


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Open a file and if succesful, add its handle to the list.
;
;On Entry:-
;
;DS:EDX	- File name pointer as normal.
;
;On Exit:-
;
;If carry clear,
;AX	- Handle for file.
;
;else file not opened.
;
OpenFile	proc	near
	mov	ax,3d02h		;Open with read & write access.
	int	21h
	jc	@@9
	;
	push	ebx
	push	ecx
	mov	ebx,offset OpenFilesList	;Point at the list.
	mov	ecx,60
@@0:	cmp	w[ebx],-1		;free entry?
	jz	@@1
	add	ebx,2		;next entry.
	loop	@@0
	jmp	@@8
@@1:	mov	[ebx],ax		;store this handle.
	pop	ecx
	pop	ebx
	clc			;restore sucess flag.
@@9:	ret
	;
@@8:	mov	bx,ax
	mov	ah,3eh		;close again cos not enough table space.
	int	21h
	pop	ecx
	pop	ebx
	stc
	ret
OpenFile	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Create a file and if succesful, add its handle to the list.
;
;On Entry:-
;
;DS:EDX	- File name pointer as normal.
;
;On Exit:-
;
;If carry clear,
;AX	- Handle for file.
;
;else file not created.
;
CreateFile	proc	near
	mov	ah,3ch		;Create function.
	xor	cx,cx		;normal attributes.
	int	21h
	jc	@@9
	;
	push	ebx
	push	ecx
	mov	ebx,offset OpenFilesList	;Point at the list.
	mov	ecx,60
@@0:	cmp	w[ebx],-1		;free entry?
	jz	@@1
	add	ebx,2		;next entry.
	loop	@@0
	jmp	@@8
@@1:	mov	[ebx],ax		;store this handle.
	pop	ecx
	pop	ebx
	clc			;restore sucess flag.
@@9:	ret
	;
@@8:	mov	bx,ax
	mov	ah,3eh		;close again cos not enough table space.
	int	21h
	pop	ecx
	pop	ebx
	stc
	ret
CreateFile	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Set the file pointer position for a file.
;
;On Entry:-
;
;AL	- Method
;BX	- Handle
;DX:CX	- 32-bit position.
;
;Methods are:-
;
;0	- Absolute offset from start.
;1	- signed offset from current position.
;2	- signed offset from end of file.
;
SetFilePointer	proc	near
	xchg	dx,cx		;DOS has them in a stupid order.
	push	bx
	mov	ah,42h		;set pointer function.
	int	21h
	pop	bx
	mov	cx,ax		;fetch small result.
	ret
SetFilePointer	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Read some data from a file.
;
;On Entry:-
;
;BX	- Handle
;ECX	- Length
;EDI	- Position.
;
ReadFile	proc	near
	xor	edx,edx
@@0:	pushm	bx,ecx,edx,edi
	cmp	ecx,65535		;size of chunks to load.
	jc	@@2
	mov	ecx,65535		;as close to 64k as can get.
@@2:	mov	edx,edi
	mov	ah,3fh
	int	21h		;read from the file.
	popm	bx,ecx,edx,edi
	jc	@@9
	movzx	eax,ax		;get length read.
	add	edx,eax		;update length read counter.
	sub	ecx,eax		;update length counter.
	add	edi,eax		;move memory pointer.
	or	ecx,ecx
	jz	@@8		;read as much as was wanted.
	or	eax,eax		;did we read anything?
	jz	@@8
	jmp	@@0
@@8:	mov	eax,edx		;get accumulated length read.
	clc
	ret
	;
@@9:	stc
	ret
ReadFile	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Write some data to a file.
;
;On Entry:-
;
;BX	- Handle
;ECX	- Length
;ESI	- Position.
;
WriteFile	proc	near
	xor	edx,edx
@@0:	pushm	bx,ecx,edx,esi
	cmp	ecx,65535		;size of chunks to load.
	jc	@@2
	mov	ecx,65535		;as close to 64k as can get.
@@2:	mov	edx,esi
	mov	ah,40h
	int	21h		;read from the file.
	popm	bx,ecx,edx,esi
	jc	@@9
	movzx	eax,ax		;get length read.
	add	edx,eax		;update length read counter.
	sub	ecx,eax		;update length counter.
	add	esi,eax		;move memory pointer.
	or	ecx,ecx
	jz	@@8		;read as much as was wanted.
	or	eax,eax		;did we write anything?
	jz	@@9
	jmp	@@0
@@8:	mov	eax,edx		;get accumulated length read.
	clc
	ret
	;
@@9:	stc
	ret
WriteFile	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Close a file and delete its handle from the list.
;
;On Entry:-
;
;BX	- Handle for file to close.
;
CloseFile	proc	near
	pushm	ax,ebx,ecx
	mov	ax,bx
	mov	ebx,offset OpenFilesList	;Point at the list.
	mov	ecx,60
@@0:	cmp	[ebx],ax		;right entry?
	jz	@@1
	add	ebx,2		;next entry.
	loop	@@0
	jmp	@@9
@@1:	mov	w[ebx],-1		;clear this handle.
	popm	ax,ebx,ecx
	mov	ah,3eh		;close file function.
	int	21h
	ret
@@9:	popm	ax,ebx,ecx
	stc
	ret
CloseFile	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Close all files in the list.
;
CloseAllFiles	proc	near
	push	ax
	push	ebx
	push	ecx
	mov	ebx,offset OpenFilesList	;Point at the list.
	mov	ecx,60		;size OpenFilesList /2
@@0:	cmp	w[ebx],0		;used entry?
	jnz	@@2
@@1:	add	ebx,2		;next entry.
	loop	@@0
	pop	ecx
	pop	ebx
	pop	ax
	ret
@@2:	push	ebx
	push	ecx
	mov	bx,[ebx]		;get this handle.
	mov	ah,3eh		;close file function.
	int	21h
	pop	ecx
	pop	ebx
	jmp	@@1
CloseAllFiles	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Delete a file.
;
;On Entry:-
;
;DS:EDX	- File name to delete.
;
DeleteFile	proc	near
	mov	ah,41h
	int	21h
	ret
DeleteFile	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Read a line of text from input file specified.
;
;On Entry:-
;
;BX	- File handle.
;DS:EDI	- Buffer address.
;
;On Exit:-
;
;AX	- Status.
;	0 - Line is in the buffer.
;	1 - EOF.
;
;CX	- Line length.
;
ReadLine	proc	near
	pushm	eax,ebx,edx
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
	inc	ecx
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
	xor	edx,edx
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
	xor	eax,eax
	ret
ReadLine	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
InitFileBuffer	proc	near
	mov	FileBufferCount,0
	ret
InitFileBuffer	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ReadBufferByte	proc	near
	mov	ax,0
	cmp	FileBufferCount,0
	jnz	@@0
	;
	;Need to re-fill the buffer.
	;
	pushm	eax,ebx,ecx,edx,esi,edi,ebp
	mov	edi,offset FileBuffer
	mov	ecx,1024
	call	ReadFile
	mov	FileBufferCount,eax
	mov	FileBufferPosition,offset FileBuffer
	popm	eax,ebx,ecx,edx,esi,edi,ebp
	jc	@@9
	;
@@0:	cmp	FileBufferCount,0	;still zero?
	jz	@@8
	inc	ah		;getting a byte then.
	pushm	esi,edi
	mov	esi,FileBufferPosition
	mov	al,[esi]
	inc	FileBufferPosition
	dec	FileBufferCount
	popm	esi,edi
	;
@@8:	clc
@@9:	ret
ReadBufferByte	endp


	.data


FileBufferCount dd 0
FileBufferPosition dd 0
FileBuffer	db 2048 dup (?)
OpenFilesList	dw 60 dup (-1)	;Enough for 60 files.

