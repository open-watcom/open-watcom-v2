	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Build a text list of modes in the buffer specified.
;
;Calling:
;
;BuildModeList(buffer);
;
;On Entry:
;
;buffer	- buffer to use.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
_BuildModeList	proc	syscall buffer:dword
	push	esi
	mov	esi,buffer
	call	BuildModeList
	pop	esi
	ret
_BuildModeList	endp


;-------------------------------------------------------------------------
;
;Build a text list of modes in the buffer specified.
;
;On Entry:
;
;ESI	- buffer to use.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
BuildModeList	proc	near
	pushad
	mov	edi,esi
	mov	esi,offset ValidModeList
@@0:	lodsw
	cmp	ax,-1
	jz	@@8
	movzx	eax,ax
	push	esi
	;
	cmp	ax,13h
	jnz	@@VESA
	push	edi
	mov	edi,offset ModeInfoBuffer
	mov	MIB_ModeAttributes[edi],16+2
	mov	MIB_BitsPerPixel[edi],8
	mov	MIB_XResolution[edi],320
	mov	MIB_YResolution[edi],200
	pop	edi
	jmp	@@GotInfo
	;
@@VESA:	push	eax
	push	edi
	mov	cx,ax
	mov	ax,4f01h
	mov	edi,offset ModeInfoBuffer
	int	10h
	pop	edi
	pop	eax
@@GotInfo:	;
	mov	esi,offset ModeInfoBuffer
	test	MIB_ModeAttributes[esi],16
	jz	@@7
	;
	test	MIB_ModeAttributes[esi],2
	jz	@@7
	;
	push	eax
	movzx	eax,MIB_BitsPerPixel[esi]
	cmp	eax,8
	pop	eax
	jc	@@7
	;
	stosd
	;
	mov	esi,offset Forced32kList
	mov	bx,15
	mov	cx,ax
@@32k:	lodsw
	cmp	ax,cx
	jz	@@Forced
	cmp	ax,-1
	jnz	@@32k
	mov	esi,offset Forced64kList
	inc	bx
@@64k:	lodsw
	cmp	ax,cx
	jz	@@Forced
	cmp	ax,-1
	jnz	@@64k
	mov	esi,offset ModeInfoBuffer
	movzx	bx,MIB_BitsPerPixel[esi]
@@Forced:	mov	esi,offset ModeInfoBuffer
	mov	MIB_BitsPerPixel[esi],bl
	;
	movzx	eax,MIB_XResolution[esi]
	call	Bin2Dec
	mov	b[edi],"x"
	inc	edi
	;
	movzx	eax,MIB_YResolution[esi]
	call	Bin2Dec
	mov	b[edi],"x"
	inc	edi
	;
	movzx	eax,MIB_BitsPerPixel[esi]
	mov	ebx," M61"
	cmp	eax,24
	jnc	@@3
	mov	ebx," K46"
	cmp	eax,16
	jz	@@3
	mov	ebx," K23"
	cmp	eax,15
	jz	@@3
	mov	ebx," 652"
	cmp	eax,8
	jnc	@@3
	mov	ebx,"  61"
	cmp	eax,4
	jnc	@@3
	mov	ebx,"   4"
	cmp	eax,2
	jnc	@@3
	mov	ebx,"   2"
@@3:	mov	eax,ebx
	stosd
	dec	edi
	xor	al,al
	stosb
	;
@@7:	pop	esi
	jmp	@@0
	;
@@8:	mov	eax,-1
	stosd
	popad
	ret
BuildModeList	endp

	efile

	end


