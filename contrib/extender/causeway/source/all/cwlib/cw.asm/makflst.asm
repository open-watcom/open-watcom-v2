	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Generate list of files that match a given file mask. Wild cards permitted.
;
;On Entry:
;
;EDX	- File name mask (can include a path).
;CX	- search attributes.
;
;On Exit:
;
;EAX	- buffer with file names etc, EAX=0 if not enough memory.
;
;Each entry in the buffer will have the following format.
;
;13 bytes zero terminated ASCII file name (may be less)
; 1 byte file attribute (directory,vulume,hidden,system etc.)
;
; If you specify directories in the attribute search mask the names . & .. will
; be removed from the list to avoid making special checks for them. Anything
; that might have found these useful will have to have knowledge of its
; current path anyway.
;
;
;Attributes are as follows:-
;
;  5  4  3  2  1  0
;  |  |  |  |  |  |
;  |  |  |  |  |  \_ Read only. (1)
;  |  |  |  |  \____ Hidden.    (2)
;  |  |  |  \_______ System.    (4)
;  |  |  \__________ Label.     (8)
;  |  \_____________ Directory. (16)
;  \________________ Archive.   (32)
;
; Bits 6-15 are reserved and may be used in the search mask, but bits 8-15 won't
; appear in the attributes for listed files.
;
; The first dword in the buffer is the number of entries.
; Next 256 bytes is the path to append the names to.
;
MakeFileList	proc	near
	pushm	ebx,ecx,edx,esi,edi
	mov	FileList,0
	mov	FileMask,edx
	mov	FileAttr,ecx
	;
	push	es
	mov	ah,2fh
	int	21h
	mov	d[OldDta],ebx
	mov	w[OldDta],es
	pop	es
	mov	edx,offset DTABuffer	;set DTA address.
	mov	ah,1ah
	int	21h
	;
	mov	ecx,4+256
	call	Malloc
	jc	@@9
	mov	FileList,esi
	mov	d[esi],0
	mov	b[esi+4],0
	;
	mov	esi,FileMask
	cmp	b[esi],0
	jz	@@mask
	cmp	b[esi+1],0
	jz	@@mask
	cmp	b[esi+1],":"
	jz	@@mask
	;
	;No drive in the path so get the current path.
	;
	mov	edx,FileList
	add	edx,4
	call	GetPath
	mov	esi,FileMask
	;
@@mask:	mov	edi,FileList
	add	edi,4
	xchg	esi,edi
@@m0:	lodsb
	or	al,al
	jnz	@@m0
	dec	esi
	xchg	esi,edi
	mov	eax,FileList
	add	eax,4
	cmp	edi,eax
	jz	@@m1
	cmp	b[edi-1],"\"
	jz	@@m1
	mov	al,"\"
	stosb
@@m1:	movsb
	cmp	b[esi-1],0
	jnz	@@m1
	;
	;Make sure we skip any drive or path specs in the mask.
	;
	mov	esi,FileMask
	mov	edx,esi
@@m02:	lodsb
	cmp	al,"\"
	jnz	@@m03
	mov	edx,esi
@@m03:	or	al,al
	jnz	@@m02
	mov	FileMask,edx
	;
	test	FileAttr,16
	jz	@@Normal
	mov	esi,offset StarDotStar
	jmp	@@first
@@Normal:	mov	esi,FileMask
@@First:	mov	edi,FileList
	add	edi,4
	xchg	esi,edi
	mov	edx,esi
@@l0:	lodsb
	cmp	al,"\"
	jnz	@@l1
	mov	edx,esi
@@l1:	or	al,al
	jnz	@@l0
	mov	esi,edx
	xchg	esi,edi
@@l2:	movsb
	cmp	b[esi-1],0
	jnz	@@l2
	;
	mov	edx,FileList
	add	edx,4
	mov	ecx,FileAttr
	mov	ah,4eh		;find first file
	int	21h
	jc	@@99
	jmp	@@2
	;
@@0:	mov	ah,4fh
	int	21h
	jc	@@99
	;
@@2:	mov	al,DtaBuffer+21	;get attributes.
	test	al,16
	jz	@@3		;not a directory.
	cmp	DTABuffer+1eh,'.'
	jnz	@@30		;not current.
	cmp	DTABuffer+1fh,0
	jz	@@0		;ignore current.
	cmp	DTABuffer+1fh,'.'
	jz	@@0		;ignore previouse.
	jmp	@@30
	;
@@3:	test	FileAttr,16
	jnz	@@0
	;
@@30:	mov	esi,FileList
	mov	eax,14
	mul	d[esi]
	add	eax,4+256+14
	mov	ecx,eax
	call	ReMalloc
	jc	@@9
	mov	FileList,esi
	inc	d[esi]
	mov	edi,esi
	add	edi,ecx
	sub	edi,14
	;
	mov	esi,offset DtaBuffer+1eh	;point to file name.
	mov	ecx,13
	cld
@@4:	movsb			;copy the name and check for
	cmp	b[esi-1],0		;a terminator. Not needed really
	jz	@@5		;but it keeps the buffer tidy.
	loop	@@4
	jmp	@@6
@@5:	dec	ecx
	jz	@@6
	xor	al,al
	rep	stosb		;fill it out with 0's.
@@6:	mov	al,DtaBuffer+21	;get attributes.
	and	al,63
	stosb			;store them.
	jmp	@@0
	;
@@99:	test	FileAttr,16
	jz	@@90
	and	FileAttr,not 16
	jmp	@@Normal
	;
@@90:	;lose the mask from the path.
	;
	mov	esi,FileList
	add	esi,4
	mov	edi,esi
@@m3:	lodsb
	cmp	al,"\"
	jnz	@@m4
	mov	edi,esi
	dec	edi
@@m4:	or	al,al
	jnz	@@m3
	stosb
	;
@@9:	push	ds
	lds	edx,f[OldDta]	;set DTA address.
	mov	ah,1ah
	int	21h
	pop	ds
	;
	mov	eax,FileList
	or	eax,eax
	jnz	@@10
	clc
@@10:	cmc
	popm	ebx,ecx,edx,esi,edi
	ret
MakeFileList	endp


	sdata

FileList	dd 0
FileAttr	dd 0
FileMask	dd 0
OldDta	df 0
DTABuffer	db 128 dup (0)
StarDotStar	db "*.*",0


	efile
	end

