	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Sort a file list into alpha-numeric order. Assumes you want DIR's at the top
;of the list.
;
;On Entry:
;
;ESI	- pointer to file list to sort.
;
;On Exit:
;
;ALL registers preserved.
;
;File names are not case converted so upper case names will appear at the top
;of the list.
;
SortFileList	proc	near
	pushm	eax,ecx,edx,esi,edi
	mov	ecx,[esi]		;Get number of entries.
	add	esi,4+256
	or	ecx,ecx
	jz	l9
	dec	ecx
	jz	l9
	mov	edi,esi
	add	edi,14
	mov	edx,ecx
l5:	pushm	esi,edi,edx,ecx
l4:	pushm	esi,edi,ecx
	test	b[esi+13],16		;Directory?
	jz	l6
	test	b[edi+13],16		;Directory?
	jnz	l7
	jmp	l3		;Force dir's to the top.
l6:	test	b[edi+13],16		;Directory?
	jnz	l1		;Force dir's to the top.
l7:	mov	ecx,12
l0:	mov	al,[esi]
	mov	ah,[edi]
	inc	esi
	inc	edi
	cmp	ah,al
	jc	l1
	jnz	l3
	loop	l0
	jmp	l3
	;
l1:	popm	esi,edi,ecx
	pushm	esi,edi,ecx
	mov	ecx,14
l2:	mov	al,[edi]
	xchg	al,[esi]
	stosb
	inc	esi
	loop	l2
l3:	popm	esi,edi,ecx
	add	esi,14
	add	edi,14
	loop	l4
	popm	esi,edi,edx,ecx
	dec	edx
	jnz	l5
	;
l9:	popm	eax,ecx,edx,esi,edi
	ret
SortFileList	endp


	efile
	end

