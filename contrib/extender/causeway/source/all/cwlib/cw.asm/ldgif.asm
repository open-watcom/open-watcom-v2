	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Loads and unpacks a gif file.
;
;On Entry:
;
;EDX	- File name.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- pointer to bitmap.
;
LoadGif	proc	near
	pushad
	mov	edi,offset Gd_StartInit	;Initialise the data.
	mov	ecx,offset Gd_EndInit-Gd_StartInit	;/
	cld			;/
	xor	al,al		;/
	rep_stosb		;/
	mov	Gd_ErrorStack,esp	;setup error stack point.
	;
	call	OpenFile
	jc	@@file_error
	mov	Gd_Handle,bx
	call	InitFileBuffer	;initialise file buffering.
	;
	mov	eax,8192+4096+4096+4096+8192
	mov	ecx,eax
	call	malloc
	jc	@@mem_error
	mov	edi,esi
	mov	Gd_OutRow,esi
	add	esi,8192
	mov	Gd_OStack,esi
	add	esi,4096
	mov	Gd_CtFirst,esi
	add	esi,4096
	mov	Gd_CtLast,esi
	add	esi,4096
	mov	Gd_CtLink,esi
	xor	esi,eax
	rep_stosb
	;
	call	@@FGetc		;now check the file starts with
	cmp	al,'G'		;Gif
	jnz	@@form_error		;/
	call	@@FGetc		;/
	cmp	al,'I'		;/
	jnz	@@form_error		;/
	call	@@FGetc		;/
	cmp	al,'F'		;/
	jnz	@@form_error		;/
@@upg2:	mov	cx,3
@@upg3:	push	cx		;skip the version for now.
	call	@@FGetc		;/
	pop	cx		;/
	dec	cx
	jnz	@@upg3		;/
	call	@@FGetw		;fetch origional screen dimensions.
	mov	w[Gd_ScreenWidth],ax	;/
	call	@@FGetw		;/
	mov	w[Gd_ScreenHeight],ax	;/
	call	@@FGetc
	mov	w[Gd_UpGd_sflags],ax	;get global flags.
	and	ax,7		;/
	inc	ax		;/
	mov	w[Gd_UpGd_gpix],ax	;set global pixel size.
	call	@@FGetc
	mov	w[Gd_BackGround],ax	;set background colour.
	call	@@FGetC
	cmp	al,0		;next byte is allways 0.
	jnz	@@form_error
	test	w[Gd_UpGd_sflags],80h	;check if there's a global colour
	jz	@@upg5		;map. (palette)
	mov	cx,w[Gd_UpGd_gpix]	;global palette size.
	call	@@GetColours		;fetch the palette.
	;
@@upg5:	;the main loop. Images/Extensions are discovered here.
	;
@@upg6:	call	@@FGetc		;get block type.
	mov	w[Gd_UpGd_work],ax
	cmp	ax,-1		;bad data?
	jne	@@upg8
	jmp	@@form_error
@@upg8:	cmp	al,','		;an image?
	jnz	@@upg11		;try an extension
	call	@@FGetw		;get page start x.
	mov	w[Gd_ImageX],ax	;/
	call	@@FGetw		;get page start y.
	mov	w[Gd_ImageY],ax	;/
	call	@@FGetw		;get page width.
	mov	w[Gd_ImageWide],ax	;/
	call	@@FGetw		;get page depth.
	mov	w[Gd_ImageDeep],ax	;/
	call	@@FGetc		;get local flags.
	mov	w[Gd_UpGd_Iflags],ax	;/
	and	ax,64		;Set the interlaced flag.
	mov	w[Gd_Interlaced],ax	;/
	;
	;NOW the page size should be set and a bitmap created.
	;
	movzx	ecx,w[GD_ImageWide]
	movzx	edx,w[GD_ImageDeep]
	xor	ebx,ebx
	call	CreateBitmap
	jc	@@mem_error
	mov	_GIF_Bitmap,eax
	mov	esi,eax
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	add	eax,size BM
	add	eax,esi
	mov	edi,eax
	mov	esi,offset SystemPalette
	mov	ecx,256*3
	rep	movsb
	;
	mov	ax,w[Gd_upGd_gpix]	;default to global pixel size.
	mov	w[Gd_upGd_pixelsize],ax	;/
	test	w[Gd_upGd_iflags],80h	;local palette?
	jz	@@upg10
	mov	ax,w[Gd_upGd_iflags]	;get the local palette.
	and	ax,7		;/
	inc	ax		;/
	mov	w[Gd_upGd_pixelsize],ax	;/
	mov	cx,ax		;/
	push	cx		;/
	call	@@GetColours		;/
	pop	cx		;/
