	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Work out the pixel length of a string.
;
;On Entry:
;
;ESI	- String to scan.
;EDI	- Font to use.
;
;On Exit:
;
;EAX	- Length in pixels.
;
;ALL other registers preserved.
;
StringLenPixels proc near
	pushm	ebx,ecx,esi,edi
	xor	ecx,ecx
@@1:	movzx	eax,b[esi]
	or	al,al
	jz	@@0
	inc	esi
	mov	ebx,[edi+eax*4]	;point to this character.
	add	ecx,BM_Wide[ebx]
	jmp	@@1
@@0:	mov	eax,ecx
	popm	ebx,ecx,esi,edi
	ret
StringLenPixels endp


	efile
	end

