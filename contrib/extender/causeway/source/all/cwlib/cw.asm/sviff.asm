	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Encodes and writes a bitmap in IFF format.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - compress output.
;	1 - old format.
;EDX	- file name.
;ESI	- Bitmap to write.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file error.
;	2 - not enough memory (for buffers).
;	3 - format error.
;
;All other registers preserved.
;
SaveIFF	proc	near
	local @@PicWidth:dword, @@PicDepth:dword, @@BodyLow:word, @@BodyHigh:word, \
	@@flags:dword, @@bitmap:dword
	pushad
	;
	mov	@@flags,eax
	mov	@@bitmap,esi
	mov	d[IFF_ErrorStack],esp
	mov	d[IFF_Handle],0
	mov	_IFF_LineBuffer,0
	;
	call	CreateFile
	jc	@@file_error
	mov	d[IFF_Handle],ebx
	;
	;Get memory for line buffer.
	;
	mov	ecx,16384
	call	malloc
	jc	@@mem_error
	mov	_IFF_LineBuffer,esi
	;
	mov	esi,offset IFFDefault
	mov	edi,offset IFFHeader
	mov	ecx,48/4
	rep	movsd
;
;Set up variables ready for save.
;
	mov	esi,@@bitmap
	mov	eax,BM_Wide[esi]
	mov	@@PicWidth,eax
	xchg	ah,al
	mov	w[IFFXSize],ax
	mov	w[IFFDrawnX],ax
	mov	eax,BM_Depth[esi]
	mov	@@PicDepth,eax
	xchg	ah,al
	mov	w[IFFYSize],ax
	mov	w[IFFDrawnY],ax
	;
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mov	edx,eax
	mov	ecx,BM_Wide[esi]	;Set plane length
	mov	ax,1		;Set planes to compress.
	test	@@flags,2
	jz	@@noforceold0
	mov	ax,8
@@noforceold0:	mov	bl,8		;Number of colour planes.
	mov	bh,8
	cmp	BM_PWide[esi],1
	jz	@@GotPlanes
	mov	bh,16
	mov	ax,16
	cmp	BM_PWide[esi],2
	jz	@@GotPlanes
	mov	bh,24
	mov	ax,24
@@GotPlanes:	test	@@flags,2
	jnz	@@forceold0
	cmp	bh,8
	jnz	@@forceold0
	add	cx,15
	and	cx,not 15
	jmp	@@20
	;
@@forceold0:	mov	al,bh
	xor	ah,ah		;Set planes for compresion.
	mov	bl,bh		;Set planes for colour map.
	add	cx,15
	and	cx,not 15
	shr	cx,3		;make it byte sized.
	pushm	ax,cx
	mov	ax,cx
	mov	cl,bh
	xor	ch,ch
	mul	cx
	mov	dx,ax		;Set line length.
	popm	ax,cx
@@20:	mov	w[IFFCompPlanes],ax	;Set number of planes for compress.
	mov	b[IFFColourLen],bl	;Set planes for colour map.
	mov	w[IFFLineLength],cx	;Set plane line length
	mov	w[IFFTransLength],dx	;Set transfer length
	test	@@flags,2
	jnz	@@forceold1
	cmp	b[IFFColourLen],8	;256 colour?
	jnz	@@CreatIt
	mov	esi,offset NewIFFHeader
	mov	edi,offset IFFPlaneForm
	mov	ecx,8
	rep	movsb
@@forceold1:	;
@@CreatIt:	;Now write the header and pallet info
	;
	mov	ah,b[IFFColourLen]
	mov	b[IFFNumPlanes],ah	;set number of plains
	mov	bx,256*3
;	cmp	ah,8
;	jnz	@@NoPalette
	xchg	bl,bh
	mov	w[IFFCmapLen+2],bx	;set pallet length
	;
@@NoPalette:	;Set compresion flag.
	;
	test	@@flags,1
	jz	@@nosetcomp
	mov	b[IFFCompYN],1
	;
@@nosetcomp:	;write 48 byte header
	;
	mov	ebx,d[IFF_Handle]
	mov	ecx,48
	mov	edx,offset IFFHeader
	call	WriteFile
	jc	@@file_error
	;
	;now to create the pallet data
	;
	cmp	w[IFFCmapLen+2],0
	jz	@@NoPalette2
	;
	mov	esi,@@bitmap
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	add	eax,size BM
	add	esi,eax
	mov	edi,offset Palette
	mov	ecx,256*3
