	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Load font file into a slot.
;
;On Entry:
;
;EDX	- file name.
;ECX	- slot number.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file not found or I/O error.
;	2 - not enough memory.
;
;ALL other registers preserved.
;
LoadFont	proc	near
	pushm	ebx,ecx,edx,esi,edi
	mov	_LF_Addr,0
;
;Check if we should lose a previous entry first.
;
	mov	_LF_Entry,ecx
	cmp	[FontTable+ecx*4],0
	jz	@@0
	mov	esi,[FontTable+ecx*4]
	call	Free
	mov	[FontTable+ecx*4],0
;
;Make an execution path relative name.
;
@@0:	mov	esi,offset ExecutionPath
	mov	edi,offset _LF_Name
	call	CopyString
	mov	esi,edx
	call	AppendString
	mov	edx,offset _LF_Name
;
;Find out how big the file is.
;
	call	OpenFile
	jc	@@9_1
	mov	al,2
	xor	ecx,ecx
	call	SetFilePointer
	call	CloseFile
;
;Try and allocate the memory.
;
	call	malloc
	jc	@@9_2
	mov	eax,_LF_Entry
	mov	[FontTable+eax*4],esi
	mov	_LF_Addr,esi
;
;Load the file.
;
	call	OpenFile
	jc	@@9_1
	mov	edx,esi
	call	ReadFile
	cmp	eax,ecx
	jnz	@@9_1
;
;If its slot zero then copy details into system variables.
;
	cmp	_LF_Entry,0
	jnz	@@1
	mov	SystemFont,esi
	movzx	eax,b[esi+0]
	mov	FontWidth,eax
	movzx	eax,b[esi+1]
	mov	FontDepth,eax
;
;re-locate font table values to make them real.
;
@@1:	mov	ecx,255
	mov	eax,esi
	add	esi,4
@@2:	add	d[esi],eax
	add	esi,4
	loop	@@2
	;
	xor	eax,eax
	jmp	@@9
;
;File not found or I/O error.
;
@@9_1:	mov	eax,1
	jmp	@@9
;
;Not enough memory error.
;
@@9_2:	mov	eax,2
;
;General exit point.
;
@@9:	or	eax,eax
	jz	@@10
	cmp	_LF_Addr,0
	jz	@@10
	mov	esi,_LF_Addr
	call	Free
	mov	ecx,_LF_Entry
	mov	[FontTable+ecx*4],0
@@10:	popm	ebx,ecx,edx,esi,edi
	ret
LoadFont	endp


	sdata

_LF_Entry	dd ?
_LF_Addr	dd ?
_LF_Name	db 256 dup (?)

FontTable	label dword
	dd 16 dup (0)
SystemFont	dd 0
FontWidth	dd ?
FontDepth	dd ?


	efile
	end

