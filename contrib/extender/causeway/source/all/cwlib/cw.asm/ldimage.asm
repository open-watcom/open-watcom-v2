	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Load an image file.
;
;On Entry:
;
;EDX	- pointer to name of file to load.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - Bad file format.
;	2 - File access error.
;	3 - Not enough memory.
;ESI	- Bitmap pointer.
;
LoadImageFile	proc	near
	pushm	ebx,ecx,edx,edi,ebp
	;
	mov	_LIF_Exten,'    '
	mov	esi,edx
	mov	edi,-1
@@2:	lodsb
	cmp	al,'.'
	jz	@@3
	or	al,al
	jnz	@@2
	jmp	@@4
@@3:	mov	edi,esi
	jmp	@@2
@@4:	cmp	edi,-1
	jz	@@40
	mov	esi,edi
	;
	mov	eax,[esi]
	and	eax,0ffffffh
	call	UpperChar
	ror	eax,8
	call	UpperChar
	ror	eax,8
	call	UpperChar
	rol	eax,16
	mov	_LIF_Exten,eax
	;
@@40:	cmp	_LIF_Exten,'AGT'	;use extension for TGA's.
	jz	@@DoTGA
	cmp	_LIF_Exten,"XCP"	;use extension for PCX's.
	jz	@@DoPCX
	cmp	_LIF_Exten,"MB"	;use extension for BM's.
	jz	@@DoBM
	cmp	_LIF_Exten," MB"	;use extension for BM's.
	jz	@@DoBM
	call	OpenFile
	jc	@@open_error
	push	edx
	mov	edx,offset _LIF_Buffer
	mov	ecx,16
	call	ReadFile		;read header.
	pop	edx
	pushm	edx,eax
	pushf
	call	CloseFile
	popf
	popm	edx,eax
	jc	@@file_error		;exit on error.
	cmp	eax,ecx		;did we read enough?
	jnz	@@form_error
	;
	;Now investigate the header to see what file format we're
	;dealing with.
	;
	cmp	_LIF_Buffer+0,'G'
	jnz	@@0
	cmp	_LIF_Buffer+1,'I'
	jnz	@@0
	cmp	_LIF_Buffer+2,'F'
	jnz	@@0
	call	LoadGIF		;read a gif file.
	or	eax,eax
	jnz	@@9
	jmp	@@8
	;
@@0:	cmp	d[_LIF_Buffer],'MROF'
	jnz	@@1
	call	LoadIFF		;read iff file.
	or	eax,eax
	jnz	@@9
	jmp	@@8
	;
@@1:	mov	eax,1		;bad format.
	jmp	@@9
	;
@@DoPCX:	call	LoadPCX
	or	eax,eax
	jnz	@@9
	jmp	@@8
	;
@@DoBM:	call	LoadBM
	or	eax,eax
	jnz	@@9
	jmp	@@8
	;
@@DoTGA:	call	LoadTGA
	or	eax,eax
	jnz	@@9
	jmp	@@8
	;
@@8:	jmp	@@9
	;
@@file_error:	;
@@open_error:	mov	eax,2
	stc
	jmp	@@9
	;
@@form_error:	mov	eax,1
	stc
	;
@@9:	popm	ebx,ecx,edx,edi,ebp
	ret
LoadImageFile	endp


	sdata
;
_LIF_Buffer	db 16 dup (0)
_LIF_Exten	dd ?


	efile
	end
