	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Find closest match in HardwarePalette to RGB value specified. Must be more
;accurate ways of doing it than this but I don't know about them!
;
;On Entry:
;
;EAX	- RGB value.
;
;On Exit:
;
;EAX	- Nearest pen value in HardwarePalette
;
SearchRGB:
	public SearchRGB
_SearchRGB_	proc	syscall
	local @@Pen:byte, @@Chosen:byte, @@Count:dword, @@Data:dword
	pushad
	mov	@@Pen,0
	mov	@@Chosen,0
	mov	@@Count,256
;
;Put bytes into right order for memory variable.
;
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	@@Data,eax
	lea	edi,@@Data
;
;Scale 0-255 values to suit VGA DAC 0-63 values.
;
	shr	b[edi+0],2
	shr	b[edi+1],2
	shr	b[edi+2],2
;
;Now find the nearest match.
;
	mov	esi,offset HardwarePalette
	mov	ecx,0FFFFFFh
l0:	xor	edx,edx
	movzx	eax,b[0+esi]
	movzx	ebx,b[0+edi]
	sub	eax,ebx
	jns	l1
	neg	eax
l1:	add	eax,eax
	add	edx,eax
	;
	movzx	eax,b[1+esi]
	movzx	ebx,b[1+edi]
	sub	eax,ebx
	jns	l2
	neg	eax
l2:	add	eax,eax
	add	edx,eax
	;
	movzx	eax,b[2+esi]
	movzx	ebx,b[2+edi]
	sub	eax,ebx
	jns	l3
	neg	eax
l3:	add	eax,eax
	add	edx,eax
	;
	cmp	edx,ecx
	jnc	l4
	mov	ecx,edx
	mov	al,@@Pen
	mov	@@Chosen,al
l4:	add	esi,3
	inc	@@pen
	dec	@@Count
	jnz	l0
	;
	popad
	movzx	eax,@@Chosen
	ret
_SearchRGB_	endp


	efile
	end


