;
;/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
;
;CauseWay real mode startup program. Interogates target program for setup flags
;and writes to real debugger file before passing control to the real debugger.
;
	.model small
	.stack 256
	.386
	option oldstructs


	include equates.asm
	include macros.asm
	include ..\strucs.inc


	.code


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Start	proc	near
	mov	ax,_data		;make data addresable.
	mov	ds,ax
	mov	PspSeg,es
	;
	mov	ax,es
	mov	cx,_endprog		;Get program end segment.
	sub	cx,ax		;Size program.
	mov	bx,cx		;/
	inc	bx		;/
	mov	ah,4ah		;Re-size memory block function.
	int	21h		;/
	;
	;Get an execution path for the real program.
	;
	mov	ah,51h		;Get PSP of currently executing
	int	21h		;program...
	mov	es,bx		;/
	mov	es,es:[2ch]		;Get enviroment string address.
	xor	si,si		;Point at the enviroment string.
@@10a:	mov	al,es:[si]		;Get a byte.
	inc	si		;/
	or	al,al		;End of a string?
	jnz	@@10a		;keep looking.
	mov	al,es:[si]		;Double zero?
	or	al,al		;/
	jnz	@@10a		;keep looking.
	add	si,3		;Skip last 0 and word count.
	pushm	ds,ds,es		;make list addressable.
	popm	es,ds		;/
	cld			;/
	lea	di,DebugPath		;Where we're gonna put it.
@@70:	movsb
	cmp	b[si-1],0		;copy till end.
	jnz	@@70
	pop	ds
	;
	;Put debug name on the end of the path.
	;
	mov	si,offset DebugPath
	mov	di,si
	xor	ah,ah
@@100:	lodsb
	or	al,al
	jz	@@101
	cmp	al,'.'
	jnz	@@100
	mov	di,si
	dec	di
	jmp	@@100
@@101:	mov	b[di],"."
	inc	di
	mov	si,offset DebugExten
	mov	cx,4
	push	ds
	pop	es
	cld
	rep	movsb
	;
	call	ReadCommand		;get target file.
	or	ax,ax
	jz	@@Exec		;skip if no name.
	;
	;Make .EXE file name.
	;
	mov	si,w[OptionTable+128]	;get file name mask.
	mov	di,offset EXEFileName
	cld
	push	ds
	pop	es
	xor	al,al
@@e0:	movsb
	cmp	b[si-1],'.'
	jnz	@@e1
	cmp	b[si],'.'
	jnz	@@e5
	movsb
	jmp	@@e0
@@e5:	mov	al,1
@@e1:	cmp	b[si-1],0
	jnz	@@e0
	or	al,al
	jnz	@@e2
	mov	b[di-1],'.'
	mov	si,offset EXEextension
@@e4:	movsb
	cmp	b[si-1],0
	jnz	@@e4
	;
@@e2:	mov	Handle,0
	mov	dx,offset EXEFileName	;get name.
	mov	ax,3d00h		;open, read only.
	int	21h
	jc	@@03
	mov	Handle,ax		;store the handle.
	mov	bx,ax
	mov	dx,offset ExeSignature	;somewhere to put the info.
	mov	cx,1bh		;size of it.
	mov	ah,3fh
	int	21h
	jc	@@03
	cmp	ax,1bh		;did we read right amount?
	jnz	@@03
	cmp	w[ExeSignature],'ZM'	;Normal EXE?
	jnz	@@03
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
	mov	cx,ax
	xchg	cx,dx		;swap round for DOS.
	mov	ax,4200h		;set absolute position.
	int	21h
	mov	bx,Handle
	mov	dx,offset NewHeader	;somewhere to put the info.
	mov	cx,size NewHeaderStruc	;size of it.
	mov	ah,3fh
	int	21h
	jc	@@03
	cmp	ax,size NewHeaderStruc	;did we read right amount?
	jnz	@@03
	cmp	w[NewHeader],'P3'	;ID ok?
	jnz	@@03
	mov	si,offset NewHeader
	mov	eax,NewFlags[si]
	mov	TargetFlags,ax