@@gp0:	lodsb
	mov	ah,al
	shl	al,2
	and	ah,1
	or	al,ah
	shl	ah,1
	or	al,ah
	stosb
	dec	ecx
	jnz	@@gp0
	;
	movzx	ecx,w[IFFCMapLen+2]
	xchg	ch,cl		;get colour map length
	mov	edx,offset Palette
	mov	ebx,d[IFF_Handle]
	call	WriteFile
	;
@@NoPalette2:	;Write body text and length
	;
	mov	edx,offset IFF_plength
	mov	ecx,8
	mov	ebx,d[IFF_Handle]
	call	WriteFile
	;
	;Get this offset and store it.
	;
	mov	ax,4201h		;offset from current position.
	mov	ebx,d[IFF_Handle]	;Handle for this file
	xor	cx,cx		;Offset of 0
	mov	dx,cx
	int	21h
	mov	@@BodyHigh,dx	;current file pointer offset!
	mov	@@BodyLow,ax		;/
;
;Now write the real data.
;
	test	@@flags,2
	jnz	@@ConvertIt
	cmp	b[IFFColourLen],8	;256 colour?
	jnz	@@ConvertIt
	;
	;\
	;Insert TINY image here if new format.
	;/
	;
@@ConvertIt:	;
	;\
	;Convert a complete screen here.
	;/
	;
	mov	w[IFF_YLoc],0
	;
@@0:	;Fetch this line from the work frame.
	;
	mov	esi,@@bitmap
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mov	ecx,eax
	movzx	ebx,w[IFF_YLoc]
	mul	ebx
	add	eax,size BM
	add	esi,eax
	mov	edi,_IFF_LineBuffer
	rep_movsb
	inc	w[IFF_YLoc]
	;
	cmp	b[IFFNumPlanes],16+1
	jc	@@NoSwitchBytes
	mov	esi,@@bitmap
	mov	ecx,BM_Wide[esi]
	mov	esi,_IFF_LineBuffer
@@swb0:	mov	al,[esi+0]
	xchg	al,[esi+2]
	mov	[esi+0],al
	add	esi,3
	dec	ecx
	jnz	@@swb0
@@NoSwitchBytes:
	;
	test	@@flags,2
	jnz	@@forceold2
	cmp	b[IFFColourLen],8	;256 colour?
	jz	@@DoCompress		;skip plane making.
	;
	;\
@@forceold2:	;Convert to number of planes required.
	;/
	;
	mov	esi,@@bitmap
	mov	eax,BM_PWide[esi]
	mov	_IFF_ByteWide,eax	;move to next plane
	mov	esi,_IFF_LineBuffer
	mov	edi,esi
	add	edi,8192
	movzx	ecx,w[IFFTransLength]
	xor	eax,eax
	push	edi
	rep_stosb		;clear the buffer first.
	pop	edi
	;
	mov	dl,1		;Set plane bit.
	mov	dh,128		;Set current bit.
	movzx	ecx,w[IFFCompPlanes]	;set number of planes to do.
@@8:	pushm	edi,esi,ecx
	mov	ecx,@@PicWidth	;Width to scan.
@@7:	test	b[esi],dl		;This plane set?
	jz	@@5
	or	b[edi],dh		;Set current bit
@@5:	add	esi,_IFF_ByteWide
	shr	dh,1		;next bit to check.
	or	dh,dh
	jnz	@@6
	mov	dh,128
	inc	edi
@@6:	dec	ecx
	jnz	@@7		;do all of this plane
	popm	edi,esi,ecx
	rol	dl,1		;next plane test
	jnc	@@noskip
	inc	esi
@@noskip:	movzx	eax,w[IFFLineLength]	;Get plane line length
	add	edi,eax
	mov	dh,128		;Set current bit.
	dec	ecx
	jnz	@@8
	mov	esi,_IFF_LineBuffer
	mov	edi,esi
	add	esi,8192
	movzx	ecx,w[IFFTransLength]
	rep_movsb		;copy it back into line buffer.
	;
@@DoCompress:	;Now compress the data!
	;
	mov	esi,_IFF_LineBuffer
	mov	edi,esi
	add	edi,8192
	cmp	b[IFFCompYN],0	;Compression turned on?
	jz	@@NoCompress
	movzx	ecx,w[IFFCompPlanes]	;Planes to do it for!
@@CompLoop:	pushm	esi,ecx
	movzx	ecx,w[IFFLineLength]	;width of line!
	call	IFFCompress		;Compress the line
	popm	esi,ecx
	movzx	eax,w[IFFLineLength]	;Next plane please
	add	esi,eax
	dec	ecx
	jnz	@@CompLoop		;Compress it!
	jmp	@@WriteData		;now write it!
