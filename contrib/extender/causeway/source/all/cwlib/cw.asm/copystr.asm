	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Copy a string from one place to another.
;
;On Entry:
;
;ESI	- Source string.
;EDI	- Destination string.
;
;On Exit:
;
;ALL registers preserved.
;
CopyString	proc	near
	pushm	eax,ecx,esi,edi
	push	esi
	xchg	esi,edi
	or	ecx,-1
	xor	al,al
	repnz	scasb		;get the strings length.
	inc	ecx
	neg	ecx
	xchg	esi,edi
	pop	esi
	rep	movsb		;copy the string.
	popm	eax,ecx,esi,edi
	ret
CopyString	endp


	efile
	end

