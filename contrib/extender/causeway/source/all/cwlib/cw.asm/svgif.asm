	include ..\cwlib.inc
	scode


_GIF_structsize equ	32
_GIF_hashfirst	equ	0
_GIF_hashnext	equ	2
_GIF_hashchar	equ	4


;------------------------------------------------------------------------------
;
;Write an image into GIF format.
;
;On Entry:-
;
;EAX	- Flags, ignored.
;EDX	- File name to use.
;ESI	- Bitmap pointer.
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
SaveGif	proc	near
	pushad
	mov	_GIF_stackstore,esp
	mov	_GIF_flags,eax
	mov	_GIF_bitmap,esi
	;
	mov	edi,offset _GIF_StartInit
	mov	ecx,offset _GIF_EndInit
	sub	ecx,edi
	xor	al,al
	rep	stosb
	;
	cmp	BM_PWide[esi],1
	jnz	@@form_error
	;
	call	CreateFile
	jc	@@file_error
	mov	_GIF_handle,ebx
	;
	mov	ecx,20480
	call	Malloc
	jc	@@mem_error
	mov	_GIF_Hash,esi
	mov	edi,esi
	xor	al,al
	rep_stosb
	;
	mov	esi,_GIF_bitmap
	add	esi,size BM
	mov	_GIF_ImageBuffer,esi
	;
	mov	esi,_GIF_bitmap
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mul	BM_Depth[esi]
	add	eax,size BM
	add	esi,eax
	mov	edi,offset _GIF_Palette
	mov	ecx,256*3
@@gp0:	lodsb
	mov	ah,al
	shl	al,2
	and	ah,1
	or	al,ah
	shl	ah,1
	or	al,ah
	stosb
	loop	@@gp0
	;
	mov	esi,_GIF_bitmap
	mov	eax,BM_Wide[esi]
	mov	_GIF_ScreenWide,ax
	mov	_GIF_ImageWide,ax
	mov	eax,BM_Depth[esi]
	mov	_GIF_ScreenDeep,ax
	mov	_GIF_ImageDeep,ax
	mov	_GIF_Bits,8
	mov	_GIF_X,0
	mov	_GIF_Y,0
	;
	mov	cx,_GIF_Bits		;get number of bits and work out
	mov	ax,1		;number of colours
	shl	ax,cl
	mov	_GIF_Clear,ax
	inc	ax
	mov	_GIF_Eoi,ax
	inc	ax
	mov	_GIF_FirstFree,ax
	call	@@Header		;write the header.
	jc	@@file_error
	call	@@Compress		;write the gif data.
	call	@@Footer		;clean up.
	jc	@@file_error
	xor	eax,eax
	jmp	@@exit
	;
@@file_error:	mov	eax,1
	jmp	@@exit
@@mem_error:	mov	eax,2
	jmp	@@exit
@@form_error:	mov	eax,3
	jmp	@@exit
	;
@@exit:	mov	ebx,eax
	xchg	ebx,_GIF_Handle
	or	ebx,ebx
	jz	@@closed
	call	CloseFile
@@closed:	xor	esi,esi
	xchg	esi,_GIF_Hash
	or	esi,esi
	jz	@@freed
	call	Free
@@freed:	mov	esp,_GIF_StackStore
	popad
	mov	eax,_GIF_Handle
	ret


;------------------------------------------------------------------------------
;
;Write the header.
;
@@Header:	mov	edx,offset _GIF_Head
	mov	ecx,6
	call	@@FWrite
	mov	ax,_GIF_ScreenWide
	call	@@FPutW
	mov	ax,_GIF_ScreenDeep
	call	@@FPutW
	mov	ax,_GIF_Bits
	dec	ax
	or	al,90h
	call	@@FPutC
	mov	ax,0	;_GIF_Background
	call	@@FPutC
	mov	al,0
	call	@@FPutC
	mov	ax,1
	mov	cx,_GIF_Bits
	shl	ax,cl
	mov	cx,3
	mul	cx
	mov	cx,ax
	mov	edx,offset _GIF_Palette
	call	@@FWrite
	mov	al,','
	call	@@FPutC
	mov	ax,0	;_GIF_ImageLeft
	call	@@FPutW
	mov	ax,0	;_GIF_ImageTop
	call	@@FPutW
	mov	ax,_GIF_ImageWide
	call	@@Fputw
	mov	ax,_GIF_ImageDeep
	call	@@Fputw
	mov	ax,_GIF_Bits
	dec	ax
	call	@@Fputc		;write local flag.
	mov	ax,_GIF_Bits
	call	@@Fputc		;initial code size.
	ret


