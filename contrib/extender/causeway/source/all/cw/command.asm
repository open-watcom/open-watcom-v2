;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ReadCommand	proc	near
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
;To test flag:- cmp OptionTable+'?',0	;where ? is upper case character.
;To test text:- cmp w[offset OptionTable+128+('?'*2)],0
;To get text:-  mov bx,w[offset OptionTable+128+('?'*2)]
;
;To get first file name,  mov bx,w[offset OptionTable+128+0]
;       second            mov bx,w[offset OptionTable+128+2]
;
;The text gatherer will also accept : or = after the switch, eg,
;
;	Chop-Rob /aEJW1 test.pak
;	or
;	Chop-Rob /a:EJW1 test.pak
;	or
;	Chop-Rob /a=EJW1 test.pak
;
;Switches & file names may be in any position. Duplicate switch settings with
;text will store multiple text, but only the last will be addressable.
;
	push	es
	sys	Info		;Get system selectors.
	mov	es,bx
	mov	edi,offset OptionText
	mov	OptionPointer,edi
	mov	esi,80h		;/
	xor	ecx,ecx
	mov	cl,es:[esi]		;get tail length
	cmp	cl,2
	jc	@@9		;not long enough!
	mov	edi,esi
	add	edi,ecx
	mov	es:b[edi+1],0	;terminate the tail.
	inc	si		;skip length.
	;
@@0:	mov	al,es:[esi]		;need to skip leading spaces.
	inc	si		;/
	or	al,al		;/
	jz	@@9		;/
	cmp	al,' '		;/
	jz	@@0		;/
	dec	si		;/
	;
@@1:	cmp	es:b[esi],'/'	;option switch?
	jz	@@Option		;/
	cmp	es:b[esi],'-'	;/
	jz	@@Option		;/
	cmp	es:b[esi],'+'	;/
	jz	@@Option		;/
	;
@@2:	xor	ebx,ebx		;/
	mov	bl,OptionCounter	;Get file entry number.
	inc	OptionCounter	;/
	shl	ebx,2		;/
	add	ebx,offset OptionTable+128 ;/
	mov	edi,OptionPointer	;Current free space pointer.
	mov	[ebx],edi		;update table entry.
	;
	xor	cl,cl
@@3:	cmp	es:b[esi],0		;end of name?
	jz	@@4		;/
	cmp	es:b[esi],' '	;/
	jz	@@4		;/
	mov	al,es:[esi]		;Copy this character.
	mov	[edi],al		;/
	inc	esi		;/
	inc	edi		;/
	mov	cl,1		;flag SOMETHING found.
	jmp	@@3		;keep fetching them.
	;
@@4:	mov	b[edi],0		;Terminate the name.
	inc	edi		;/
	mov	OptionPointer,edi	;Update table pointer.
	;
	or	cl,cl		;Make sure we found something.
	jnz	@@0		;Go look for more info.
	dec	OptionPointer
	dec	OptionCounter	;move pointer/counter back.
	xor	ebx,ebx
	mov	bl,OptionCounter	;Get file entry number.
	shl	ebx,2		;/
	add	ebx,offset OptionTable+128 ;/
	mov	w[ebx],0		;reset table entry.
	jmp	@@0
	;
@@Option:	mov	ah,es:[esi]		;Get switch character.
	inc	esi
@@5:	cmp	es:b[esi],0		;check for end of line.
	jz	@@9		;/
	cmp	es:b[esi],' '	;skip spaces.
	jnz	@@6		;/
	inc	esi		;/
	jmp	@@5		;/
	;
@@6:	mov	al,es:[esi]		;get the switched character.
	and	al,127
	inc	esi
	cmp	al,61h		; 'a'
	jb	@@12
	cmp	al,7Ah		; 'z'
	ja	@@12
	and	al,5Fh		;convert to upper case.
@@12:	xor	ebx,ebx
	mov	bl,al
	add	ebx,offset OptionTable	;Index into the table.
	cmp	ah,'-'
	jnz	@@7
	xor	ah,ah		;Convert '-' to zero.
@@7:	mov	[ebx],ah		;Set flag accordingly.
	;
	cmp	es:b[esi],' '	;check for assosiated text.
	jz	@@0
	cmp	es:b[esi],0
	jz	@@9
	cmp	es:b[esi],'='
	jz	@@900
	cmp	es:b[esi],':'	;allow colon as seperator.
	jnz	@@8
@@900:	inc	esi		;skip colon.
	;
@@8:	xor	ebx,ebx
	mov	bl,al		;Get the option number again.
	shl	ebx,2		; &
	add	ebx,offset OptionTable+128 ;index into the table.
	mov	edi,OptionPointer	;current position in the table.
	mov	[ebx],edi		;store pointer in the table.
	;
@@10:	cmp	es:b[esi],0		;end of line?
	jz	@@9
	cmp	es:b[esi],' '	;end of text?
	jz	@@11
	mov	al,es:[esi]
	mov	[edi],al
	inc	esi
	inc	edi
	jmp	@@10
	;
@@11:	mov	b[edi],0		;terminate string.
	inc	edi
	mov	OptionPointer,edi	;store new text pointer.
	jmp	@@0		;scan some more text.
@@9:	mov	al,OptionCounter
	xor	ah,ah
	or	ax,ax		;set flags for file names.
	mov	ebx,d[OptionTable+128]	;point to first file name.
	pop	es
	ret
;
OptionCounter	db 0
OptionTable	db 128 dup (0)
	dd 128 dup (0)
OptionPointer	dd ?
OptionText	db 256 dup (0)
ReadCommand	endp


