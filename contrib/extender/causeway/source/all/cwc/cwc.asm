	include cwlib.inc
	include cw.inc
	include strucs.inc
	includelib cw.lib


	ifndef ENGLISH
ENGLISH	equ	0
	endif
	ifndef SPANISH
SPANISH	equ	0
	endif

RepMinSize	equ	2


decode_c_struc struc
DecC_ID	db "CWC"
DecC_Bits	db 0
DecC_Len	dd ?
DecC_Size	dd ?
decode_c_struc	ends


;
;DeCoDe next input bit.
;
_DCD_ReadBit	macro
	local __0
	adc	ebp,ebp
	dec	dl
	jnz	__0
	mov	ebp,[esi]
	lea	esi,4[esi]
	mov	dl,dh
__0:	;
	endm


	scode

;
Copyright	db 13,10
	db 79 dup ("Ä"),13,10
	db "CauseWay Compressor 3.01 Copyright 1993-96 Michael Devore; all rights reserved.",13,10
	db 79 dup ("Ä"),13,10
	db 13,10,0

;------------------------------------------------------------------------------
;
;Main entry point from default startup code.
;
_Main	proc	near
	mov	ErrorStack,esp	;Store the stack so it can be
	;			;retrieved for error processing.
	mov	esi,offset Copyright
	pushm	ds,cs
	pop	ds
	call	PrintString		;Print copyright message.
	pop	ds
	;
	mov	ErrorNumber,1
	call	ReadCommand		;Read the command line.
	cmp	OptionCounter,1
	jc	System		;need a file name to work on
	cmp	OptionPointers,0	;so make sure we have one.
	jz	System
;
;Force EXE extension if nothing specified.
;
	mov	esi,OptionPointers
	mov	edi,offset FileName	;get file name mask.
	xor	al,al
	cld
@@e0:	movsb
	cmp	b[esi-1],'.'
	jnz	@@e1
	mov	al,1
@@e1:	cmp	b[esi-1],"\"
	jnz	@@e3
	xor	al,al
@@e3:	cmp	b[esi-1],0
	jnz	@@e0
	or	al,al
	jnz	@@e2
	mov	b[edi-1],'.'
	mov	esi,offset EXEextension
@@e4:	movsb
	cmp	b[esi-1],0
	jnz	@@e4
@@e2:	;
	mov	edx,offset FileName	;Build a file name list from
	xor	ecx,ecx
	call	MakeFileList		;the mask specified.
	mov	ErrorNumber,2
	or	eax,eax		;Get anything?
	jz	System
	mov	ErrorNumber,3
	cmp	d[eax],0		;Check we got some names.
	jz	System
	mov	FileList,eax		;Store the file name list.
	;
	pushad
	cmp	OptionTable+"M",0
	jz	@@nm0
	mov	esi,OptionPointers+("M"*4)
	mov	al,[esi]
	cmp	al,"1"
	jc	@@nm0
	cmp	al,"3"+1
	jnc	@@nm0
	sub	al,"0"
	movzx	eax,al
	mov	al,b[_CD_RepMaxTab+eax]
	mov	b[_CD_IDText+3],al
@@nm0:	popad
	;
	pushad
	cmp	OptionTable+"L",0
	jz	@@nl0
	mov	esi,OptionPointers+("L"*4)
	xor	edx,edx
@@nl2:	movzx	eax,b[esi]
	inc	esi
	or	al,al
	jz	@@nl1
	cmp	al,"0"
	jc	@@nl1
	cmp	al,"9"+1
	jnc	@@nl1
	sub	al,"0"
	add	edx,edx
	mov	ebx,edx
	add	edx,edx
	add	edx,edx
	add	edx,ebx
	add	edx,eax
	jmp	@@nl2
@@nl1:

; MED 06/03/97
;	cmp	edx,256
;	jc	@@nl3
;	mov	edx,255
	cmp	edx,4096
	jc	@@nl3
	mov	edx,4095

@@nl3:	mov	_CD_EncodeLit,edx
@@nl0:	popad
	;
	cmp	OptionTable+"X",0
	jz	@@nx0
	mov	OptionTable+"E",0
	;
@@nx0:	mov	esi,eax
	pushad
	add	esi,4
	mov	edi,offset TempFileName
	call	CopyString
	mov	esi,offset TempBase
	call	AppendString
	popad
	xor	ecx,ecx
@@NameLoop:	pushm	ecx,esi
	mov	edi,offset FileName
	call	FileListMakeName	;Build a full path\name
	;
	mov	ah,03h		;Get cursor position.
	xor	bh,bh		;/
	int	10h		;/
	mov	CursorPos,edx	;/
	;
	mov	esi,offset FileName	;Let them know which file we're
	call	PrintString		;working on.
	;
	cmp	OptionTable+"D",0
	jnz	@@Data
	mov	edx,offset FileName
	call	GetFileType		;find out what sort of file it is.
	mov	ErrorNumber,5
	jc	@@Error
	or	eax,eax
	jz	@@EXE
	dec	eax
	jz	@@3P
	dec	eax
	jz	@@EXE3P
	dec	eax
	jz	@@Data
	;
	mov	ErrorNumber,4
@@Error:	call	FileErrorPrint
	jmp	@@Next2
	;
@@EXE:	;Real mode EXE to deal with.
	;
	mov	ErrorNumber,10
	cmp	OptionTable+"E",0
	jnz	@@error
	mov	edx,offset FileName
	call	EXEProcess
	jmp	@@Next
	;
@@3P:	;Stand alone 3P file.
	;
	mov	ErrorNumber,10
	cmp	OptionTable+"E",0
	jnz	@@error
	mov	edx,offset FileName
	call	Process3P
	jc	@@Error
	jmp	@@Next
	;
@@EXE3P:	;Real mode EXE with 3P attatched.
	;
	mov	ErrorNumber,10
	cmp	OptionTable+"E",0
	jnz	@@error
	mov	edx,offset FileName
	call	EXEand3PProcess
	jc	@@Error
	jmp	@@Next
	;
@@Data:	;Plain data file.
	;
	mov	edx,offset FileName
	cmp	OptionTable+"E",0
	jz	@@DataE
	call	DataExpand
	jc	@@Error
	jmp	@@Next
@@DataE:	call	DataProcess
	jc	@@Error
	jmp	@@Next
	;
@@Next:	mov	esi,offset CRText
	call	PrintString
	;
@@Next2:	popm	ecx,esi
	inc	ecx
	cmp	ecx,[esi]
	jc	@@NameLoop
	;
	mov	ErrorNumber,0
	jmp	System
_Main	endp


;------------------------------------------------------------------------------
;
;Main exit routine. Program entry ESP is picked up again and we return to
;default code.
;
System	proc	near
	mov	esp,ErrorStack
	;
	mov	esi,ErrorNumber
	mov	esi,[ErrorList+esi*4]
	call	PrintString
	;
	mov	eax,ErrorNumber
	ret
System	endp


;------------------------------------------------------------------------------
;
;Process a real mode EXE file.
;
EXEProcess	proc	near
	local @@InHandle:dword, @@OutHandle:dword, @@EXEMem:dword,
	@@InName:dword, @@HeaderLen:dword, @@ImageLen:dword, @@CompLen:dword
	mov	@@InName,edx
	mov	@@InHandle,0
	mov	@@OutHandle,0
	mov	@@EXEMem,0
;
;Open the input file.
;
	mov	ErrorNumber,5
	call	OpenFile
	jc	@@9
	mov	@@InHandle,ebx
;
;Create the output file.
;
	mov	ErrorNumber,6
	mov	edx,offset TempFileName
	call	CreateFile
	jc	@@9
	mov	@@OutHandle,ebx
;
;Load the EXE into memory in a suitable order.
;
	mov	ErrorNumber,7
	mov	edx,offset ExeSignature
	mov	ebx,@@InHandle
	mov	ecx,1bh
	call	ReadFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Get header size in bytes.
	;
	mov	bx,w[ExeHeaderSize]	;Work out header size.
	xor	cx,cx		;/
	add	bx,bx		;/
	adc	cx,0		;/
	add	bx,bx		;/
	adc	cx,0		;/
	add	bx,bx		;/
	adc	cx,0		;/
	add	bx,bx		;/
	adc	cx,0		;/
	shl	ecx,16
	mov	cx,bx
	mov	@@HeaderLen,ecx
	;
	;Get exe image size in bytes.
	;
	mov	ax,w[ExeLength+2]	;get length in 512 byte blocks

; MED 01/17/96
	cmp	WORD PTR [ExeLength],0
	je	medexe2	; not rounded if no modulo

	dec	ax		;lose 1 cos its rounded up

medexe2:
	add	ax,ax		;mult by 2
	mov	dh,0
	mov	dl,ah
	mov	ah,al
	mov	al,dh		;mult by 256=*512
	add	ax,w[ExeLength]	;add length mod 512
	adc	dx,0		;add any carry to dx
	shl	edx,16
	mov	dx,ax
	sub	edx,@@HeaderLen
	mov	@@ImageLen,edx
	;
	;Get the memory for it all.
	;
	mov	ecx,@@HeaderLen
	add	ecx,@@ImageLen
	mov	ErrorNumber,2
	call	Malloc
	jc	@@9
	mov	@@EXEMem,esi
	;
	;Read the image portion of the file.
	;
	mov	ecx,@@HeaderLen
	mov	ebx,@@InHandle
	xor	al,al
	call	SetFilePointer
	mov	edx,@@EXEMem
	mov	ecx,@@ImageLen
	mov	ErrorNumber,7
	call	ReadFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Copy the header to the end of the image.
	;
	mov	esi,offset ExeSignature
	mov	edi,@@EXEMem
	add	edi,@@ImageLen
	mov	ecx,1bh
	rep	movsb
	;
	;Read the relocation table to the end of the header.
	;
	movzx	ecx,ExeRelocFirst
	xor	al,al
	call	SetFilePointer
	movzx	ecx,ExeRelocNum
	shl	ecx,2		;4 bytes per entry.
	mov	edx,@@EXEMem
	add	edx,@@ImageLen
	add	edx,1bh
	mov	ErrorNumber,7
	call	ReadFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
