	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Convert normal names to lower case and directory names to upper case.
;
;On Entry:
;
;ESI	- pointer to file list.
;
;On Exit:
;
;ALL registers preserved.
;
;Most routines that need to display a file list want directory names in upper
;case and the remaining names in lower case. This routine formats a file list
;to that specification. Doing things this way means that if the file list is
;then sorted, directory names will appear at the top.
;
ULFileList	proc	near
	pushm	eax,ecx,esi,edi
	mov	ecx,[esi]		;Get number of entries.
	jecxz	l9
	add	esi,4+256
l0:	test	b[esi+13],16		;Directory?
	jnz	l4
	push	esi
	mov	edi,esi
l1:	lodsb
	or	al,al
	jz	l3
	cmp	al,61h		; 'a'
	jb	l2
	cmp	al,7Ah		; 'z'
	ja	l2
	and	al,5Fh		;convert to upper case.
l2:	cmp	al,"A"
	jc	l5
	cmp	al,"Z"+1
	jnc	l5
	sub	al,"A"
	add	al,"a"
l5:	stosb
	jmp	l1
l3:	pop	esi
l4:	add	esi,14
	loop	l0
l9:	popm	eax,ecx,esi,edi
	ret
ULFileList	endp


	efile
	end