@@NoCompress:	movzx	ecx,w[IFFTransLength]	;get file transfer length
	rep_movsb		;just copy the data
	;
@@WriteData:	;And put it in the file
	;
	mov	ecx,_IFF_LineBuffer
	add	ecx,8192
	sub	edi,ecx
	mov	ecx,edi		;Set data length
	mov	ebx,d[IFF_Handle]
	mov	edx,_IFF_LineBuffer
	add	edx,8192
	call	WriteFile
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	dec	@@PicDepth
	jnz	@@0
	;
	;Now its time to write a new header incase of compresion.
	;
	mov	ax,4202h		;update the header.
	xor	cx,cx		;offset from end!
	mov	dx,cx
	mov	ebx,d[IFF_Handle]
	int	21h		;get 32 bit file length!
	test	ax,1
	jz	@@199
	add	ax,1
	adc	dx,0
	pushm	ax,dx
	mov	ebx,d[IFF_Handle]
	mov	ax,4000h
	mov	cx,1
	int	21h
	popm	ax,dx
@@199:	sub	ax,8
	sbb	dx,0
	xchg	ah,al
	xchg	dh,dl		;put them right way round.
	mov	w[IFFFileLen],dx	;Set high word data length
	mov	w[IFFFileLen+2],ax	;Set low word data length.
	xchg	ah,al
	xchg	dh,dl		;put them right way round.
	add	ax,8
	adc	dx,0		;correct it again.
	sub	ax,@@BodyLow
	sbb	dx,@@BodyHigh	;remove body position
	xchg	ah,al
	xchg	dh,dl		;put them right way round.
	mov	w[IFFFileLen2],dx	;Set high word data length
	mov	w[IFFFileLen2+2],ax	;Set low word data length
	mov	ax,4200h		;move to file start again.
	xor	cx,cx
	mov	dx,cx
	mov	ebx,d[IFF_Handle]
	int	21h
	mov	ebx,d[IFF_Handle]	;write the header.
	mov	ecx,48
	mov	edx,offset IFFHeader
	call	WriteFile
	mov	ax,4200h		;move to body text
	mov	ebx,d[IFF_Handle]
	mov	cx,@@BodyHigh
	mov	dx,@@BodyLow
	sub	dx,8
	sbb	cx,0
	int	21h
	mov	edx,offset IFF_plength	;write new data length
	mov	ecx,8
	mov	ebx,d[IFF_Handle]
	mov	ah,40h
	int	21h
	;
	xor	ebx,ebx
	jmp	@@error
	;
@@mem_error:	mov	ebx,2
	jmp	@@error
	;
@@file_error:	mov	ebx,1
	jmp	@@error
	;
@@error:	xchg	ebx,d[IFF_Handle]
	or	ebx,ebx
	jz	@@closed
	mov	ah,3eh
	int	21h
@@closed:	xor	esi,esi
	xchg	esi,_IFF_LineBuffer
	or	esi,esi
	jz	@@freed
	call	Free
@@freed:	popad
	mov	eax,d[IFF_Handle]
	ret
SaveIFF	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
IFFCompress	proc	near
;
;Compress a line of data using LBM algorythm.
;
;On Entry:-
;
;ESI 	- Source data.
;EDI 	- Destination buffer.
;ECX    	- Line length to compress.
;
;On Exit:-
;
;EDI 	- Next available byte in the buffer.
;
	push	ebp
	mov	ah,0		;reset copy flag.
@@CompLoop:	or	ecx,ecx		;finished this line?
	jz	@@Finished		;/
	mov	al,[esi]		;Fetch a byte.
	cmp	ecx,1		;Last byte on the line?
	jz	@@CopyToFin		;Copy last byte.
	cmp	al,[esi+1]		;any repeat yet?
	jz	@@CountRep		;/
@@CountCopy:	mov	dl,-1		;reset the counter.
	mov	ebp,esi		;store position.
@@CntCop0:	cmp	ecx,1
	jz	@@CntCop1
	cmp	dl,127		;check the length.
	jz	@@CntCop1		;/
	mov	al,[esi]		;get current byte.
	cmp	al,[esi+1]		;repeated still?
	jz	@@CntCop1		;put in the output string.
	inc	esi		;next byte.
	inc	dl		;update counter.
	loop	@@CntCop0		;scan to line end.