;
;Write a dummy EXE header and stub to the output file.
;
	mov	ErrorNumber,8
	mov	edx,offset ExeSignature
	mov	ebx,@@OutHandle
	mov	ecx,20h		;has to be a multiple of 16
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	mov	edx,offset EXECopyStub
	mov	ecx,EXECopyStubLen
	add	ecx,15
	and	ecx,not 15
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
;
;Work out how much we've to compress for the status print routine plus init
;cursor position.
;
	movzx	eax,ExeRelocNum
	shl	eax,2
	add	eax,@@ImageLen
	add	eax,1bh
	mov	StatusTotalLength,eax
	mov	StatusOutPrint,0
	;
	;Get cursor position.
	;
	mov	ah,03h		;Get cursor position.
	xor	bh,bh		;/
	int	10h		;/
	mov	dl,b[StatusPrintSize]
	mov	StatusCursorPos,edx	;/
	mov	StatusBlockCount,0
	call	ShowProgress
;
;Compress the data.
;
	mov	ErrorNumber,8
	mov	ebx,@@OutHandle
	mov	esi,@@EXEMem
	mov	ecx,StatusTotalLength
	pushm	_CD_CtrlSize,_CD_CtrlBits,d[_CD_IDText]
	mov	_CD_CtrlSize,2
	mov	_CD_CtrlBits,16
	cmp	b[_CD_IDText+3],9
	jnc	@@2
	mov	b[_CD_IDText+3],9
@@2:	call	EncodeFile
	popm	_CD_CtrlSize,_CD_CtrlBits,d[_CD_IDText]
	jc	@@9
	mov	@@CompLen,ecx
	mov	eax,ecx
	add	ecx,15
	and	ecx,not 15
	sub	ecx,eax
	mov	edx,offset DummyData
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
;
;Update status display.
;
	mov	eax,EXECopyStubLen
	add	eax,15
	and	eax,not 15
	mov	ebx,EXEDecStubLen
	add	ebx,15
	and	ebx,not 15
	mov	ecx,@@CompLen
	add	ecx,15
	and	ecx,not 15
	add	eax,ebx
	add	eax,ecx
	mov	StatusBlockCount,eax
	mov	StatusNoBar,1
	call	ShowProgress
;
;Write decompressor code to output file.
;
	mov	edx,offset EXEDecStub
	mov	ecx,EXEDecStubLen
	add	ecx,15
	and	ecx,not 15
	mov	ebx,@@OutHandle
	mov	ErrorNumber,7
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
;
;Now update the EXE header details.
;
	mov	ebx,@@OutHandle
	xor	ecx,ecx
	mov	al,1
	call	SetFilePointer		;Get file length.
	mov	eax,ecx
	mov	edx,eax
	and	eax,511
	mov	w[EXELength],ax
	shr	ecx,9
	or	ax,ax
	jz	@@0
	inc	ecx
@@0:	mov	w[EXELength+2],cx
	mov	w[ExeHeaderSize],2
	movzx	ecx,w[EXERelocNum]
	mov	w[ExeRelocNum],0
	mov	w[ExeEntryIP],0
	mov	w[ExeEntryCS],0
	mov	w[EXERelocFirst],20h
	mov	eax,EXECopyStubLen
	add	eax,15
	and	eax,not 15
;	sub	eax,100h
	sub	eax,CWCStackSize

	shr	eax,4
	mov	w[ExeStackSeg],ax
;	mov	w[ExeEntrySP],100h-10h
	mov	w[ExeEntrySP],CWCStackSize-10h

	;
	mov	eax,@@ImageLen	;origional image length.
	shl	ecx,2
	add	eax,ecx		;include relocation items.
	add	eax,1bh		;include header.
	mov	ecx,EXEDecStubLen
	add	ecx,15
	and	ecx,not 15
	add	eax,ecx		;allow for decompressor.
;	add	eax,100h		;allow for stack.
	add	eax,CWCStackSize	;allow for stack.

	sub	edx,20h
	sub	eax,edx
	jns	@@1
	mov	eax,edx
@@1:	shr	eax,4
	inc	eax
	add	w[EXEMinAlloc],ax
	;
	;Write new header.
	;
	mov	ebx,@@OutHandle
	xor	ecx,ecx
	xor	al,al
	call	SetFilePointer
	;
	mov	edx,offset ExeSignature
	mov	ecx,20h
	mov	ErrorNumber,7
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
;
;Now update the stub details.
;
	mov	edx,offset EXECopyStub
	mov	eax,EXECopyStubLen
	add	eax,15
	and	eax,not 15
	sub	eax,10h
	add	edx,eax
	;
	mov	eax,EXECopyStubLen
	add	eax,15
	and	eax,not 15
	mov	ebx,@@CompLen
	add	ebx,15
	and	ebx,not 15
	mov	ecx,EXEDecStubLen
	add	ecx,15
	and	ecx,not 15
	add	eax,ebx
	add	eax,ecx
	shr	eax,4
	mov	[edx],ax
	;
	mov	eax,@@CompLen
	add	eax,15
	and	eax,not 15
	mov	ebx,size EXEDecStubLen
	add	ebx,15
	and	ebx,not 15
	add	eax,ebx
	shr	eax,4
	mov	[edx+2],ax
	;
	mov	eax,@@CompLen
	add	eax,15
	shr	eax,4
	mov	[edx+6],ax
	;
	mov	eax,@@ImageLen
	mov	w[edx+8],ax
	shr	eax,16
	mov	w[edx+10],ax
	;
	mov	edx,offset EXECopyStub
	mov	ecx,EXECopyStubLen
	add	ecx,15
	and	ecx,not 15
	mov	ebx,@@OutHandle
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
;
;Close both files.
;
	xor	ebx,ebx
	xchg	ebx,@@InHandle
	call	CloseFile
	xor	ebx,ebx
	xchg	ebx,@@OutHandle
	call	CloseFile
;
;Rename the temp file to overwrite the origional.
;
	mov	edx,@@InName
	call	DeleteFile
	mov	edx,offset TempFileName
	mov	edi,@@InName		;get file name mask.
	call	RenameFile
	;
	mov	ErrorNumber,0
	clc
	;
@@9:	pushf
	mov	ebx,@@InHandle
	or	ebx,ebx
	jz	@@9_0
	call	CloseFile
@@9_0:	mov	ebx,@@OutHandle
	or	ebx,ebx
	jz	@@9_1
	call	CloseFile
	popf
	pushf
	jnc	@@9_1
	mov	edx,offset TempFileName
	call	DeleteFile
@@9_1:	mov	esi,@@EXEMem
	or	esi,esi
	jz	@@9_2
	call	Free
@@9_2:	popf
	ret
EXEProcess	endp


;------------------------------------------------------------------------------
;
;Process stand alone 3P file.
;
Process3P	proc near
	local @@InHandle:dword, @@OutHandle:dword, @@InName:dword
	mov	@@InName,edx
	mov	@@InHandle,0
	mov	@@OutHandle,0
;
;Open the input file.
;
	mov	ErrorNumber,5
	call	OpenFile
	jc	@@9
	mov	@@InHandle,ebx
;
;Create the output file.
;
	mov	ErrorNumber,6
	mov	edx,offset TempFileName
	call	CreateFile
	jc	@@9
	mov	@@OutHandle,ebx
;
;Now process the 3P section of the file.
;
	mov	esi,@@InHandle
	mov	edi,@@OutHandle
	cmp	OptionTable+"E",0
	jnz	@@0
	call	Encode3PFile
	jmp	@@1
@@0:	call	Decode3PFile
@@1:	jc	@@9
;
;Close both files.
;
	xor	ebx,ebx
	xchg	ebx,@@InHandle
	call	CloseFile
	xor	ebx,ebx
	xchg	ebx,@@OutHandle
	call	CloseFile
;
;Rename the temp file to overwrite the origional.
;
	mov	edx,@@InName
	call	DeleteFile
	mov	edx,offset TempFileName
	mov	edi,@@InName		;get file name mask.
	call	RenameFile
	;
	mov	ErrorNumber,0
	clc
	;
@@9:	pushf
	mov	ebx,@@InHandle
	or	ebx,ebx
	jz	@@9_0
	call	CloseFile
@@9_0:	mov	ebx,@@OutHandle
	or	ebx,ebx
	jz	@@9_1
	call	CloseFile
	popf
	pushf
	jnc	@@9_1
	mov	edx,offset TempFileName
	call	DeleteFile
@@9_1:	popf
	ret
Process3P	endp


;------------------------------------------------------------------------------
;
;Process a real mode EXE file with 3P file attatched.
;
EXEand3PProcess proc near
	local @@InHandle:dword, @@OutHandle:dword, @@StubMem:dword,
	@@InName:dword
	mov	@@InName,edx
	mov	@@InHandle,0
	mov	@@OutHandle,0
	mov	@@StubMem,0
;
;Open the input file.
;
	mov	ErrorNumber,5
	call	OpenFile
	jc	@@9
	mov	@@InHandle,ebx
;
;Create the output file.
;
	mov	ErrorNumber,6
	mov	edx,offset TempFileName
	call	CreateFile
	jc	@@9
	mov	@@OutHandle,ebx
