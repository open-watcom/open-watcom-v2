	include ..\cwlib.inc
	include ..\cw.inc

	ifndef CWNEAR
	.stack 1024
	endif

	scode

CW_DEBUG_ENTRY_CS_EIP df _Main
	public CW_DEBUG_ENTRY_CS_EIP

;-------------------------------------------------------------------------
;
;Setup things as needed by specific extender/model so that things always
;look the same.
;
Startup	proc	near
;
;Store useful values.
;
	mov	ax,DGROUP
	mov	ds,ax
	mov	PSPSegment,es
	mov	ax,es:w[2ch]
	mov	ENVSegment,ax
	push	ds
	pop	es
;
;Try to lock programs memory. Uses currently un-documented entries in the PSP.
;
	mov	fs,PSPSegment
	mov	esi,fs:[EPSP_MemBase]
	mov	ecx,fs:[EPSP_MemSize]
	sys	LockMem32
	mov	edx,offset PhysicalMemError
	jc	an_error
;
;Get a code segment alias.
;
	mov	bx,cs
	sys	AliasSel
	mov	edx,offset SelectorError
	jc	an_error
	mov	CodeSegAlias,ax
;
;Get linear base of DS
;
	mov	bx,ds
	sys	GetSelDet32
	mov	DataLinearBase,edx
	or	ecx,-1
	sys	SetSelDet32
;
;Move ESP into the data segment.
;
	mov	bx,ds
	sys	GetSelDet32
	mov	esi,edx
	mov	bx,ss
	sys	GetSelDet32
	sub	edx,esi
	mov	ax,ds
	mov	ss,ax
	add	esp,edx
;
;Get 0-4G selector.
;
	pushad
	push	es
	sys	Info
	mov	ZeroSelector,ax
	pop	es
	popad
;
;Install a critical error handler.
;
	mov	bl,24h
	sys	GetVect
	mov	d[OldInt24],edx
	mov	w[OldInt24+4],cx
	mov	edx,offset CriticalHandler
	mov	cx,cs
	sys	SetVect
;
;Get main application file name.
;
	mov	es,ENVSegment
	xor	esi,esi
l0:	mov	al,es:[esi]		;Get a byte.
	inc	esi		;/
	or	al,al		;End of a string?
	jnz	l0		;keep looking.
	mov	al,es:[esi]		;Double zero?
	or	al,al		;/
	jnz	l0		;keep looking.
	add	esi,3		;Skip last 0 and word count.
	mov	edi,offset ExecutionPath
	mov	ecx,128
	pushm	ds,ds,es
	popm	es,ds
l2:	movsb
	cmp	b[esi-1],0		;got to the end yet?
	jz	l3
	loop	l2
l3:	pop	ds
	push	edi
	mov	ecx,edi
	mov	esi,offset ExecutionPath
	mov	edi,offset ExecutionName
	sub	ecx,esi
	rep	movsb
	pop	edi
l4:	cmp	b[edi-1],"\"
	jz	l5
	cmp	b[edi-1],":"
	jz	l5
	dec	edi
	jmp	l4
l5:	mov	b[edi],0
;
;Call the main code.
;
	call	_Main
	push	eax
;
;Check if we should remove critical error handler.
;
	cmp	w[OldInt24+4],0
	jz	l9
	mov	bl,24h
	mov	edx,d[OldInt24]
	mov	cx,w[OldInt24+4]
	sys	SetVect
;
;terminate.
;
l9:	pop	eax
	mov	ah,4ch
	int	21h
;
;Error conditions during startup come here.
;
an_error:	mov	ah,9
	int	21h
	mov	eax,-1
	push	eax
	jmp	l9
Startup	endp


;-------------------------------------------------------------------------
;
;Critical error handler, returns FAIL code so just checking for carry will
;take care of things.
;
CriticalHandler proc far
	mov	ax,3
	iretd
CriticalHandler endp


	sdata

PhysicalMemError db "Not enough PHYSICAL memory free to run this program.",13,10,"$"
SelectorError	db "Not enough selectors available from host.",13,10,"$"

_CodeSegAlias	label word
	public _CodeSegAlias
CodeSegAlias	dw ?
_ZeroSelector	label word
	public _ZeroSelector
ZeroSelector	dw ?
_PSPSegment	label word
	public _PSPSegment
PSPSegment	dw ?
_ENVSegment	label word
	public _ENVSegment
ENVSegment	dw ?
_DataLinearBase label dword
	public _DataLinearBase
DataLinearBase	dd ?
_ExecutionPath	label byte
	public _ExecutionPath
ExecutionPath	label byte
	db 256 dup (0)
_ExecutionName	label byte
	public _ExecutionName
ExecutionName	db 256 dup (0)

OldInt24	df 0


	efile
	end