@@upg10:	mov	w[Gd_BufCt],0		;start decoding the data.
	call	@@FGetc		;/
	mov	cx,ax		;/
	mov	dx,w[Gd_upGd_pixelsize]	;/
	call	@@UnpackImage		;/
	jmp	@@upg6		;/
@@upg11:	cmp	al,'!'		;an extension?
	jnz	@@upg16
	call	@@FGetc		;?
@@upg13:	call	@@FGetc		;get block length?
	mov	cx,ax
	cmp	ax,0		;end of block?
	je	@@upg15		;/
@@upg14:	push	cx
	call	@@FGetc		;read block.
	pop	cx		;/
	dec	cx
	jnz	@@upg14		;/
	jmp	@@upg13		;get next block.
@@upg15:	jmp	@@upg6		;back to main loop point.
@@upg16:	cmp	al,3bh		; ';' end of stream?
	jnz	@@form_error
	xor	ebx,ebx
	jmp	@@exit
	;
@@file_error:	mov	ebx,1
	jmp	@@exit
@@mem_error:	mov	ebx,2
	jmp	@@exit
@@form_error:	mov	ebx,3
	jmp	@@exit
	;
@@exit:	mov	esp,Gd_ErrorStack
	xchg	bx,Gd_Handle
	or	bx,bx
	jz	@@closed
	call	CloseFile
@@closed:	xor	eax,eax
	xchg	eax,Gd_OutRow
	or	eax,eax
	jz	@@released0
	mov	esi,eax
	call	Free
@@released0:	movzx	eax,Gd_Handle
	mov	esi,_GIF_Bitmap
	or	eax,eax
	jz	@@nobrel
	call	Free
	mov	_GIF_Bitmap,0
	jmp	@@nopal
@@nobrel:	mov	eax,BM_Wide[esi]
	mul	BM_Depth[esi]
	add	eax,size BM
	add	esi,eax
	mov	edi,offset Palette
	xchg	esi,edi
	mov	ecx,256*3
	rep_movsb
@@nopal:	popad
	movzx	eax,Gd_Handle
	mov	esi,_GIF_Bitmap
	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@UnpackImage:
;
;Extract gif image, code start in CX, pixelsize in DX.
;
	push	ecx
	push	edx
	mov	ax,1
	shl	ax,cl
	mov	w[Gd_ClearCode],ax
	inc	ax
	mov	w[Gd_Eoi],ax
	inc	cx
	mov	w[Gd_ReqCt],cx
	mov	cx,w[Gd_ClearCode]
	call	@@InitCodeTable
	mov	w[Gd_OldCode],-1
	mov	w[Gd_Done],0
	mov	w[Gd_Pass],0
	mov	ax,w[Gd_ImageWide]
	mov	w[Gd_RowCnt],ax
	mov	w[Gd_XLoc],0
	mov	w[Gd_YLoc],0
	pop	edx
	pop	ecx
@@extimg1:	push	ecx
	push	edx
	mov	dx,w[Gd_ReqCt]
	call	@@GetCode
	mov	w[Gd_Code],ax
	pop	edx
	pop	ecx
	cmp	ax,w[Gd_ClearCode]
	jne	@@extimg2
	push	ecx
	push	edx
	mov	cx,w[Gd_ClearCode]
	call	@@InitCodeTable
	pop	edx
	pop	ecx
	mov	ax,cx
	inc	ax
	mov	w[Gd_ReqCt],ax
	mov	w[Gd_OldCode],-1
	jmp	@@extimg7
@@extimg2:	cmp	ax,w[Gd_Eoi]
	jne	@@extimg3
	call	@@FlushIn
	mov	w[Gd_Done],-1
	jmp	@@extimg7