;
;Copy stub loader into temp file.
;
	mov	ErrorNumber,7
	mov	edx,offset ExeSignature
	mov	ebx,@@InHandle
	mov	ecx,1bh
	call	ReadFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	ax,w[ExeLength+2]	;get length in 512 byte blocks

; MED 01/17/96
	cmp	WORD PTR [ExeLength],0
	je	medexe3	; not rounded if no modulo

	dec	ax		;lose 1 cos its rounded up

medexe3:
	add	ax,ax		;mult by 2
	mov	dh,0
	mov	dl,ah
	mov	ah,al
	mov	al,dh		;mult by 256=*512
	add	ax,w[ExeLength]	;add length mod 512
	adc	dx,0		;add any carry to dx
	mov	cx,ax
	xchg	cx,dx
	shl	ecx,16
	mov	cx,dx
	mov	ErrorNumber,2
	call	Malloc
	jc	@@9
	mov	@@StubMem,esi
	pushm	ecx,esi
	xor	ecx,ecx
	xor	al,al
	mov	ebx,@@InHandle
	call	SetFilePointer
	popm	ecx,esi
	;
	;Read the file.
	;
	mov	ErrorNumber,7
	mov	edx,esi
	mov	ebx,@@InHandle
	call	ReadFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Write the file.
	;
	mov	ErrorNumber,8
	mov	ebx,@@OutHandle
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Free the memory.
	;
	xor	esi,esi
	xchg	esi,@@StubMem
	call	Free
;
;Now process the 3P section of the file.
;
	mov	esi,@@InHandle
	mov	edi,@@OutHandle
	cmp	OptionTable+"E",0
	jnz	@@0
	call	Encode3PFile
	jmp	@@1
@@0:	call	Decode3PFile
@@1:	jc	@@9
;
;Close both files.
;
	xor	ebx,ebx
	xchg	ebx,@@InHandle
	call	CloseFile
	xor	ebx,ebx
	xchg	ebx,@@OutHandle
	call	CloseFile
;
;Rename the temp file to overwrite the origional.
;
	mov	edx,@@InName
	call	DeleteFile
	mov	edx,offset TempFileName
	mov	edi,@@InName		;get file name mask.
	call	RenameFile
	;
	mov	ErrorNumber,0
	clc
	;
@@9:	pushf
	mov	ebx,@@InHandle
	or	ebx,ebx
	jz	@@9_0
	call	CloseFile
@@9_0:	mov	ebx,@@OutHandle
	or	ebx,ebx
	jz	@@9_1
	call	CloseFile
	popf
	pushf
	jnc	@@9_1
	mov	edx,offset TempFileName
	call	DeleteFile
@@9_1:	mov	esi,@@StubMem
	or	esi,esi
	jz	@@9_2
	call	Free
@@9_2:	popf
	ret
EXEand3PProcess endp


;------------------------------------------------------------------------------
;
;Process a plain data file.
;
DataProcess	proc	near
	local @@InHandle:dword, @@OutHandle:dword, @@InName:dword,
	@@InPos:dword, @@CopyMem:dword
	mov	@@InName,edx
	mov	@@InHandle,0
	mov	@@OutHandle,0
	mov	@@CopyMem,0
;
;Open the input file.
;
	mov	ErrorNumber,5
	call	OpenFile
	jc	@@9
	mov	@@InHandle,ebx
;
;Create the output file.
;
	mov	ErrorNumber,6
	mov	edx,offset TempFileName
	call	CreateFile
	jc	@@9
	mov	@@OutHandle,ebx
;
;Work out how much we've to compress for the status print routine plus init
;cursor position.
;
	mov	al,1
	xor	ecx,ecx
	mov	ebx,@@InHandle
	call	SetFilePointer
	mov	@@InPos,ecx
	xor	ecx,ecx
	mov	al,2
	call	SetFilePointer	;move to end of the file.
	xchg	ecx,@@InPos
	xor	al,al
	call	SetFilePointer	;move back to old position.
	xchg	ecx,@@InPos
	mov	StatusTotalLength,ecx
	mov	StatusOutPrint,0
	;
	;Get cursor position.
	;
	mov	ah,03h		;Get cursor position.
	xor	bh,bh		;/
	int	10h		;/
	mov	dl,b[StatusPrintSize]
	mov	StatusCursorPos,edx	;/
	mov	StatusBlockCount,0
	call	ShowProgress
;
;Compress the data.
;
	mov	ErrorNumber,2
	mov	ecx,StatusTotalLength
	call	Malloc
	jc	@@9
	mov	@@CopyMem,esi
	;
	;Read the data.
	;
	mov	ErrorNumber,7
	mov	edx,esi
	mov	ebx,@@InHandle
	call	ReadFile
	jc	@@9
	cmp	eax,ecx
	stc
	jnz	@@9
	;
	;Check it isn't already compressed.
	;
	mov	ErrorNumber,9
	mov	eax,[esi]
	and	eax,0FFFFFFh
	cmp	eax,"CWC"
	stc
	jz	@@9
	;
	;Compress it.
	;
	mov	ErrorNumber,8
	mov	ebx,@@OutHandle
	call	EncodeFile
	jc	@@9
	push	ecx
	xor	esi,esi
	xchg	esi,@@CopyMem
	call	Free
	pop	ecx
;
;Update status display.
;
	mov	StatusBlockCount,ecx
	mov	StatusNoBar,1
	call	ShowProgress
;
;Close both files.
;
	xor	ebx,ebx
	xchg	ebx,@@InHandle
	call	CloseFile
	xor	ebx,ebx
	xchg	ebx,@@OutHandle
	call	CloseFile
;
;Rename the temp file to overwrite the origional.
;
	mov	edx,@@InName
	call	DeleteFile
	mov	edx,offset TempFileName
	mov	edi,@@InName		;get file name mask.
	call	RenameFile
	;
	mov	ErrorNumber,0
	clc
	;
@@9:	pushf
	mov	ebx,@@InHandle
	or	ebx,ebx
	jz	@@9_0
	call	CloseFile
@@9_0:	mov	ebx,@@OutHandle
	or	ebx,ebx
	jz	@@9_1
	call	CloseFile
	popf
	pushf
	jnc	@@9_1
	mov	edx,offset TempFileName
	call	DeleteFile
@@9_1:	mov	esi,@@CopyMem
	or	esi,esi
	jz	@@9_2
	call	Free
@@9_2:	popf
	ret
DataProcess	endp


;------------------------------------------------------------------------------
;
;Process a plain data file.
;
DataExpand	proc	near
	local @@InHandle:dword, @@OutHandle:dword, @@InName:dword,
	@@InPos:dword, @@CopyMem:dword
	mov	@@InName,edx
	mov	@@InHandle,0
	mov	@@OutHandle,0
	mov	@@CopyMem,0
;
;Open the input file.
;
	mov	ErrorNumber,5
	call	OpenFile
	jc	@@9
	mov	@@InHandle,ebx
;
;Create the output file.
;
	mov	ErrorNumber,6
	mov	edx,offset TempFileName
	call	CreateFile
	jc	@@9
	mov	@@OutHandle,ebx
;
;Work out how much we've to expand for the status print routine plus init
;cursor position.
;
	mov	al,1
	xor	ecx,ecx
	mov	ebx,@@InHandle
	call	SetFilePointer
	mov	@@InPos,ecx
	xor	ecx,ecx
	mov	al,2
	call	SetFilePointer	;move to end of the file.
	xchg	ecx,@@InPos
	xor	al,al
	call	SetFilePointer	;move back to old position.
	xchg	ecx,@@InPos
	mov	StatusTotalLength,ecx
	mov	StatusOutPrint,0
	;
	;Get cursor position.
	;
	mov	ah,03h		;Get cursor position.
	xor	bh,bh		;/
	int	10h		;/
	mov	dl,b[StatusPrintSize]
	mov	StatusCursorPos,edx	;/
	mov	StatusBlockCount,0
;
;Compress the data.
;
	mov	ErrorNumber,2
	mov	ecx,StatusTotalLength
	call	Malloc
	jc	@@9
	mov	@@CopyMem,esi
	;
	;Read the data.
	;
	mov	ErrorNumber,7
	mov	edx,esi
	mov	ebx,@@InHandle
	call	ReadFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Get data size.
	;
	pushad
	mov	eax,DecC_Len[esi]
	mov	StatusTotalLength,eax
	mov	StatusNoBar,2
	call	ShowProgress
	popad
	;
	;Compress it.
	;
	mov	ErrorNumber,4
	mov	ebx,@@OutHandle
	call	DecodeFile
	jc	@@9
	push	ecx
	xor	esi,esi
	xchg	esi,@@CopyMem
	call	Free
	pop	ecx
;
;Update status display.
;
	mov	StatusBlockCount,ecx
	mov	StatusNoBar,1
	call	ShowProgress
;
;Close both files.
;
	xor	ebx,ebx
	xchg	ebx,@@InHandle
	call	CloseFile
	xor	ebx,ebx
	xchg	ebx,@@OutHandle
	call	CloseFile
;
;Rename the temp file to overwrite the origional.
;
	mov	edx,@@InName
	call	DeleteFile
	mov	edx,offset TempFileName
	mov	edi,@@InName		;get file name mask.
	call	RenameFile
	;
	mov	ErrorNumber,0
	clc
	;
@@9:	pushf
	mov	ebx,@@InHandle
	or	ebx,ebx
	jz	@@9_0
	call	CloseFile
@@9_0:	mov	ebx,@@OutHandle
	or	ebx,ebx
	jz	@@9_1
	call	CloseFile
	popf
	pushf
	jnc	@@9_1
	mov	edx,offset TempFileName
	call	DeleteFile
@@9_1:	mov	esi,@@CopyMem
	or	esi,esi
	jz	@@9_2
	call	Free
@@9_2:	popf
	ret