;------------------------------------------------------------------------------
;
;Tidy up after finished writing data.
;
@@Footer:	mov	al,0
	call	@@FPutc		;write 0 length block.
	mov	al,3bh
	call	@@FPutc		;write end of file marker.
	ret


;------------------------------------------------------------------------------
;
;Compress file pointed to.
;
@@Compress:	call	@@InitTable
	mov	ax,_GIF_Clear
	call	@@WriteCode
	call	@@FGetch
@@l4:	xor	ah,ah
@@l4a:	mov	_GIF_PrefixCode,ax
	call	@@FGetch
	jc	@@l17
	mov	_GIF_ThisByte,al
	mov	bx,_GIF_PrefixCode
	call	@@LookUpCode
	jnc	@@l4a
	call	@@AddCode
	push	bx
	mov	ax,_GIF_PrefixCode
	call	@@WriteCode
	pop	bx
	mov	al,_GIF_ThisByte
	cmp	bx,_GIF_MaxCode
	jl	@@l4
	cmp	_GIF_NBits,12
	jl	@@l14
	mov	ax,_GIF_Clear
	call	@@WriteCode
	call	@@InitTable
	mov	al,_GIF_ThisByte
	jmp	@@l4
@@l14:	inc	_GIF_NBits
	shl	_GIF_MaxCode,1
	jmp	@@l4
@@l17:	mov	ax,_GIF_PrefixCode
	call	@@WriteCode
	mov	ax,_GIF_Eoi
	call	@@WriteCode
	mov	ax,_GIF_BitOffset
	cmp	ax,0
	je	@@l18
	mov	cx,8
	xor	dx,dx
	div	cx
	or	dx,dx
	je	@@l17a
	inc	ax
@@l17a:	call	@@Flush
@@l18:	ret


;------------------------------------------------------------------------------
;
;Initialise the table.
;
@@InitTable:	mov	ax,_GIF_Bits
	inc	ax
	mov	_GIF_NBits,ax
	mov	ax,_GIF_Clear
	shl	ax,1
	mov	_GIF_MaxCode,ax
	movzx	eax,_GIF_Clear
	mov	ecx,5
	mul	ecx
	mov	ecx,eax
	mov	ax,-1
	mov	edi,_GIF_Hash
	rep	stosw
	mov	ax,_GIF_FirstFree
	mov	_GIF_FreeCode,ax
	ret


;------------------------------------------------------------------------------
;
;Write one code.
;
@@WriteCode:	push	ax
	mov	ax,_GIF_BitOffset
	mov	cx,_GIF_NBits
	add	_GIF_BitOffset,cx
	mov	cx,8
	xor	dx,dx
	div	cx
	cmp	ax,255-4
	jl	@@wc1
	call	@@Flush
	push	dx
	add	dx,_GIF_NBits
	mov	_GIF_BitOffset,dx
	pop	dx
	movzx	eax,ax
	add	eax,offset _GIF_OutputData
	mov	esi,eax
	mov	al,[esi]
	mov	_GIF_OutputData,al
	xor	ax,ax
@@wc1:	movzx	eax,ax
	add	eax,offset _GIF_OutputData
	mov	edi,eax
	pop	ax
	mov	cx,dx
	xor	dx,dx
	jcxz	@@wc3
@@wc2:	shl	ax,1
	rcl	dx,1
	dec	cx
	jnz	@@wc2
	or	al,[edi]
@@wc3:	stosw
	mov	al,dl
	stosb
	ret


;------------------------------------------------------------------------------
;
;Flush buffer.
;
@@Flush:	push	ax
	push	ebx
	push	cx
	push	edx
	mov	_GIF_OneByte,al
	mov	ebx,_GIF_Handle
	mov	edx,offset _GIF_OneByte
	mov	cx,ax
	inc	cx
	mov	ax,4000h
	int	21h
	pop	edx
	pop	cx
	pop	ebx
	pop	ax
	ret