@@extimg3:	movzx	ebx,w[Gd_Code]
	shl	ebx,1
	mov	ebp,d[Gd_CtLink]
	cmp	ds:w[ebp+ebx],-2
	je	@@extimg5
	cmp	w[Gd_OldCode],-1
	je	@@extimg4
	push	ecx
	push	edx
	movzx	ebx,w[Gd_Code]
	mov	cx,w[Gd_OldCode]
	call	@@InsertCode
	pop	edx
	pop	ecx
@@extimg4:	jmp	@@extimg6
@@extimg5:	push	ecx
	push	edx
	movzx	ebx,w[Gd_OldCode]
	mov	cx,w[Gd_OldCode]
	call	@@InsertCode
	pop	edx
	pop	ecx
@@extimg6:	push	ecx
	push	edx
	movzx	ebx,w[Gd_Code]
	call	@@PutX
	pop	edx
	pop	ecx
	mov	ax,w[Gd_Code]
	mov	w[Gd_OldCode],ax
@@extimg7:	cmp	w[Gd_Done],0
	jne	@@extimg8
	jmp	@@extimg1
@@extimg8:	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@InitCodeTable:
;
;Initialise the code table, clear code in CX.
;
	sub	ebx,ebx		;start with clear code in bx
	mov	ax,cx		;next code will be the 
	add	ax,2		;clear code +2
	mov	w[Gd_NextCode],ax
	mov	ax,cx		;next limit will be
	shl	ax,1		;clear code times two
	mov	w[Gd_NextLim],ax
	movzx	ecx,cx
@@inittable1:	cmp	ebx,ecx
	jge	@@inittable2
	mov	ebp,Gd_CtFirst
	mov	ds:b[ebp+ebx],bl
	mov	ebp,Gd_CtLast
	mov	ds:b[ebp+ebx],bl
	shl	ebx,1
	mov	ebp,d[Gd_CtLink]
	mov	ds:w[ebp+ebx],-1
	shr	ebx,1
	inc	ebx
	jmp	@@inittable1
@@inittable2:	cmp	ebx,4096
	jge	@@inittable3
	shl	ebx,1
	mov	ebp,d[Gd_CtLink]
	mov	ds:w[ebp+ebx],-2
	shr	ebx,1
	inc	ebx
	jmp	@@inittable2
@@inittable3:	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@DoPixel:
;
;Does one pixel, value in AX
;
	movzx	ebx,w[Gd_XLoc]
	add	ebx,Gd_OutRow
	mov	b[ebx],al
	inc	w[Gd_XLoc]
	dec	w[Gd_RowCnt]
	cmp	w[Gd_RowCnt],0
	jne	@@dopixel3
	;
	;Copy the line into the output bitmap.
	;
	movzx	ebx,w[Gd_YLoc]
	mov	edi,_GIF_Bitmap
	mov	eax,BM_Wide[edi]
	mul	ebx
	add	eax,edi
	add	eax,size BM
	mov	edi,eax
	movzx	ecx,w[Gd_ImageWide]
	mov	esi,Gd_Outrow
	rep_movsb
	;
	mov	w[Gd_XLoc],0
	mov	ax,w[Gd_ImageWide]
	mov	w[Gd_RowCnt],ax
	cmp	w[Gd_Interlaced],0
	je	@@dopixel2
	movzx	ebx,w[Gd_Pass]
	shl	ebx,1
	mov	ax,w[Gd_IncTable+ebx]
	add	w[Gd_YLoc],ax
	mov	ax,w[Gd_YLoc]
	cmp	ax,w[Gd_ImageDeep]
	jl	@@dopixel1
	inc	w[Gd_Pass]
	movzx	ebx,w[Gd_Pass]
	shl	ebx,1
	mov	ax,w[Gd_StartTable+ebx]
	mov	w[Gd_YLoc],ax
@@dopixel1:	jmp	@@dopixel3
@@dopixel2:	inc	w[Gd_YLoc]
	mov	ax,w[Gd_YLoc]
	cmp	ax,w[Gd_ImageDeep]
	jl	@@dopixel3
	mov	w[Gd_YLoc],0