DataExpand	endp


;------------------------------------------------------------------------------
;
;Encode 3P section of a file. Assumes source file pointer is already pointing
;at source data and destination file pointer is already pointing at destination
;position.
;
;On Entry:
;
;ESI	- source handle.
;EDI	- destination handle.
;
;On Exit:
;
;Carry set on error.
;
Encode3PFile	proc	near
	local @@InHandle:dword, @@OutHandle:dword, @@CopyMem:dword,
	@@OutPos:dword, @@OutLength:dword
	mov	@@InHandle,esi
	mov	@@OutHandle,edi
	mov	@@CopyMem,0
	mov	@@OutLength,0
;
;Get current output position.
;
	mov	ebx,edi
	xor	ecx,ecx
	mov	al,1
	call	SetFilePointer
	mov	@@OutPos,ecx
;
;Get the 3P section of the header.
;
	mov	ErrorNumber,7
	mov	edx,offset NewHeader
	mov	ecx,size NewHeaderStruc
	mov	ebx,@@InHandle
	call	ReadFile
	jc	@@8
	cmp	eax,ecx		;did we read enough?
	jnz	@@8
	;
	;Make sure it's the right format.
	;
	mov	ErrorNumber,4
	cmp	w[NewHeader+NewID],"P3"
	jnz	@@8
;
;Check if it's already compressed.
;
	mov	ErrorNumber,9
	test	b[NewHeader+NewFlags+3],128
	jnz	@@8
;
;Write the header.
;
	mov	ErrorNumber,8
	mov	edx,offset NewHeader
	mov	ecx,size NewHeaderStruc
	mov	ebx,@@OutHandle
	call	WriteFile
	jc	@@8
	cmp	eax,ecx		;did we write enough?
	jnz	@@8
;
;Work out how much we've to compress for the status print routine plus init
;cursor position.
;
	movzx	eax,w[NewHeader+NewSegments]
	shl	eax,3
	mov	ebx,d[NewHeader+NewRelocs]
	shl	ebx,2
	add	eax,ebx
	add	eax,d[NewHeader+NewExports]
	add	eax,d[NewHeader+NewImports]
	add	eax,d[NewHeader+NewLength]
	mov	StatusTotalLength,eax
	mov	StatusOutPrint,0
	;
	;Get cursor position.
	;
	mov	ah,03h		;Get cursor position.
	xor	bh,bh		;/
	int	10h		;/
	mov	dl,b[StatusPrintSize]
	mov	StatusCursorPos,edx	;/
	mov	StatusBlockCount,0
	call	ShowProgress
;
;Compress the segment definitions.
;
	mov	ErrorNumber,2
	movzx	ecx,w[NewHeader+NewSegments]
	shl	ecx,3		;8 bytes per entry.
	call	Malloc
	jc	@@8
	mov	@@CopyMem,esi
	;
	;Read the segment definitions.
	;
	mov	ErrorNumber,7
	mov	edx,esi
	mov	ebx,@@InHandle
	call	ReadFile
	jc	@@8
	cmp	eax,ecx
	jnz	@@8
	;
	;Compress them.
	;
	push	_CD_EncodeLit
	mov	_CD_EncodeLit,0
	mov	ErrorNumber,8
	mov	ebx,@@OutHandle
	call	EncodeFile
	pop	_CD_EncodeLit
	jc	@@8
	add	@@OutLength,ecx
	xor	esi,esi
	xchg	esi,@@CopyMem
	call	Free
;
;Compress the relocation data.
;
	mov	ecx,d[NewHeader+NewRelocs]
	or	ecx,ecx
	jz	@@NoRelocs
	shl	ecx,2		;4 bytes per entry.
	mov	ErrorNumber,2
	call	Malloc
	jc	@@8
	mov	@@CopyMem,esi
	;
	;Read them into the buffer.
	;
	mov	edx,esi
	mov	ErrorNumber,7
	mov	ebx,@@InHandle
	call	ReadFile
	jc	@@8
	cmp	eax,ecx
	jnz	@@8
	;
	;Compress them.
	;
	push	_CD_EncodeLit
	mov	_CD_EncodeLit,0
	mov	ErrorNumber,8
	mov	ebx,@@OutHandle
	call	EncodeFile
	pop	_CD_EncodeLit
	jc	@@8
	add	@@OutLength,ecx
	xor	esi,esi
	xchg	esi,@@CopyMem
	call	Free
;
;Compress the EXPORT data.
;
@@NoRelocs:	mov	ecx,d[NewHeader+NewExports]
	or	ecx,ecx
	jz	@@NoExports
	mov	ErrorNumber,2
	call	Malloc
	jc	@@8
	mov	@@CopyMem,esi
	;
	;Read them into the buffer.
	;
	mov	edx,esi
	mov	ErrorNumber,7
	mov	ebx,@@InHandle
	call	ReadFile
	jc	@@8
	cmp	eax,ecx
	jnz	@@8
	;
	;Compress them.
	;
	push	_CD_EncodeLit
	mov	_CD_EncodeLit,0
	mov	ErrorNumber,8
	mov	ebx,@@OutHandle
	call	EncodeFile
	pop	_CD_EncodeLit
	jc	@@8
	add	@@OutLength,ecx
	xor	esi,esi
	xchg	esi,@@CopyMem
	call	Free
;
;Compress the IMPORT data.
;
@@NoExports:	mov	ecx,d[NewHeader+NewImports]
	or	ecx,ecx
	jz	@@NoImports
	mov	ErrorNumber,2
	call	Malloc
	jc	@@8
	mov	@@CopyMem,esi
	;
	;Read them into the buffer.
	;
	mov	edx,esi
	mov	ErrorNumber,7
	mov	ebx,@@InHandle
	call	ReadFile
	jc	@@8
	cmp	eax,ecx
	jnz	@@8
	;
	;Compress them.
	;
	push	_CD_EncodeLit
	mov	_CD_EncodeLit,0
	mov	ErrorNumber,8
	mov	ebx,@@OutHandle
	call	EncodeFile
	pop	_CD_EncodeLit
	jc	@@8
	add	@@OutLength,ecx
	xor	esi,esi
	xchg	esi,@@CopyMem
	call	Free
;
;Compress the image data.
;
@@NoImports:	mov	ErrorNumber,2
	mov	ecx,d[NewHeader+NewLength]
	call	Malloc
	jc	@@8
	;
	;Read the data.
	;
	mov	edx,esi
	mov	ebx,@@InHandle
	call	ReadFile
	jc	@@8
	cmp	eax,ecx
	jnz	@@8
	;
	;Compress the data.
	;
	mov	ErrorNumber,8
	mov	ebx,@@OutHandle
	call	EncodeFile
	jc	@@8
	add	@@OutLength,ecx
	xor	esi,esi
	xchg	esi,@@CopyMem
	call	Free
;
;Update the header.
;
	mov	eax,@@OutLength
	add	eax,size NewHeaderStruc
	mov	d[NewHeader+NewSize],eax
	or	b[NewHeader+NewFlags+3],128	;mark it as compressed.
	;
	;Get current position.
	;
	mov	ebx,@@OutHandle
	xor	ecx,ecx
	mov	al,1
	call	SetFilePointer
	;
	;Go back to header position.
	;
	xchg	ecx,@@OutPos
	xor	al,al
	call	SetFilePointer
	;
	;Write new header.
	;
	mov	ErrorNumber,8
	mov	edx,offset NewHeader
	mov	ecx,size NewHeaderStruc
	call	WriteFile
	jc	@@8
	cmp	eax,ecx
	jnz	@@8
	;
	;Move back to the end again.
	;
	mov	ecx,@@OutPos
	xor	al,al
	call	SetFilePointer
;
;Update status display.
;
	mov	eax,@@OutLength
	add	eax,size NewHeaderStruc
	mov	StatusBlockCount,eax
	mov	StatusNoBar,1
	call	ShowProgress
;
;All done so exit.
;
	mov	ErrorNumber,0
	clc
	jmp	@@9
@@8:	stc
@@9:	pushf
	mov	esi,@@CopyMem
	or	esi,esi
	jz	@@9_0
	call	Free
@@9_0:	popf
	ret
Encode3PFile	endp


;------------------------------------------------------------------------------
;
;Decode 3P section of a file. Assumes source file pointer is already pointing
;at source data and destination file pointer is already pointing at destination
;position.
;
;On Entry:
;
;ESI	- source handle.
;EDI	- destination handle.
;
;On Exit:
;
;Carry set on error.
;
Decode3PFile	proc	near
	ret
Decode3PFile	endp


;------------------------------------------------------------------------------
;
;Decode a section of file.
;
;On Entry:
;
;EBX	- Output file handle.
;ESI	- Pointer to source data.
;
;On Exit:
;
;Carry set on error.
;
DecodeFile	proc	near
	pushm	ebx,edx,esi,edi,ebp
	mov	_DCD_Stack,esp	;Set for error exit.
	mov	_DCD_Handle,ebx	;Store the handle.
	;
	;Check for main ID string.
	;
	mov	eax,d[DecC_ID+esi]
	and	eax,0FFFFFFh
	cmp	eax,"CWC"
	jnz	@@form_error
	movzx	ecx,b[DecC_Bits+esi]
	push	ds
	mov	ds,CodeSegAlias
	assume ds:_TEXT
	mov	b[@@Shifter+2],cl
	mov	eax,1
	shl	eax,cl
	dec	eax
	mov	d[@@Masker+2],eax
	assume ds:DGROUP
	pop	ds
	mov	ebx,DecC_Len[esi]
	mov	_CD_LastLen,ebx
;
;Get on with decodeing the data.
;
	add	esi,size decode_c_struc
	mov	ebp,[esi]
	add	esi,4
	mov	dl,32
	mov	dh,dl
	mov	edi,offset _CD_OutBuffer
