	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Work out length of a string.
;
;On Entry:
;
;ESI	- pointer to string.
;
;On Exit:
;
;EAX	- length of string.
;
;ALL other registers preserved.
;
LenString	proc	near
	pushm	edi,ecx
	mov	edi,esi
	or	ecx,-1
	xor	al,al
	repnz	scasb		;get the strings length.
	mov	eax,ecx
	not	eax
	dec	eax		;Don't include terminator.
	popm	edi,ecx
	ret
LenString	endp


	efile
	end