@@dopixel3:	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@PutX:
;
;Puts a code into the table.
;
	sub	ecx,ecx
	mov	esi,Gd_OStack
@@putx1:	mov	ebp,Gd_CtLast
	mov	al,ds:b[ebp+ebx]
	mov	[esi],al
	inc	esi
	inc	ecx
	shl	ebx,1
	mov	ebp,d[Gd_CtLink]
	movzx	ebx,ds:w[ebp+ebx]
	cmp	bx,-1
	jne	@@putx1
	cmp	dx,1
	jne	@@putx3
@@putx2:	dec	esi
	mov	al,[esi]
	and	ax,1
	push	ecx
	push	esi
	call	@@DoPixel
	pop	esi
	mov	al,[esi]
	and	ax,0ffh
	shr	ax,1
	push	esi
	call	@@DoPixel
	pop	esi
	pop	ecx
	dec	ecx
	jnz	@@putx2
	jmp	@@putx4
@@putx3:	dec	esi
	mov	al,[esi]
	and	ax,0ffh
	push	ecx
	push	esi
	call	@@DoPixel
	pop	esi
	pop	ecx
	dec	ecx
	jnz	@@putx3
@@putx4:	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@InsertCode:
;
;code in bx, oldcode in cx, csizeptr in ES:DI
;
	push	ebx
	movzx	ebx,w[Gd_NextCode]
	shl	ebx,1
	mov	ebp,d[Gd_CtLink]
	mov	ds:w[ebp+ebx],cx
	pop	ebx
	push	ebx
	mov	ebp,Gd_CtFirst
	mov	al,ds:b[ebp+ebx]
	movzx	ebx,w[Gd_NextCode]
	mov	ebp,Gd_CtLast
	mov	ds:b[ebp+ebx],al
	movzx	ebx,cx
	mov	ebp,Gd_CtFirst
	mov	al,ds:b[ebp+ebx]
	movzx	ebx,w[Gd_NextCode]
	mov	ebp,Gd_CtFirst
	mov	b[ebp+ebx],al
	pop	ebx
	inc	w[Gd_NextCode]
	mov	ax,w[Gd_NextCode]
	cmp	ax,w[Gd_NextLim]
	jne	@@insertcode2
	cmp	w[Gd_ReqCt],12
	jnl	@@insertcode2
	inc	w[Gd_ReqCt]
	shl	w[Gd_NextLim],1
@@insertcode2:	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@FlushIn:
;
;Flushes the input.
;
@@flushin1:	cmp	w[Gd_BufCt],0
	je	@@flushin2
	call	@@FgetC
	dec	w[Gd_BufCt]
	jmp	@@flushin1
@@flushin2:	call	@@FGetC
	mov	w[Gd_BufCt],ax
	cmp	w[Gd_BufCt],0
	jne	@@flushin1
	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@GetCode:
;
;gets a code, ReqCt is in DX...
;
	cmp	dx,8
	jg	@@getcode1
	push	dx
	call	@@GetBCode
	pop	dx
	jmp	@@getcode2
@@getcode1:	push	dx
	mov	dx,8
	call	@@GetBCode
	pop	dx
	push	ax
	push	dx
	sub	dx,8
	call	@@GetBCode
	pop	dx
	mov	cx,8
	shl	ax,cl
	pop	bx
	or	ax,bx
@@getcode2:	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@GetbCode:
;
;Gets a B code, REQCT is in DX...
;
	cmp	w[Gd_RemCt],0
	jne	@@getbcode1
	call	@@GetGb
	mov	w[Gd_Rem],ax
	mov	w[Gd_RemCt],8
@@getbcode1:	mov	ax,dx
	cmp	w[Gd_RemCt],ax
	jnl	@@getbcode2
	call	@@GetGb
	mov	cx,w[Gd_RemCt]
	shl	ax,cl
	or	w[Gd_Rem],ax
	add	w[Gd_RemCt],8