;
;The main decompresion loop.
;
	align 4
@@0:	_DCD_ReadBit
	jnc	@@1
	;
	;Read a raw byte.
	;
	mov	al,[esi]
	mov	[edi],al
	inc	esi
	inc	edi
	dec	ebx
	js	@@4
	jmp	@@0
	;
	align 4
@@1:	_DCD_ReadBit
	jnc	@@2
	;
	;Do a rep with 8 bit position, 2 bit length.
	;
	xor	ecx,ecx
	_DCD_ReadBit
	adc	cl,cl
	_DCD_ReadBit
	adc	cl,cl
	add	cl,2
	xor	eax,eax
	mov	al,[esi]
	inc	esi
	dec	eax
	js	@@1_0
	add	eax,ecx
	pushm	esi
	mov	esi,edi
	sub	esi,eax
	sub	ebx,ecx
	js	@@4
	rep	movsb
	popm	esi
	jmp	@@0
	;
	;Do a run with 2 bit length.
	;
	align 4
@@1_0:	mov	al,[esi]
	inc	esi
	inc	cl
	sub	ebx,ecx
	js	@@4
	rep	stosb
	jmp	@@0
	;
	align 4
@@2:	_DCD_ReadBit
	jnc	@@3
	;
	;Do a rep with 12 bit position, 4 bit length.
	;
	xor	eax,eax
	mov	ax,[esi]
	add	esi,2
	mov	cl,al
	and	ecx,15
	add	cl,2
	shr	eax,4
	dec	eax
	js	@@2_0
	add	eax,ecx
	pushm	esi
	mov	esi,edi
	sub	esi,eax
	sub	ebx,ecx
	js	@@4
	rep	movsb
	popm	esi
	jmp	@@0
	;
	;Do a run with 4 bit length.
	;
	align 4
@@2_0:	mov	al,[esi]
	inc	esi
	inc	cl
	sub	ebx,ecx
	js	@@4
	rep	stosb
	jmp	@@0
	;
	;Do a rep with 12-bit position, 12-bit length.
	;
	align 4
@@3:	mov	eax,[esi]
	add	esi,3
	and	eax,0FFFFFFh
	mov	ecx,eax
@@Masker:	and	ecx,4095
	add	ecx,2
@@Shifter:	shr	eax,12
	dec	eax
	js	@@3_0
	add	eax,ecx
	pushm	esi
	mov	esi,edi
	sub	esi,eax
	sub	ebx,ecx
	js	@@4
	rep	movsb
	popm	esi
	jmp	@@0
	;
	;Check for special codes of 0-15 (Would use 2 or 4 bit if really needed)
	;
	align 4
@@3_0:	cmp	ecx,RepMinSize+15+1
	jnc	@@3_1
	cmp	cl,RepMinSize+2	;Rationalise destination?
	jz	@@3_2		;flush output buffer.
	cmp	cl,RepMinSize+1	;Re-fill buffer?
	jz	@@0		;ignore input flush's.
	cmp	cl,RepMinSize+3	;Literal string?
	jz	@@3_3
	cmp	cl,RepMinSize	;Terminator?
	jz	@@4
	jmp	@@form_error
	;
	;Do a run with 12-bit length.
	;
	align 4
@@3_1:	mov	al,[esi]
	inc	esi
	inc	ecx
	sub	ebx,ecx
	js	@@4
	rep	stosb
	jmp	@@0
	;
	;Buffer needs to be flushed.
	;
	align 4
@@3_2:	mov	eax,_CD_LastLen
	sub	eax,ebx
	mov	StatusOutPrint,eax
	mov	StatusNoBar,2
	call	UpdateProgress
	;
	cmp	edi,offset _CD_OutBuffer+65536
	jc	@@0
	pushm	eax,ebx,ecx,edx,esi
	mov	ecx,edi
	sub	ecx,offset _CD_OutBuffer
	sub	ecx,65536
	mov	edx,offset _CD_OutBuffer
	mov	ebx,_DCD_Handle
	call	WriteFile
	cmp	eax,ecx
	jnz	@@read_error
	pushm	ecx,edi
	mov	eax,edi
	mov	esi,offset _CD_OutBuffer
	mov	edi,esi
	add	edi,ecx
	xchg	esi,edi
	mov	ecx,eax
	sub	ecx,esi
	cld
	rep	movsb
	popm	ecx,edi
	sub	edi,ecx
	popm	eax,ebx,ecx,edx,esi
	jmp	@@0
	;
	;Get literal string of bytes.
	;
	align 4
@@3_3:	xor	ecx,ecx
	mov	cl,[esi]		;get the length.
	inc	esi
	sub	ebx,ecx
	js	@@4
	rep	movsb		;copy them.
	jmp	@@0
	;
	;We're all done so exit.
	;
	align 4
@@4:	or	ebx,ebx
	jnz	@@form_error
	;
	;Write last piece of the file.
	;
	mov	ecx,edi
	sub	ecx,offset _CD_OutBuffer
	mov	edx,offset _CD_OutBuffer
	mov	ebx,_DCD_Handle
	call	WriteFile
	jc	@@read_error
	cmp	eax,ecx
	jnz	@@read_error
	;
	mov	ecx,_CD_LastLen
	xor	eax,eax
	clc
	jmp	@@exit
	;
@@read_error:	mov	eax,1
	stc
	jmp	@@exit
	;
@@form_error:	mov	eax,2
	stc
	;
@@exit:	mov	esp,_DCD_Stack
	popm	ebx,edx,esi,edi,ebp
	ret
DecodeFile	endp


;------------------------------------------------------------------------------
;
;Encode a section of file.
;
;On Entry:
;
;ECX	- Length to do.
;ESI	- Source data.
;EBX	- Destination file handle.
;
;On Exit:
;
;Carry set on error else,
;
;ECX	- encoded length.
;
EncodeFile	proc	near
	pushad
	mov	_CD_InESP,esp
	xor	eax,eax
	mov	_CD_Flags,eax
	mov	_CD_OutHandle,ebx	;Need the handle to write data to.
	mov	_CD_FoundLen,ecx
	xor	ecx,ecx
	mov	al,1
	call	SetFilePointer
	mov	_CD_OutStart,ecx	;store this position in the file.
	xor	eax,eax
	mov	_CD_OutTotal,eax
	mov	_CD_SigInCount,eax
	mov	_CD_OutCtrl,eax
	mov	_CD_OutCount,eax
	mov	eax,_CD_CtrlSize
	mov	_CD_Outpos,eax
	mov	_CD_SigOutCount,eax
	;
	pushad
	movzx	ecx,b[_CD_IDText+3]
	mov	_CD_RepMaxShift,ecx
	mov	eax,1
	shl	eax,cl
	add	eax,RepMinSize-1
	mov	_CD_RepMaxLen,eax
	sub	ecx,24
	neg	ecx
	mov	eax,1
	shl	eax,cl
	add	eax,RepMinSize-1
	mov	_CD_RepMaxSize,eax
	popad


	;
	;Get memory for the control points
	;
	pushad
	mov	CodeLast,0
	mov	CodeCount,0
	mov	CodePos,0
	mov	ecx,65536*(4+4)
	call	malloc
	jc	@@code0
	mov	CodeHeads,esi
	mov	edi,esi
	xor	al,al
	rep	stosb
	mov	ecx,_CD_RepMaxSize
	add	ecx,8
	shl	ecx,3
	call	malloc
	jc	@@code0
	mov	CodeFree,esi
	mov	CodeFree+4,esi
	mov	ecx,_CD_RepMaxSize
	add	ecx,7
@@code1:	mov	eax,esi
	add	eax,4+4
	mov	[esi],eax
	mov	esi,eax
	dec	ecx
	jnz	@@code1
	mov	d[esi],0
	clc
@@code0:	popad
	jc	_CD_Error



	push	esi
	mov	edx,offset _CD_IDText	;Send a copy of the ID string
	mov	ecx,4		;to the output.
	call	WriteFile
	jc	_CD_Error
	cmp	eax,ecx
	jnz	_CD_Error
	mov	edx,offset _CD_FoundLen	;Send a copy of the length to
	call	WriteFile		;the output.
	jc	_CD_Error
	cmp	eax,ecx
	jnz	_CD_Error
	mov	edx,offset _CD_OutTotal	;Send dummy output length.
	call	WriteFile
	jc	_CD_Error
	cmp	eax,ecx
	jnz	_CD_Error
	pop	esi
	;
	xor	ebp,ebp		;length of trailing data.
	xor	eax,eax
	mov	ebx,_CD_FoundLen	;length of input.
	mov	_CD_FoundLen,eax
	mov	_CD_LastLen,eax
	;
@@0:	;Search for a run of bytes.
	;
	mov	_CD_FoundLen,0
	mov	_CD_RepLength,0
	;
	cmp	_CD_EncodeLit,0
	jnz	@@4
	;

	mov	edi,esi
	mov	ecx,ebx
	cmp	ecx,_CD_RepMaxLen
	jc	@@0_0
	mov	ecx,_CD_RepMaxLen
@@0_0:	mov	al,[edi]
	repe	scasb
	sub	edi,esi
	dec	edi
	mov	_CD_RepLength,edi

	;
	;Search buffer for biggest matching string.
	;

	cmp	ebx,RepMinSize
	jc	@@4		;not big enough for a string.
	xor	eax,eax
	mov	ax,[esi]		;get code to search for.
	shl	eax,3
	add	eax,CodeHeads
	mov	eax,[eax]		;Get start of chain.
	mov	edx,RepMinSize