@@03:	mov	bx,Handle
	or	bx,bx
	jz	@@04
	mov	ax,3e00h
	int	21h
@@04:	;
	mov	Handle,0
	mov	dx,offset debugpath
	mov	ax,3d02h		;open, read only.
	int	21h
	jc	@@13
	mov	Handle,ax		;store the handle.
	mov	bx,ax
	mov	dx,offset ExeSignature	;somewhere to put the info.
	mov	cx,1bh		;size of it.
	mov	ah,3fh
	int	21h
	jc	@@13
	cmp	ax,1bh		;did we read right amount?
	jnz	@@13
	cmp	w[ExeSignature],'ZM'	;Normal EXE?
	jnz	@@13
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
	xchg	cx,dx		;swap round for DOS.
	mov	ax,4200h		;set absolute position.
	int	21h
	mov	bx,Handle
	mov	dx,offset NewHeader	;somewhere to put the info.
	mov	cx,size NewHeaderStruc	;size of it.
	mov	ah,3fh
	int	21h
	jc	@@13
	cmp	ax,size NewHeaderStruc	;did we read right amount?
	jnz	@@13
	cmp	w[NewHeader],'P3'	;ID ok?
	jnz	@@13
	;
	mov	si,offset NewHeader
	mov	ax,TargetFlags
	and	ax,0ffffh-16384
	and	w[NewFlags+si],16384
	or	w[NewFlags+si],ax	;set target flags.
	;
	mov	bx,Handle
	mov	dx,-(size NewHeaderStruc)
	mov	cx,-1
	mov	ax,4201h
	int	21h		;move back to start of the header.
	mov	bx,Handle
	mov	dx,offset NewHeader	;somewhere to put the info.
	mov	cx,size NewHeaderStruc		;size of it.
	mov	ah,40h
	int	21h		;read the header again.
	jc	@@13
	;
@@13:	mov	bx,Handle
	or	bx,bx
	jz	@@14
	mov	ax,3e00h
	int	21h
@@14:	;
@@Exec:	;check if CWD.OVL can be found.
	;
	mov	dx,offset debugpath
	mov	ax,3d00h
	int	21h
	jnc	@@pathok
	mov	dx,offset NoOverlayText
	mov	ah,9
	int	21h
	mov	si,offset debugpath
@@findend:	lodsb
	or	al,al
	jnz	@@findend
	mov	b[si-1],13
	mov	b[si],10
	mov	b[si+1],'$'
	mov	dx,offset debugpath
	mov	ah,9
	int	21h
	jmp	@@exit
	;
@@pathok:	mov	bx,ax
	mov	ah,3eh		;close it again.
	int	21h
	;
	mov	ax,PspSeg
	mov	w[ExecTable+4],ax
	mov	w[ExecTable+8],ax
	mov	w[ExecTable+12],ax
	;
	mov	ax,4b00h
	mov	dx,offset debugpath
	mov	bx,offset ExecTable
	push	ds
	pop	es
	int	21h
	;
@@exit:	mov	ax,4c00h
	int	21h
@@Handle	dw 0
Start	endp


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
	mov	es,PspSeg		;point at the tail data.
	mov	si,80h		;/
	mov	cl,es:[si]		;get tail length
	xor	ch,ch
	cmp	cl,2
	jc	@@9		;not long enough!
	mov	di,si
	add	di,cx
	mov	es:b[di+1],0		;terminate the tail.
	inc	si		;skip length.
	;
@@0:	mov	al,es:[si]		;need to skip leading spaces.
	inc	si		;/
	or	al,al		;/
	jz	@@9		;/
	cmp	al,' '		;/
	jz	@@0		;/
	dec	si		;/
	;
@@1:	cmp	es:b[si],'/'		;option switch?
	jz	@@Option		;/
	cmp	es:b[si],'-'		;/
	jz	@@Option		;/
	cmp	es:b[si],'+'		;/
	jz	@@Option		;/
	;
