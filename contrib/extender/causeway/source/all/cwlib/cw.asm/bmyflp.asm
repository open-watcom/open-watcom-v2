	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Flip a bitmap from top to bottom, eg, around X axis, but affecting it in the Y
;
;On Entry:
;
;ESI	- bitmap
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
BitmapYFlip	proc	near
	public BitmapYFlip
	pushad
	mov	eax,BM_PWide[esi]
	mul	BM_Wide[esi]
	mov	ecx,eax		;Get line length.
	mul	BM_Depth[esi]
	add	eax,esi
	add	eax,size BM
	sub	eax,ecx
	mov	edi,eax		;Point to last line.
	mov	edx,BM_Depth[esi]
	shr	edx,1		;get depth to do.
	or	edx,edx
	jz	@@2
	add	esi,size BM		;point to first line.
@@0:	pushm	ecx,esi,edi
@@1:	mov	al,[esi]		;swap the data round.
	xchg	al,[edi]
	mov	[esi],al
	inc	esi
	inc	edi
	loop	@@1		;do whole line.
	popm	ecx,esi,edi
	add	esi,ecx		;next line down.
	sub	edi,ecx		;next line up.
	dec	edx
	jnz	@@0		;do all lines.
@@2:	popad
	ret
BitmapYFlip	endp

	efile
	end