@@srch0:	or	eax,eax
	jz	@@4		;found nearest RepMinSize match
	mov	edi,[eax+4]		;get match address.
	mov	ecx,esi
	sub	ecx,edi		;get distance to input
	cmp	ecx,edx
	jc	@@srch3
	cmp	edx,RepMinSize
	jz	@@srch1
	push	esi
	push	ecx
	mov	ecx,edx
	add	esi,2
	add	edi,2
	sub	ecx,2
	repe	cmpsb		;check current length.
	pop	ecx
	jz	@@srch2
	pop	esi
@@srch3:	mov	eax,[eax]
	jmp	@@srch0
@@srch1:	push	esi
	add	esi,2
	add	edi,2
	sub	ecx,2
@@srch2:	inc	edx
	cmp	ecx,edx		;max length?
	jc	@@srch4
	cmp	ebx,edx		;reached longest string possible?
	jc	@@srch4
	cmp	edx,_CD_RepMaxLen
	jnc	@@srch4
	cmpsb
	jz	@@srch2
@@srch4:	dec	edx
	pop	esi
	mov	edi,[eax+4]		;get match address.
	mov	_CD_FoundPos,edi
	mov	_CD_FoundLen,edx
	mov	eax,[eax]
	jmp	@@srch0


	if	0
	cld
	mov	edi,esi
	sub	edi,ebp
	mov	ecx,ebp
	mov	edx,RepMinSize	;smallest match we will look for.
	sub	ecx,edx
	inc	ecx
	or	ecx,ecx
	js	@@4
	cmp	edx,ebx
	jg	@@4		;not enough data left for this.
	;
	align 4
@@1:	mov	al,[esi]
	repne	scasb		;search for this byte.
	jnz	@@4		;didn't find even this byte.
	;
	pushm	ecx,esi,edi
	mov	ecx,edx		;get length we're looking for.
	dec	edi
	repe	cmpsb		;check entire string.
	popm	ecx,esi,edi
	jz	@@2		;we found a match so record it.
	jmp	@@1		;keep looking.
	;
	align 4
@@2:	dec	edi
	;
	mov	_CD_FoundPos,edi	;store position and length of this match.
	mov	_CD_FoundLen,edx
	;
@@3:	dec	ecx
	js	@@4
	;
	cmp	edx,ebx
	jge	@@4		;not enough data left for this.
	cmp	edx,_CD_RepMaxLen
	jge	@@4
	;
	mov	al,[esi+edx]
	cmp	al,[edi+edx]
	jz	@@3_0
	inc	ecx
	inc	edi
	jmp	@@1
	;
	align 4
@@3_0:	inc	_CD_FoundLen
	inc	edx		;increase match length we want.
	jmp	@@3
	endif


	;
@@4:	cmp	_CD_RepLength,RepMinSize+1
	jc	@@4_0_0
	mov	eax,_CD_RepLength
	cmp	_CD_FoundLen,eax
	jnc	@@4_0_0
	;
	;Code a run instead of raw or rep
	;
	cmp	eax,RepMinSize+1+3+1
	jc	@@r4_1
	cmp	eax,RepMinSize+1+15+1
	jc	@@r4_2
	jmp	@@r4_3
	;
@@4_0_0:	cmp	_CD_FoundLen,RepMinSize
	jc	@@4_0
	;
	cmp	_CD_FoundLen,RepMinSize+3+1
	jnc	@@4_0_1
	mov	eax,esi
	sub	eax,_CD_FoundPos	;get distance.
	sub	eax,_CD_FoundLen
	inc	eax
	cmp	eax,255+1
	jc	@@4_1
	;
@@4_0_1:	cmp	_CD_FoundLen,RepMinSize+1
	jc	@@4_0
	;
	cmp	_CD_FoundLen,RepMinSize+15+1
	jnc	@@4_0_2
	mov	eax,esi
	sub	eax,_CD_FoundPos	;get distance.
	sub	eax,_CD_FoundLen
	inc	eax
	cmp	eax,4095+1
	jc	@@4_2
	;
@@4_0_2:	cmp	_CD_FoundLen,RepMinSize+2
	jc	@@4_0
	;
	jmp	@@4_3
	;
@@4_0:	;Send a raw byte(s).
	;
	cmp	_CD_EncodeLit,0
	jz	@@4_1_0
	;
	;Send literal string code.
	;

med4:
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	mov	eax,3
	call	_CD_WriteByte
	xchg	ah,al
	call	_CD_WriteByte
	shr	eax,16
	call	_CD_WriteByte
	;
	;Send the length.
	;
	mov	eax,_CD_EncodeLit

; MED 06/03/97
	cmp	eax,255
	jbe	med2
	mov	eax,255
med2:

	call	_CD_WriteByte
	;
	;Send the actual string.
	;
	mov	ecx,_CD_EncodeLit

; MED 06/03/97	
;	mov	_CD_EncodeLit,0
	cmp	ecx,255
	jbe	med3
	mov	ecx,255
med3:
	sub	_CD_EncodeLit,ecx
	
	cmp	ecx,ebx
	jc	@@4_1_1
	mov	ecx,ebx
	
@@4_1_1:	pushm	esi,ecx
@@4_1_2:	mov	al,[esi]
	call	_CD_WriteByte
	inc	esi
	dec	ecx
	jnz	@@4_1_2

	popm	esi,ecx
	jmp	@@8

@@4_1_0:	;Do normal single raw byte.
	;
	stc
	call	_CD_WriteBit
	mov	al,[esi]
	call	_CD_WriteByte
	mov	ecx,1
	jmp	@@8
	;
@@4_1:	;Send repeat as byte distance, 2 bits for length.
	;
	clc
	call	_CD_WriteBit
	stc
	call	_CD_WriteBit
	mov	eax,_CD_FoundLen
	sub	eax,RepMinSize
	shl	eax,32-2
	shl	eax,1
	call	_CD_WriteBit
	shl	eax,1
	call	_CD_WriteBit
	mov	eax,esi
	sub	eax,_CD_FoundPos
	sub	eax,_CD_FoundLen
	inc	eax
	call	_CD_WriteByte
	mov	ecx,_CD_FoundLen
	jmp	@@8
	;
@@r4_1:	;Send run as zero distance, 2 bit length, byte to run.
	;
	clc
	call	_CD_WriteBit
	stc
	call	_CD_WriteBit
	mov	eax,_CD_RepLength
	sub	eax,RepMinSize+1
	shl	eax,32-2
	shl	eax,1
	call	_CD_WriteBit
	shl	eax,1
	call	_CD_WriteBit
	xor	eax,eax
	call	_CD_WriteByte
	mov	al,[esi]
	call	_CD_WriteByte
	mov	ecx,_CD_RepLength
	jmp	@@8
	;
@@4_2:	;Send repeat as 12 bit position, 4 bit length.
	;
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	stc
	call	_CD_WriteBit
	mov	eax,esi
	sub	eax,_CD_FoundPos
	sub	eax,_CD_FoundLen
	inc	eax
	shl	eax,4
	mov	ecx,_CD_FoundLen
	sub	ecx,RepMinSize
	or	eax,ecx
	call	_CD_WriteByte
	xchg	ah,al
	call	_CD_WriteByte
	mov	ecx,_CD_FoundLen
	jmp	@@8
	;
@@r4_2:	;send run as 12 bit zero, 4 bit length, byte run.
	;
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	stc
	call	_CD_WriteBit
	xor	eax,eax
	mov	ecx,_CD_RepLength
	sub	ecx,RepMinSize+1
	or	eax,ecx
	call	_CD_WriteByte
	xchg	ah,al
	call	_CD_WriteByte
	mov	al,[esi]
	call	_CD_WriteByte
	mov	ecx,_CD_RepLength
	jmp	@@8
	;
@@4_3:	;Send repeat as 12-bit position, 12 bit length.
	;
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	mov	eax,esi
	sub	eax,_CD_FoundPos
	sub	eax,_CD_FoundLen
	inc	eax
	mov	ecx,_CD_RepMaxShift
	shl	eax,cl
	mov	ecx,_CD_FoundLen
	sub	ecx,RepMinSize
	or	eax,ecx
	call	_CD_WriteByte
	xchg	ah,al
	call	_CD_WriteByte
	shr	eax,16
	call	_CD_WriteByte
	mov	ecx,_CD_FoundLen
	jmp	@@8
	;
@@r4_3:	;Send 12-bit zero, 12 bit length, byte run.
	;
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	xor	eax,eax
	mov	ecx,_CD_RepLength
	sub	ecx,RepMinSize+1
	or	eax,ecx
	call	_CD_WriteByte
	xchg	ah,al
	call	_CD_WriteByte
	shr	eax,16
	call	_CD_WriteByte
	mov	al,[esi]
	call	_CD_WriteByte
	mov	ecx,_CD_RepLength
	jmp	@@8
	;

@@8:

	;
	;Update search control data
	;
	pushm	eax,ebx,ecx,edx,edi


@@con0:
	;
	;Lose a code
	;
	cmp	ebp,_CD_RepMaxSize
	jc	@@con1
	mov	edi,esi
	sub	edi,ebp		;point to out-going code
	xor	edx,edx
	mov	dx,[edi]		;get out-going code
	shl	edx,3
	add	edx,CodeHeads	;point to out-going head
	cmp	d[edx],0		;NOT POSSIBLE!
	jz	@@bugger
	mov	ebx,[edx]		;point to this entry.
	mov	eax,[ebx]		;point to next entry.
	mov	[edx],eax		;set new head.
	cmp	ebx,d[edx+4]		;tail as well?
	jnz	@@con2
	mov	d[edx+4],eax		;set new tail.
@@con2:	mov	eax,CodeFree
	mov	CodeFree,ebx		;set new head value.
	mov	[ebx],eax
	jmp	@@con5