@@CntCop1:	or	ah,ah		;Last action copy?
	jz	@@CntCop2		;no,do as normal.
	mov	al,dl		;get the count value
	inc	al		;get number of bytes to add.
	add	al,[ebx]		;add previouse length.
	cmp	al,127		;check its not too long.
	jnc	@@CntCop2		;do as normal if it is.
	mov	[ebx],al		;Set the new count value.
	mov	esi,ebp		;Get start of this copy
	push	ecx		;save width left to do
	movzx	ecx,dl		;Set up the loop counter
	inc	ecx		;/
	rep	movsb		;and shift it!
	pop	ecx		;get width remaining back
	jmp	@@CompLoop		;go back to main loop.
@@CntCop2:	mov	esi,ebp		;point at the data again.
	mov	ebx,edi		;store command byte position.
	mov	[edi],dl		;store copy count value.
	inc	edi		;/
	push	ecx		;save length remaining.
	movzx	ecx,dl		;get count value.
	inc	ecx		;/
	rep	movsb		;copy it!
	pop	ecx		;get remaining width back
	mov	ah,1		;Set copy flag.
	jmp	@@CompLoop		;back to main loop.
@@CountRep:	mov	dl,-1		;reset the counter
@@CntRep0:	cmp	dl,127		;maximum count?
	jz	@@CntRep1		;do it.
	cmp	al,[esi]		;still repeating?
	jnz	@@CntRep1		;dump this value.
	inc	esi		;next byte.
	inc	dl		;update counter.
	loop	@@CntRep0		;scan to line end.
@@CntRep1:	cmp	dl,2		;short repeat?
	jnc	@@CntRep2		;no, do as normal.
	or	ah,ah		;did a copy last?
	jz	@@CntRep2		;no, do as normal.
	cmp	b[ebx],126		;enough space for this?
	jnc	@@CntRep2		;no, do as normal.
	add	b[ebx],2		;set new length
	stosb			;Store this byte twice.
	stosb			;/
	jmp	@@CompLoop		;Go back to main loop!
@@CntRep2:	neg	dl		;make it a command byte.
	mov	[edi],dl		;repeat count
	inc	edi		;/
	stosb			;repeat value.
	mov	ah,0		;reset copy flag.
	jmp	@@CompLoop		;back to main loop
@@CopyToFin:	or	ah,ah		;did copy last?
	jz	@@CopyToFin0		;/
	cmp	b[ebx],127		;enough space for this one?
	jnc	@@CopyToFin0		;/
	inc	b[ebx]		;update count value.
	stosb			;Store this byte.
	jmp	@@Finished		;all over!
@@CopyToFin0:	mov	b[edi],0		;set command byte.
	inc	edi		;/
	stosb			;Store byte to copy.
@@Finished:	pop	ebp
	ret
IFFCompress	endp


	sdata
;
IFF_BodyText	db 'BODY',0
OldIFFHeader	db 'ILBMBMHD'
NewIFFHeader	db 'PBM BMHD'
;
IFFDefault	db 'FORM'
	dw 0,607dh
	db 'ILBMBMHD',0,0,0,14h
	dw 4001h
	dw 0c800h
	db 0,0,0
	db 0
	db 4
	db 0
	db 0,0,0,0ffh,5,6
	dw 4001h
	dw 0c800h
	db 'CMAP',0,0
	dw 300h
IFF_PLength	db 'BODY'
IFFFileLen2	dw 0,007dh
;
IFF_ErrorStack	dd ?
IFF_Handle	dd ?
;
IFFHeader	db 'FORM'
IFFFileLen	dw ?		;File length -4
IFFDont0	dw ?
IFFPlaneForm	db 'ILBMBMHD'	;or PBM BMHD
IFFCmapOff	dw ?		;offset to CMAP text
IFFDont1	dw ?
IFFXSize	dw ?		;X size of data
IFFYSize	dw ?		;Y size of data
IFFDont2	dd ?
IFFNumPlanes	db ?		;Number of colour planes
IFFBrushYN	db ?		;0 for picture,1 for with stencil,2 for brush?
IFFCompYN	db ?		;0 for Camera, 1 for compresion?
IFFDont3	db ?
IFFDont4	dd ?
IFFDrawnX	dw ?		;X resolution drawn in ?
IFFDrawnY	dw ?		;Y resolution drawn in?
IFFCmapTxt	db 'CMAP'
IFFCmapLen	dd ?		;Length of colour data
IFFDataLen	dd ?
;
IFF_ByteWide	dw ?
IFF_ByteDeep	dw ?
IFF_YLoc	dw ?
IFFCompPlanes	dw ?
IFFColourLen	db ?
IFFLineLength	dw ?
IFFTransLength	dw ?
_IFF_ByteWide	dd ?
_IFF_LineBuffer dd ?
;
Palette	db 256*3 dup (?)
;
	efile
	end

