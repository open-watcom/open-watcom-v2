	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Read the command line tail for parameters.
;
;supports / or - or + as switch/option settings. Options and names may be in
;any order, text can be tagged onto options, names must be in correct sequence
;for current program.
;
;Each character ( 33 to 127) has an entry in OptionTable & OptionTable+128.
;
;The first entry is a byte, and is 0 for OFF & none-zero for ON.
;The second entry is a pointer to any additional text specified, 0 means none.
;Entries 0-32 are reserved for text not preceded by - or + or /. These are
;intended to be file names, but neadn't be.
;All text entries are 0 terminated.
;
;OptionCounter	 - Total command line parameters (files & switches).
;OptionTable	 - Switch table, ASCII code is index to check.
;OptionPointers - Pointer table, ASCII code*4 is index to use.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
ReadCommand	proc	near
	pushad
	push	es
	mov	es,PSPSegment
	mov	edi,offset OptionText
	mov	OptionPointer,edi
	mov	esi,80h		;/
	xor	ecx,ecx
	mov	cl,es:[esi]		;get tail length
	cmp	cl,2
	jc	l9		;not long enough!
	mov	edi,esi
	add	edi,ecx
	mov	es:b[edi+1],0	;terminate the tail.
	inc	si		;skip length.
	;
l0:	mov	al,es:[esi]		;need to skip leading spaces.
	inc	esi		;/
	or	al,al		;/
	jz	l9		;/
	cmp	al,' '		;/
	jz	l0		;/
	dec	esi		;/
	;
l1:	cmp	es:b[esi],'/'	;option switch?
	jz	lOption		;/
	cmp	es:b[esi],'-'	;/
	jz	lOption		;/
	cmp	es:b[esi],'+'	;/
	jz	lOption		;/
	;
l2:	xor	ebx,ebx		;/
	mov	bl,OptionCounter	;Get file entry number.
	inc	OptionCounter	;/
	shl	ebx,2		;/
	add	ebx,offset OptionTable+128 ;/
	mov	edi,OptionPointer	;Current free space pointer.
	mov	[ebx],edi		;update table entry.
	;
	xor	cl,cl
l3:	cmp	es:b[esi],0		;end of name?
	jz	l4		;/
	cmp	es:b[esi],' '	;/
	jz	l4		;/
	mov	al,es:[esi]		;Copy this character.
	mov	[edi],al		;/
	inc	esi		;/
	inc	edi		;/
	mov	cl,1		;flag SOMETHING found.
	jmp	l3		;keep fetching them.
	;
l4:	mov	b[edi],0		;Terminate the name.
	inc	edi		;/
	mov	OptionPointer,edi	;Update table pointer.
	;
	or	cl,cl		;Make sure we found something.
	jnz	l0		;Go look for more info.
	dec	OptionPointer
	dec	OptionCounter	;move pointer/counter back.
	xor	ebx,ebx
	mov	bl,OptionCounter	;Get file entry number.
	shl	ebx,2		;/
	add	ebx,offset OptionTable+128 ;/
	mov	w[ebx],0		;reset table entry.
	jmp	l0
	;
lOption:	mov	ah,es:[esi]		;Get switch character.
	inc	esi
l5:	cmp	es:b[esi],0		;check for end of line.
	jz	l9		;/
	cmp	es:b[esi],' '	;skip spaces.
	jnz	l6		;/
	inc	esi		;/
	jmp	l5		;/
	;
l6:	mov	al,es:[esi]		;get the switched character.
	and	al,127
	inc	esi
	cmp	al,61h		; 'a'
	jb	l12
	cmp	al,7Ah		; 'z'
	ja	l12
	and	al,5Fh		;convert to upper case.
l12:	xor	ebx,ebx
	mov	bl,al
	add	ebx,offset OptionTable	;Index into the table.
	cmp	ah,'-'
	jnz	l7
	xor	ah,ah		;Convert '-' to zero.
l7:	mov	[ebx],ah		;Set flag accordingly.
	;
	cmp	es:b[esi],' '	;check for assosiated text.
	jz	l0
	cmp	es:b[esi],0
	jz	l9
	cmp	es:b[esi],'='
	jz	l900
	cmp	es:b[esi],':'	;allow colon as seperator.
	jnz	l8
l900:	inc	esi		;skip colon.
	;
l8:	xor	ebx,ebx
	mov	bl,al		;Get the option number again.
	shl	ebx,2		; &
	add	ebx,offset OptionTable+128 ;index into the table.
	mov	edi,OptionPointer	;current position in the table.
	mov	[ebx],edi		;store pointer in the table.
	;
l10:	cmp	es:b[esi],0		;end of line?
	jz	l9
	cmp	es:b[esi],' '	;end of text?
	jz	l11
	mov	al,es:[esi]
	mov	[edi],al
	inc	esi
	inc	edi
	jmp	l10
	;
l11:	mov	b[edi],0		;terminate string.
	inc	edi
	mov	OptionPointer,edi	;store new text pointer.
	jmp	l0		;scan some more text.
l9:	pop	es
	popad
	movzx	eax,OptionCounter
	ret
ReadCommand	endp


	sdata

OptionCounter	db 0
OptionTable	db 128 dup (0)
OptionPointers	dd 128 dup (0)
OptionPointer	dd ?
OptionText	db 256 dup (0)


	efile
	end

