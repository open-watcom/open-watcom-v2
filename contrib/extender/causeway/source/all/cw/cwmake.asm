	.model small
	.stack 400h
	.386
	option oldstructs

	include general.inc
	include ..\strucs.inc

	.code

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Start	proc	near
	mov	ax,DGROUP
	mov	ds,ax
	mov	PSPSegment,es
	;
	mov	cx,sp
	shr	cx,4
	inc	cx
	mov	ax,es
	mov	bx,ss		;Get program end segment.
	add	bx,cx
	sub	bx,ax		;Size program.
	mov	ah,4ah
	int	21h		;Re-size memory block.
	;
	call	ProcessFile
	;
	mov	ax,4c00h
	int	21h
Start	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
ProcessFile	proc	near
	mov	dx,offset InFileName
	mov	ax,3d00h
	int	21h
	jc	@@9
	mov	InFileHandle,ax
	;
	mov	dx,offset OutFileName
	mov	ax,3c00h
	mov	cx,0
	int	21h
	jc	@@9
	mov	OutFileHandle,ax
	;
	;Process .EXE relocation table and produce new format header.
	;
	mov	dx,offset ExeSignature
	mov	cx,1bh
	mov	bx,InFileHandle
	mov	ah,3fh
	int	21h		;read the .EXE header.
	jc	@@9
	cmp	ax,1bh
	jnz	@@9
	cmp	w[ExeSignature],'ZM'	;Correct ID?
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
	mov	w[ExeSize1],bx	;/
	mov	w[ExeSize1+2],cx	;/
	;
	;Get exe image size in bytes.
	;
	mov	ax,w[ExeLength+2]	;get length in 512 byte blocks
	dec	ax		;lose 1 cos its rounded up
	add	ax,ax		;mult by 2
	mov	dh,0
	mov	dl,ah
	mov	ah,al
	mov	al,dh		;mult by 256=*512
	add	ax,w[ExeLength]	;add length mod 512
	adc	dx,0		;add any carry to dx
	sub	ax,bx		;remove header total size.
	sbb	dx,cx		;/
	mov	w[ExeSize2],ax	;/
	mov	w[ExeSize2+2],dx	;/
	;
	shl	edx,16
	mov	dx,ax
	mov	si,offset NewHeader
	mov	NewLength[si],edx	;store EXE image length in the header.
	;
	push	edx
	cmp	edx,100000h
	jc	@@0
	add	edx,4095
	shr	edx,12		;lose bottom bits.
	or	edx,1 shl 20		;Set our version of the G bit.
@@0:	mov	CodeSegLimit,edx
	pop	edx
	;
	add	edx,1024
	mov	NewAlloc[si],edx
	and	edx,0ffffffffh-3
	mov	NewEntryESP[si],edx
	mov	NewSegments[si],2
	movzx	eax,ExeRelocNum
	mov	NewRelocs[si],eax
	movzx	eax,ExeEntryIP
	mov	NewEntryEIP[si],eax	;setup entry offset.
	mov	NewEntryCS[si],1
	mov	NewEntrySS[si],0
	;
	;Read relocation table.
	;
	mov	bx,ExeRelocNum	;get number of relocation items.
	or	bx,bx
	jz	@@NoRelocMem
	shl	bx,2
	shr	bx,4
	inc	bx
	mov	ah,48h
	int	21h
	jc	@@9
	mov	RelocSegment,ax	;stow the memory address.
	;
	mov	dx,ExeRelocFirst
	mov	cx,0
	mov	bx,InFileHandle
	mov	ax,4200h
	int	21h		;move to relocation table.
	;
	mov	cx,ExeRelocNum
	shl	cx,2
	mov	bx,InFileHandle
	push	ds
	mov	ds,RelocSegment
	mov	dx,0
	mov	ax,3f00h
	int	21h
	pop	ds
	jc	@@9
	;