@@getbcode2:	movzx	ebx,dx
	mov	al,b[Gd_Cmask+ebx]
	and	ax,0ffh
	mov	bx,w[Gd_Rem]
	and	bx,ax
	push	bx
	sub	w[Gd_RemCt],dx
	mov	cx,dx
	shr	w[Gd_Rem],cl
	pop	ax
	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@GetGb:
;
;Gets the buffer length?
;
	cmp	w[Gd_BufCt],0
	jne	@@GetGb2
	call	@@FGetC
	mov	w[Gd_BufCt],ax	;save the size.
	cmp	w[Gd_BufCt],-1	;if its eof
	je	@@getgb1
	cmp	w[Gd_BufCt],0	;or zero length.
	je	@@getgb1		;go handle the error.
	jmp	@@getgb2
@@getgb1:	jmp	@@form_error
@@getgb2:	call	@@FgetC		;get the byte
	cmp	ax,-1		;if it's Eof
	jne	@@getgb3
	jmp	@@form_error
@@getgb3:	dec	w[Gd_BufCt]		;say we got this byte.
	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@GetColours:
;
;Gets colours from Gif file, Psize in CX.
;
	mov	ax,1		;shift right by psize
	shl	ax,cl
	mov	cx,ax		;ax is number of colours.
	sub	ebx,ebx
	push	cx
@@GetColour1:	push	cx
	call	@@FgetC
	mov	[Palette+ebx+0],al
	call	@@FgetC
	mov	[Palette+ebx+1],al
	call	@@FgetC
	mov	[Palette+ebx+2],al
	add	ebx,3
	pop	cx
	dec	cx
	jnz	@@GetColour1
	pop	cx
	mov	esi,offset Palette
	movzx	ecx,cx
	mov	eax,ecx
	shl	ecx,1
	add	ecx,eax
@@Scale:	shr	b[esi],2
	inc	esi
	loop	@@Scale
	ret


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@FGetW:
;
;Get a word from the file.
;
	call	@@FGetc
	mov	bx,ax
	call	@@FGetc
	mov	bh,al
	mov	ax,bx
	ret

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
@@FGetc:
;
;Get a byte in desired format from file.
;
	call	ReadBufferByte	;read byte from the file.
	or	ax,ax
	mov	ah,0		;reset ah.
	jnz	@@0		;any error?
	mov	ax,-1		;pretend end of data.
@@0:	ret
LoadGIF	endp


	sdata
;
Gd_CMask	db 0,1,3,7,15,31,63,127,255
Gd_IncTable	dw 8,8,4,2,0
Gd_StartTable	dw 0,4,2,1,0
;
Gd_StartInit	label byte
_GIF_Bitmap	dd ?
Gd_OutRow	dd ?	;db 2048 dup (0)
Gd_Ostack	dd ?	;db 4096 dup (0)
Gd_CtFirst	dd ?	;db 4096 dup (0)
Gd_CtLast	dd ?	;db 4096 dup (0)
Gd_CtLink	dd ?	;dw 4096 dup (0)
Palette	db 256*3 dup (?)
Gd_Interlaced	dw 0
Gd_ImageWide	dw 0
Gd_ImageDeep	dw 0
Gd_ImageX	dw 0
Gd_ImageY	dw 0
Gd_XLoc	dw 0
Gd_YLoc	dw 0
Gd_ScreenWidth	dw 0
Gd_ScreenHeight dw 0
Gd_BackGround	dw 0
Gd_Eoi	dw 0
Gd_Rem	dw 0
Gd_RemCt	dw 0
Gd_BufCt	dw 0
Gd_NextLim	dw 0
Gd_NextCode	dw 0
Gd_RowCnt	dw 0
Gd_Pass	dw 0
Gd_ClearCode	dw 0
Gd_ReqCt	dw 0
Gd_Done	dw 0
Gd_Code	dw 0
Gd_OldCode	dw 0
Gd_upGd_sflags	dw 0
Gd_upGd_gpix	dw 0
Gd_upGd_pixelsize dw 0
Gd_upGd_alldone dw 0
Gd_upGd_work	dw 0
Gd_upGd_iflags	dw 0
Gd_upGd_done	dw 0
Gd_Handle	dw 0
Gd_EndInit	label byte
Gd_ErrorStack	dd 0


	efile
	end