@@con1:	inc	ebp
@@con5:

	;
	;Add a code
	;
	shr	CodeLast,8		;rotate code buffer.
	xor	eax,eax
	mov	ah,[esi]		;in-comming value.
	or	CodeLast,eax		;add new value.
	inc	CodeCount
	cmp	CodeCount,2
	jc	@@con3
	mov	ebx,CodeFree		;pickup free entry.
	mov	eax,[ebx]
	mov	CodeFree,eax		;set new header.
	mov	edx,CodeLast		;get new code.
	shl	edx,3
	add	edx,CodeHeads	;point to head/tail
	mov	eax,[edx+4]		;get tail pointer
	mov	[edx+4],ebx		;set new tail
	or	eax,eax
	jz	@@con4
	mov	[eax],ebx		;set chain pointer.
@@con4:	mov	eax,esi		;CodePos
	dec	eax
	mov	[ebx+4],eax		;set code position.
	mov	d[ebx],0		;clear chain link.
	cmp	d[edx],0
	jnz	@@con3
	mov	[edx],ebx		;set head as well.

@@con3:	inc	CodePos
	inc	esi
	dec	ecx
	jnz	@@con0

	popm	eax,ebx,ecx,edx,edi



	;
	add	StatusOutPrint,ecx
	call	UpdateProgress
	add	_CD_SigInCount,ecx
	;
	;Check if we should send input flush code.
	;
	cmp	_CD_SigOutCount,8192-16
	jc	@@9_0
	pushad
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	mov	eax,1
	call	_CD_WriteByte
	xchg	ah,al
	call	_CD_WriteByte
	shr	eax,16
	call	_CD_WriteByte
	mov	_CD_SigOutCount,0
	popad
	;
@@9_0:	;Check if we should send output flush code.
	;
	cmp	_CD_SigInCount,8192-16
	jc	@@9_1
	pushad
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	mov	eax,2
	call	_CD_WriteByte
	xchg	ah,al
	call	_CD_WriteByte
	shr	eax,16
	call	_CD_WriteByte
	mov	_CD_SigInCount,0
	popad
	;
@@9_1:	sub	ebx,ecx
	jnz	@@0
;
;Send terminator code.
;
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	clc
	call	_CD_WriteBit
	xor	eax,eax
	call	_CD_WriteByte
	xchg	ah,al
	call	_CD_WriteByte
	shr	eax,16
	call	_CD_WriteByte
;
;Make sure we sent everything.
;
	cmp	_CD_OutCount,0
	jz	@@7
	mov	esi,_CD_OutCtrl
	mov	eax,_CD_OutDword
	mov	ecx,_CD_CtrlBits
	sub	ecx,_CD_OutCount
	shl	eax,cl
	cmp	_CD_CtrlBits,32
	jnc	@@7_0
	mov	w[_CD_OutBuffer+esi],ax
	jmp	@@7
@@7_0:	mov	d[_CD_OutBuffer+esi],eax
	;
@@7:	call	_CD_FlushBuffer
	;
	mov	ebx,_CD_OutHandle	;Get current file position so
	xor	ecx,ecx		;we know where to come back to.
	mov	al,1
	call	SetFilePointer
	xchg	ecx,_CD_OutStart	;store this position and set 
	add	ecx,4+4		;position to total output
	xor	al,al		;field of header.
	call	SetFilePointer
	mov	edx,offset _CD_OutTotal	;write a copy of total compressed
	mov	ecx,4		;data length.
	call	WriteFile
	mov	ecx,_CD_OutStart	;now restore file position to the
	xor	al,al		;end incase caller wants to write
	call	SetFilePointer	;stuff without thinking.
	add	_CD_OutTotal,4+4+4
	;
	clc
	jmp	_CD_Exit


@@bugger:	or	ax,-1
	mov	es,ax

EncodeFile	endp


;------------------------------------------------------------------------------
;
;Write a bit to output.
;
;On Entry:
;
;Carry	- Bit to send.
;
_CD_WriteBit	proc	near
	rcl	_CD_OutDword,1
	inc	_CD_OutCount
	push	eax
	mov	eax,_CD_CtrlBits
	cmp	_CD_OutCount,eax
	pop	eax
	jc	@@wb2
	pushm	eax,esi
	mov	_CD_OutCount,0
	mov	esi,_CD_OutCtrl
	mov	eax,_CD_OutDword
	cmp	_CD_CtrlBits,32
	jnc	@@wb1
	mov	w[_CD_OutBuffer+esi],ax
	jmp	@@wb3
@@wb1:	mov	d[_CD_OutBuffer+esi],eax
@@wb3:	popm	eax,esi
	cmp	_CD_OutPos,4096
	jc	@@wb0
	call	_CD_FlushBuffer
@@wb0:	push	eax
	mov	eax,_CD_OutPos
	mov	_CD_OutCtrl,eax
	mov	eax,_CD_CtrlSize
	add	_CD_OutPos,eax
	add	_CD_SigOutCount,eax
	pop	eax
@@wb2:	ret
_CD_WriteBit	endp


;------------------------------------------------------------------------------
;
;Write raw data.
;
;On Entry:
;
;ECX	- Length to write.
;ESI	- Source to write.
;
_CD_WriteByte	proc	near
	push	esi
	mov	esi,_CD_OutPos
	mov	b[_CD_OutBuffer+esi],al
	inc	_CD_OutPos
	inc	_CD_SigOutCount
	pop	esi
	ret
_CD_WriteByte	endp


;------------------------------------------------------------------------------
;
;Flush the output buffer.
;
_CD_FlushBuffer proc near
	pushad
	xor	ecx,ecx
	xchg	ecx,_CD_OutPos
	add	_CD_OutTotal,ecx
	mov	edx,offset _CD_OutBuffer
	mov	ebx,_CD_OutHandle
	call	WriteFile
	popad
	ret
_CD_FlushBuffer endp


;------------------------------------------------------------------------------
_CD_Error	proc	near
	mov	esp,_CD_InESP
	stc
	jmp	_CD_Exit
_CD_Error	endp


;------------------------------------------------------------------------------
_CD_Exit	proc	near
	pushf
	cmp	CodeHeads,0
	jz	@@0
	mov	esi,CodeHeads
	call	free
	mov	CodeHeads,0
@@0:	cmp	CodeFree+4,0
	jz	@@1
	mov	esi,CodeFree+4
	call	free
	mov	CodeFree+4,0
@@1:	popf
	popad
	mov	ecx,_CD_OutTotal	;return output size.
	ret
_CD_Exit	endp


;------------------------------------------------------------------------------
;
;Feed ShowProgress updates in chunk sizes it can cope with.
;
UpdateProgress	proc	near
	push	eax
@@0:	mov	eax,StatusOutPrint	;New count.
	sub	eax,StatusBlockCount
	cmp	eax,256
	jc	@@9
	add	StatusBlockCount,256
	push	StatusNoBar
	call	ShowProgress
	pop	StatusNoBar
	jmp	@@0
@@9:	mov	StatusNoBar,0
	pop	eax
	ret
UpdateProgress	endp


;------------------------------------------------------------------------------
;
;Update status progress display.
;
ShowProgress	proc	near
	pushad
	;
	;Do diamonds...
	;
	cmp	StatusNoBar,1
	mov	StatusNoBar,0
	jl	@@Bar
	jg	@@NoBar
	;
	mov	edx,StatusCursorPos
	add	dl,5		;skip %
	mov	ah,02h		;Set cursor position.
	xor	bh,bh		;/
	int	10h		;/
	mov	ecx,StatusPrintSize	;get % print pos.
	add	ecx,5		;allow for operation/% text.
	sub	ecx,79		;width of the screen
	jns	@@NoBar
	neg	ecx
@@nb0:	push	ecx
	mov	dl," "		;get character to use.
	mov	ah,2
	int	21h
	pop	ecx
	dec	ecx
	jnz	@@nb0
	jmp	@@NoBar
@@Bar:	;
	mov	ebx,StatusPrintSize	;get % print pos.
	add	ebx,5		;allow for operation/% text.
	sub	ebx,79		;width of the screen
	jns	@@400		;no room to print.
	neg	ebx
	mov	eax,StatusBlockCount	;get number required.
	shr	eax,8
	xor	edx,edx
	idiv	ebx
	and	eax,3
	mov	StatusChar,al	;store wraps.
	mov	ecx,edx		;fetch this count.
	test	StatusChar,1
	jz	@@401
	sub	ecx,ebx		;reverse it this time.
	neg	ecx
	dec	ecx		;who knows & who cares..
@@401:	mov	edx,StatusCursorPos
	add	dl,5		;skip %
	add	dl,cl		;add current pos.
	mov	ah,02h		;Set cursor position.
	xor	bh,bh		;/
	int	10h		;/
	movzx	ebx,StatusChar
	mov	dl,b[StatusCharTab+ebx]	;get character to use.
	mov	ah,2
	int	21h
@@NoBar:	;
	mov	eax,StatusBlockCount
	mov	ebx,StatusTotalLength
	call	MakePercent
	;
	mov	edi,offset StatusBuffer
	mov	ebx,100
	xor	edx,edx
	div	ebx
	or	al,al
	jnz	@@4
	mov	al,' '-'0'
@@4:	add	al,'0'
	mov	[edi],al
	inc	edi
	mov	ebx,10
	mov	eax,edx
	xor	edx,edx
	div	ebx
	add	al,'0'
	mov	[edi],al
	inc	edi
@@6:	mov	eax,edx
	add	al,'0'
	mov	[edi],al
	;
	mov	edx,StatusCursorPos
	mov	ah,02h		;Set cursor position.
	xor	bh,bh		;/
	int	10h		;/
	mov	esi,offset StatusBuffer
	call	PrintString
	;
@@400:	popad
	ret