@@2:	mov	bl,OptionCounter	;Get file entry number.
	inc	OptionCounter	;/
	xor	bh,bh		;/
	shl	bx,1		;/
	add	bx,offset OptionTable+128	;/
	mov	di,OptionPointer	;Current free space pointer.
	mov	[bx],di		;update table entry.
	;
	xor	cl,cl
@@3:	cmp	es:b[si],0		;end of name?
	jz	@@4		;/
	cmp	es:b[si],' '		;/
	jz	@@4		;/
	mov	al,es:[si]		;Copy this character.
	mov	[di],al		;/
	inc	si		;/
	inc	di		;/
	mov	cl,1		;flag SOMETHING found.
	jmp	@@3		;keep fetching them.
	;
@@4:	mov	b[di],0		;Terminate the name.
	inc	di		;/
	mov	OptionPointer,di	;Update table pointer.
	;
	or	cl,cl		;Make sure we found something.
	jnz	@@0		;Go look for more info.
	dec	OptionPointer
	dec	OptionCounter	;move pointer/counter back.
	mov	bl,OptionCounter	;Get file entry number.
	xor	bh,bh		;/
	shl	bx,1		;/
	add	bx,offset OptionTable+128	;/
	mov	w[bx],0		;reset table entry.
	jmp	@@0
	;
@@Option:	mov	ah,es:[si]		;Get switch character.
	inc	si
@@5:	cmp	es:b[si],0		;check for end of line.
	jz	@@9		;/
	cmp	es:b[si],' '		;skip spaces.
	jnz	@@6		;/
	inc	si		;/
	jmp	@@5		;/
	;
@@6:	mov	al,es:[si]		;get the switched character.
	and	al,127
	inc	si
	cmp	al,61h		; 'a'
	jb	@@12
	cmp	al,7Ah		; 'z'
	ja	@@12
	and	al,5Fh		;convert to upper case.
@@12:	mov	bl,al
	xor	bh,bh
	add	bx,offset OptionTable	;Index into the table.
	cmp	ah,'-'
	jnz	@@7
	xor	ah,ah		;Convert '-' to zero.
@@7:	mov	[bx],ah		;Set flag accordingly.
	;
	cmp	es:b[si],' '		;check for assosiated text.
	jz	@@0
	cmp	es:b[si],0
	jz	@@9
	cmp	es:b[si],'='
	jz	@@900
	cmp	es:b[si],':'		;allow colon as seperator.
	jnz	@@8
@@900:	inc	si		;skip colon.
	;
@@8:	mov	bl,al		;Get the option number again.
	xor	bh,bh		;/
	shl	bx,1		; &
	add	bx,offset OptionTable+128	;index into the table.
	mov	di,OptionPointer	;current position in the table.
	mov	[bx],di		;store pointer in the table.
	;
@@10:	cmp	es:b[si],0		;end of line?
	jz	@@9
	cmp	es:b[si],' '		;end of text?
	jz	@@11
	mov	al,es:[si]
	mov	[di],al
	inc	si
	inc	di
	jmp	@@10
	;
@@11:	mov	b[di],0		;terminate string.
	inc	di
	mov	OptionPointer,di	;store new text pointer.
	jmp	@@0		;scan some more text.
@@9:	mov	al,OptionCounter
	xor	ah,ah
	or	ax,ax		;set flags for file names.
	mov	bx,w[OptionTable+128]	;point to first file name.
	ret
ReadCommand	endp


	.data


PspSeg	dw ?
;
NoOverlayText	db 13,10,'Could not find: $'
;
OptionCounter	db 0
OptionTable	db 128 dup (0)
	dw 128 dup (0)
OptionPointer	dw OptionText
OptionText	db 256 dup (0)
;
TargetFlags	dw 0
;
DebugPath	db 128 dup (0)
DebugExten	db 'ovl',0
;
ExecTable	dw 0
	dw 80h,?	;command line.
	dw 5ch,?	;fcb1
	dw 6ch,?	;fcb2
;
EXEextension	db 'EXE',0
ExeFileName	db 128 dup (?)
;
NewHeader	NewHeaderStruc <>
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
;
Handle	dw 0
;
_Data	ends


_endprog	segment para
_endprog	ends


	end start