@@NoRelocMem:	;Get some memory for the exe image.
	;
	mov	ebx,d[ExeSize2]	;get exe image size.
	shr	ebx,4
	inc	bx
	mov	ah,48h
	int	21h
	jc	@@9
	mov	EXESegment,ax
	;
	mov	dx,w[ExeSize1]	;get image file offset.
	mov	cx,w[ExeSize1+2]
	mov	bx,InFileHandle
	mov	ax,4200h
	int	21h		;move to start of exe image.
	jc	@@9
	mov	bx,InFileHandle
	mov	cx,w[ExeSize2]
	mov	dx,w[ExeSize2+2]
	push	ds
	mov	ds,EXESegment
	mov	dx,0
	mov	ah,3fh
	int	21h
	pop	ds
	jc	@@9
	;
	mov	bx,OutFileHandle
	mov	cx,0
	mov	dx,0
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	mov	Real3POffset,edx
	;
	;Update exe image with real segment numbers.
	;
	mov	cx,ExeRelocNum	;number of entries.
	jcxz	@@NoReloc
	mov	es,RelocSegment	;the relocations.
	mov	si,0
@@3:	mov	di,es:[si]
	mov	ax,es:[si+2]
	add	ax,EXESegment
	mov	fs,ax
	mov	fs:w[di],0
	add	si,4		;next relocation entry.
	loop	@@3
	;
	;Convert relocation table to linear offsets.
	;
	mov	cx,ExeRelocNum	;number of entries.
	mov	es,RelocSegment	;list of relocations.
	mov	si,0
@@1:	movzx	eax,es:w[si+2]	;get segment offset.
	shl	eax,4		;make it linear.
	movzx	ebx,es:w[si+0]	;get offset.
	add	eax,ebx		;add in offset.
	mov	es:d[si],eax		;store linear offset.
	add	si,4
	loop	@@1
@@NoReloc:	;
	;Write main header.
	;
	mov	dx,offset NewHeader	;write the header to make space.
	mov	bx,OutFileHandle
	mov	cx,size NewHeaderStruc
	mov	ah,40h
	int	21h
	;
	;Write segment definitions.
	;
	mov	dx,offset DataSegBase
	mov	cx,8+8
	mov	bx,OutFileHandle
	mov	ah,40h
	int	21h
	;
	;Write relocation table.
	;
	mov	cx,ExeRelocNum	;number of entries.
	shl	cx,2
	mov	bx,OutFileHandle
	push	ds
	mov	ds,RelocSegment	;where they are.
	mov	dx,0
	mov	ah,40h
	int	21h
	pop	ds
	jc	@@9
	;
	;Write exe image.
	;
	mov	cx,w[ExeSize2]
	mov	dx,w[ExeSize2+2]
	mov	bx,OutFileHandle
	push	ds
	mov	ds,ExeSegment
	mov	dx,0
	mov	ah,40h
	int	21h
	pop	ds
	jc	@@9
	;
	;Calculate file size.
	;
	mov	bx,OutFileHandle
	mov	cx,0
	mov	dx,0
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,Real3POffset
	mov	si,offset NewHeader
	mov	NewSize[si],edx
	;
	;Now go back and write the real header.
	;
	mov	edx,Real3POffset
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	bx,OutFileHandle
	mov	ax,4200h
	int	21h
	mov	dx,offset NewHeader	;write the header to make space.
	mov	bx,OutFileHandle
	mov	cx,size NewHeaderStruc
	mov	ah,40h
	int	21h
	;
	mov	bx,InFileHandle
	mov	ah,3eh
	int	21h
	mov	bx,OutFileHandle
	mov	ah,3eh
	int	21h
	;
@@9:	ret
ProcessFile	endp


	.data
;
PSPSegment	dw ?
;
InFileName	db 'cw.exe',0
InFileHandle	dw ?
OutFileName	db 'cw.cw',0
OutFileHandle	dw ?
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
ExeSize1	dw ?,?	;Header size
ExeSize2	dw ?,?	;Real file size.
;
NewHeader	NewHeaderStruc <>	;make space for a header.
;
DataSegBase	dd 0
DataSegLimit	dd 0fffffh+(1 shl 20)+(1 shl 21)
CodeSegBase	dd 0
CodeSegLimit	dd ?
;
RelocSegment	dw 0
EXESegment	dw 0
Real3POffset	dd 0
;
	end	Start