ShowProgress	endp


;------------------------------------------------------------------------------
;
;Work out percentage.
;
;On Entry:-
;
;EAX	- Current value.
;EBX	- %100
;
;On Exit:-
;
;AX	- %
;
MakePercent	proc	near
	push	edx
	mov	edx,100
	mul	edx
	div	ebx
	cmp	eax,100
	jc	@@9
	mov	eax,100
@@9:	pop	edx
	ret
MakePercent	endp


;------------------------------------------------------------------------------
;
;Print an error message but don't exit the program.
;
FileErrorPrint	proc	near
	mov	edx,CursorPos
	add	dl,40
	mov	ah,02h		;Set cursor position.
	xor	bh,bh		;/
	int	10h		;/
	;
	mov	esi,ErrorNumber
	mov	esi,[ErrorList+esi*4]
	call	PrintString
	ret
FileErrorPrint	endp


;------------------------------------------------------------------------------
;
;Work out what sort of file we're dealing with.
;
;On Entry:
;
;EDX	- file name.
;
;On Exit:
;
;Carry set on error else,
;
;EAX	- file type.
;	0 - EXE real mode.
;	1 - 3P stand alone.
;	2 - EXE with 3P attatched.
;	3 - Data if enabled.
;
GetFileType	proc	near
	call	OpenFile
	jc	@@9
	mov	edx,offset ExeSignature
	mov	ecx,2
	call	ReadFile
	jc	@@8
	cmp	eax,ecx
	jnz	@@8
	mov	eax,1
	cmp	w[ExeSignature],"P3"	;Stand alone 3P?
	jz	@@7
	mov	eax,3
	cmp	w[ExeSignature],"ZM"	;Real mode EXE?
	jnz	@@7
	mov	ecx,1bh-2		;Better read the rest of the
	mov	edx,offset ExeSignature+2	;header so we can look for a
	call	ReadFile		;3P on the end.
	jc	@@8
	cmp	eax,ecx		;check we read enough.
	jnz	@@8
	;
	mov	ax,w[ExeLength+2]	;get length in 512 byte blocks

; MED 01/17/96
	cmp	WORD PTR [ExeLength],0
	je	medexe4	; not rounded if no modulo

	dec	ax		;lose 1 cos its rounded up

medexe4:
	add	ax,ax		;mult by 2
	mov	dh,0
	mov	dl,ah
	mov	ah,al
	mov	al,dh		;mult by 256=*512
	add	ax,w[ExeLength]	;add length mod 512
	adc	dx,0		;add any carry to dx
	mov	cx,ax
	xchg	cx,dx
	shl	ecx,16
	mov	cx,dx
	sub	ecx,1bh
	mov	al,1
	call	SetFilePointer	;move to end of this chunk.
	;
	mov	ecx,2
	mov	edx,offset EXESignature
	call	ReadFile		;read a header again.
	jc	@@8
	cmp	eax,ecx
	jnz	@@0
	;
	mov	eax,2
	cmp	w[ExeSignature],"P3"	;Tagged on 3P?
	jz	@@7
@@0:	mov	eax,0		;real mode EXE.
	;
@@7:	clc
	jmp	@@6
	;
@@8:	stc
	;
@@6:	pushf
	push	eax
	call	CloseFile
	pop	eax
	popf
@@9:	ret
GetFileType	endp


	sdata
;
ErrorStack	dd ?
ErrorNumber	dd 0
ErrorList	dd ErrorM00,ErrorM01,ErrorM02,ErrorM03,ErrorM04,ErrorM05,ErrorM06,ErrorM07
	dd ErrorM08,ErrorM09,ErrorM10
ErrorM00	db 0
ErrorM01	label byte
	if ENGLISH
	db "     Usage: CWC [options] filename [options]",13,10
	db 13,10
	db "   Options:",13,10
	db 13,10
	db " d     - Treat input file as Data (Expand with cwcLoad).",13,10
	db " e     - Expand data file (EXE's are locked).",13,10
	db " l     - Literal string length, eg, /l85.",13,10
	db " m2/m3 - Method (default is 1).",13,10
	db 13,10
	db " The options above that have a + before them are ON by default.",13,10
	db " Switches are ON with + or /, OFF with -.",13,10
	db " The command line can be in any order.",10,13
	db 13,10,13,10,0
	elseif SPANISH
	db "       Uso: CWC [opciones] fichero [opciones]",13,10
	db 13,10
	db "  Opciones:",13,10
	db 13,10
	db " d     - Trata el fichero de entrada como datos (Expandir con cwcLoad).",13,10
	db " e     - Expand data file (EXE's are locked).",13,10
	db " l     - Literal string length, eg, /l85.",13,10
	db " m2/m3 - M‚todo de compresi¢n (1 por defecto).",13,10
	db 13,10
	db " Las opciones que tienen + est n activadas por defecto.",13,10
	db " Para activar un switch use + o /, para desactivarlo, use -.",13,10
	db " La l¡nea de mandatos puede estar en cualquier orden.",13,10
	db 13,10,13,10,0
	endif
ErrorM02	label byte
	if ENGLISH
	db "Not enough memory.",13,10,0
	elseif SPANISH
	db "Memoria insuficiente.",13,10,0
	endif
ErrorM03	label byte
	if ENGLISH
	db "Could not find any files.",13,10,0
	elseif SPANISH
	db "Pueden no encontrarse algunos ficheros.",13,10,0
	endif
ErrorM04	label byte
	if ENGLISH
	db "Unknown format.",13,10,0
	elseif SPANISH
	db "Formato desconocido.",13,10,0
	endif
ErrorM05	label byte
	if ENGLISH
	db "File not found.",13,10,0
	elseif SPANISH
	db "Fichero no encontrado.",13,10,0
	endif
ErrorM06	label byte
	if ENGLISH
	db "Out of disk space.",13,10,0
	elseif SPANISH
	db "No hay espacio en disco.",13,10,0
	endif
ErrorM07	label byte
	if ENGLISH
	db "Read error.",13,10,0
	elseif SPANISH
	db "Error de lectura.",13,10,0
	endif
ErrorM08	label byte
	if ENGLISH
	db "Write error.",13,10,0
	elseif SPANISH
	db "Error de escritura.",13,10,0
	endif
ErrorM09	label byte
	if ENGLISH
	db "Already compressed.",13,10,0
	elseif SPANISH
	db "Ya est  comprimido.",13,10,0
	endif
ErrorM10	db "Can't expand EXE's.",13,10,0
;
CRText	db 13,10,0
FileList	dd 0
CursorPos	dd 0
FileName	db 256 dup (?)
TempFileName	db 256 dup (0)
TempBase	db '\ckanetdy.ckq',0
;
ExeSignature	db ?	;00 Identifier text.
	db ?	;01 /
ExeLength	dw ?	;02 Length of file MOD 512
	dw ?	;04 Length of file in 512 byte blocks.
ExeRelocNum	dw ?	;06 Number of relocation items.
ExeHeaderSize	dw ?	;08 Length of header in 16 byte paragraphs.
ExeMinAlloc	dw ?	;0A Minimum number of para's needed above program.
ExeMaxAlloc	dw ?	;0C Maximum number of para's needed above program.
ExeStackSeg	dw ?	;0E Segment displacement of stack module.
ExeEntrySP	dw ?	;10 value for SP at entry.
ExeCheckSum	dw ?	;12 Check sum...
ExeEntryIP	dw ?	;14 Contents of IP at entry.
ExeEntryCS	dw ?	;16 Segment displacement of CS at entry.
ExeRelocFirst	dw ?	;18 First relocation item offset.
ExeOverlayNum	db ?	;1A Overlay number.
	db 20h-1bh dup (0)
;
NewHeader	NewHeaderStruc <>	;make space for a header.
;
StatusPrintSize dd 40
StatusCursorPos dd 0
StatusOutPrint	dd ?
StatusBlockCount dd 0
StatusTotalLength dd ?
StatusNoBar	dd 0
StatusChar	db 0
StatusBuffer	db '   %',0
StatusCharTab	db 4,249,254,249
;
_CD_InESP	dd ?
_CD_Flags	dd ?
_CD_OutHandle	dd ?
_CD_OutDword	dd ?
_CD_OutCount	dd ?
_CD_OutPos	dd ?
_CD_SigOutCount dd ?
_CD_SigInCount	dd ?
_CD_OutCtrl	dd ?
_CD_CtrlSize	dd 4
_CD_CtrlBits	dd 32
_CD_OutBuffer	db 32768 dup (0)
	db 32768 dup (0)
	db 32768 dup (0)
	db 32768 dup (0)
_CD_FoundPos	dd ?
_CD_FoundLen	dd ?
_CD_LastLen	dd ?
_CD_LastPos	dd ?
_CD_RepLength	dd ?
_CD_OutTotal	dd ?
_CD_OutStart	dd ?
_CD_RepMaxLen	dd 4096+RepMinSize-1
_CD_RepMaxSize	dd 4096+RepMinSize-1
_CD_RepMaxShift dd 12
_CD_IDText	db "CWC",12
;
_CD_EncodeLit	dd 0
;
_CD_RepMaxTab	db 0,12,9,8
;
_DCD_Stack	dd ?
_DCD_Handle	dd ?
;
EXECopyStub	label byte
	include copystub.inc
EXECopyStubLen	equ	$-EXECopyStub
	db 16 dup (0)

EXEDecStub	label byte
	include decstub.inc
EXEDecStubLen	equ	$-EXEDecStub
	db 16 dup (0)

DummyData	db 16 dup (0)

EXEextension	db 'EXE',0

CodeHeads	dd 0
CodeFree	dd ?,0
CodeLast	dd ?
CodeCount	dd ?
CodePos	dd ?


	efile
	end Startup