;------------------------------------------------------------------------------
;
;Fetch one byte from input file.
;
@@FGetch:	cmp	_GIF_X,0		;had a lines worth?
	jnz	@@0
	push	bx
	mov	bx,_GIF_Y
	inc	bx
	cmp	bx,_GIF_ImageDeep	;bottom of image?
	pop	bx
	jle	@@1
	stc			;flag end of data.
	ret
	;
@@1:	push	cx
	mov	cx,_GIF_ImageWide	;image width.
	mov	_GIF_X,cx		;set new count value.
	inc	_GIF_Y		;update Y position for next time.
	pop	cx
	;
@@0:	push	esi
	mov	esi,_GIF_ImageBuffer
	inc	_GIF_ImageBuffer
	mov	al,[esi]
	dec	_GIF_X
	pop	esi
	clc
	ret


;------------------------------------------------------------------------------
;
;Find code in the table.
;
@@LookUpCode:	call	@@Index
	mov	edi,0
	cmp	b[esi+_GIF_HashFirst],-1
	je	@@gc4
	inc	edi
	mov	bx,w[esi+_GIF_HashFirst]
@@gc2:	call	@@Index
	cmp	b[esi+_GIF_HashChar],al
	jne	@@gc3
	clc
	mov	ax,bx
	ret
@@gc3:	cmp	w[esi+_GIF_HashNext],-1
	je	@@gc4
	mov	bx,w[esi+_GIF_HashNext]
	jmp	@@gc2
@@gc4:	stc
	ret


;------------------------------------------------------------------------------
;
;Index into table.
;
@@Index:	movzx	ebx,bx
	mov	esi,ebx
	shl	esi,1
	shl	esi,1
	add	esi,ebx
	add	esi,_GIF_Hash
	ret


;------------------------------------------------------------------------------
;
;Add a code to the table.
;
@@AddCode:	mov	bx,_GIF_FreeCode
	cmp	di,0
	je	@@ac1
	mov	w[esi+_GIF_HashNext],bx
	jmp	@@ac2
@@ac1:	mov	w[esi+_GIF_HashFirst],bx
@@ac2:	cmp	bx,4096
	je	@@ac3
	call	@@Index
	mov	w[esi+_GIF_HashFirst],-1
	mov	w[esi+_GIF_HashNext],-1
	mov	b[esi+_GIF_HashChar],al
	inc	w[_GIF_FreeCode]
@@ac3:	ret


;------------------------------------------------------------------------------
;
;Write to a file.
;
@@FWrite:	mov	ebx,_GIF_Handle
	mov	ax,4000h
	int	21h
	ret


;------------------------------------------------------------------------------
;
;Write's one byte in al to a file.
;
@@FPutC:	mov	b[_GIF_OneByte],al
	mov	ax,4000h
	mov	ebx,_GIF_Handle
	mov	cx,1h
	mov	edx,offset _GIF_OneByte
	int	21h
	ret


;------------------------------------------------------------------------------
;
;Write's a word to file.
;
@@FPutW:	push	ax
	call	@@FPutc
	pop	ax
	mov	al,ah
	call	@@Fputc
	ret
SaveGif	endp


	sdata
;
_GIF_Head	db 'GIF87a'
_GIF_Palette	db 256*3 dup (?)
_GIF_ScreenWide dw ?
_GIF_ScreenDeep dw ?
_GIF_ImageWide	dw ?
_GIF_ImageDeep	dw ?
_GIF_Bits	dw ?
;
_GIF_StackStore dd 0
_GIF_Flags	dd ?
_GIF_Bitmap	dd ?
;
_GIF_StartInit	label byte
_GIF_Handle	dd 0
_GIF_ImageBuffer dd 0
_GIF_Hash	dd 0
_GIF_ImageLeft	dw 0
_GIF_ImageTop	dw 0
_GIF_X	dw 0
_GIF_Y	dw 0
_GIF_Clear	dw 0
_GIF_Eoi	dw 0
_GIF_FirstFree	dw 0
_GIF_PrefixCode dw 0
_GIF_FreeCode	dw 0
_GIF_MaxCode	dw 0
_GIF_NBits	dw 0
_GIF_ThisByte	db 0
_GIF_BitOffset	dw 0
_GIF_OneByte	db 0
_GIF_outputdata db 255 dup (?)
_GIF_EndInit	label byte


	efile
	end

